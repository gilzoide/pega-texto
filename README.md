# pega-texto

Single-file [Parsing Expression Grammars (PEG)](http://bford.info/packrat/) runtime engine for C.

To use it, copy [pega-texto.h](pega-texto.h) file to your project and `#define PEGA_TEXTO_IMPLEMENTATION`
before including it in **one** C or C++ source file to create the implementation.


## Usage example

```c
#include <stdio.h>

// 1. #define PEGA_TEXTO_IMPLEMENTATION on exactly one C/C++ file and include pega-texto.h
// Optionally #define other compile-time options, check out pega-texto.h for documentation
#define PT_DEFINE_SHORTCUTS  // shortcuts make creating grammars more readable
#define PT_DATA size_t  // define action result data
#define PEGA_TEXTO_IMPLEMENTATION
#include "pega-texto.h"

// This will return the length for each line we parse
PT_DATA line_length(const char *str, size_t size, int argc, PT_DATA *argv, void *userdata) {
    return size;
}
// This will return the longest line length
PT_DATA longest_line(const char *str, size_t size, int argc, PT_DATA *argv, void *userdata) {
    size_t longest = 0;
    for(int i = 0; i < argc; i++) {
        if(argv[i] > longest) {
            longest = argv[i];
        }
    }
    return longest;
}

// Helpers for getting error locations
typedef struct text_location { int line; int column; } text_location;
text_location get_text_location(const char *str, size_t where) {
    text_location location = { 1, 1 };
    for(size_t i = 0; i < where; i++) {
        if(str[i] == '\n') {
            location.line++;
            location.column = 1;
        }
        else {
            location.column++;
        }
    }
    return location;
}
// This will be called when matching an invalid quotation mark
void invalid_quote(const char *str, size_t where, void *userdata) {
    text_location location = get_text_location(str, where);
    printf("Error: invalid quotation mark in middle of cell at %d:%d\n", location.line, location.column);
}
// This will be called when no quotation mark is found closing an open quoted cell
void unmatched_quote(const char *str, size_t where, void *userdata) {
    while(where > 1 && !(str[where] == '"' && str[where - 1] != '"')) where--;
    text_location location = get_text_location(str, where);
    printf("Error: Quotation mark starting cell at %d:%d is not closed\n", location.line, location.column);
}

// 2. Create some grammar rules, which are arrays of expressions
// Defining indices in an `enum` makes referencing rules clearer
enum rule_indices {
    R_CSV,
    R_LINE,
    R_CELL,
    R_QUOTED,
    R_EOL,
};
// Rules must have a terminating `PT_END()` operation,
// using `PT_RULE(...)` ensures we don't forget about it
pt_rule CSV = PT_RULE(
    // after the whole match succeeds, `longest_line` will be called,
    // receiving all inner `line_length` results on argv
    ACTION(longest_line,
        ZERO_OR_MORE(CALL(R_LINE))
    )
);
pt_rule LINE = PT_RULE(
    // after the whole match succeeds, `line_length` will be called for each line
    ACTION(line_length,
        CALL(R_CELL),
        ZERO_OR_MORE(B(','), CALL(R_CELL))
    ),
    EITHER(
        CALL(R_EOL),
        B('\0') // EOF
    )
);
pt_rule CELL = PT_RULE(
    EITHER(
        CALL(R_QUOTED),
        ONE_OR_MORE(
            ERROR_IF(invalid_quote, B('"')),
            ANY_BUT(S("\",\r\n"))
        )
    )
);
pt_rule QUOTED = PT_RULE(
    B('"'),
    ZERO_OR_MORE(
        EITHER(
            L("\"\""),
            ANY_BUT(B('"'))
        )
    ),
    EITHER(
        B('"'),
        ERROR(unmatched_quote)
    )
);
pt_rule EOL = PT_RULE(
    OPTIONAL(B('\r')),
    B('\n')
);

// 3. Create your grammar
// Grammars are just arrays of rules
pt_grammar G = {
    [R_CSV] = CSV,
    [R_LINE] = LINE,
    [R_CELL] = CELL,
    [R_QUOTED] = QUOTED,
    [R_EOL] = EOL,
};

int main(int argc, const char **argv) {
    const char *csv_text = "first,second,third\n1,\"2\",3";
    // 4. Call the match algorithm!
    pt_match_result result = pt_match(G, csv_text, NULL);
    switch(result.matched) {
        case PT_NO_MATCH:
            printf("Match failed! Invalid CSV content\n");
            break;

        case PT_NO_STACK_MEM:
            printf("Match failed! Out of memory\n");
            break;

        case PT_MATCHED_ERROR:
            printf("Match failed! Error matched\n");
            break;

        case PT_NULL_INPUT:
            printf("Match failed! NULL CSV content\n");
            break;

        default:
            printf("Matched CSV content with %d bytes.\n", result.matched);
            printf("Longest line is %lu bytes long\n", result.data);
            break;
    }
    return 0;
}

```


Change log
----------
+ 4.0.0 - Refactor project as a single header implementation,
  split Quantifier Expressions into At Least and At Most expressions,
  make `PT_DATA` type configurable, add Action Expressions to
  simplify implementation and make code more readable, reimplement
  match algorithm with a recursive approach, make Expressions be
  layed out contiguously in memory and remove heap based creation
  and destruction of them, make grammar literal definable at
  compile-time.
+ 3.0.0 - Change actions to receive the capture with pointer and size, instead
  of pointer to input string, start and end of capture; add Byte expression,
  change Character Class Expressions to use only functions defined in `ctype.h`,
  change Range Expressions to use 2 bytes instead of a NULL terminated string;
  use Grammars without malloc'ing them.
+ 2.1.0 - Populate `pt_match_result.data.i` with the first syntactic error code
  when syntactic errors occur.
+ 2.0.1 - Put `extern "C"` declarations in inner headers.
+ 2.0.0 - ABI change on `pt_match_options`, included Case Insensitive and
  Character Class Expressions (the old Custom Matcher), changed Custom Matcher
  Expressions to allow operating on strings, also receiving userdata.
+ 1.2.7 - Removed all the Action sequence computation, as Actions are already
  stacked in the right sequence. Running actions is now iterative, O(n) and use
  far less memory.
+ 1.2.6 - Fixed `SEQ` and `OR` expression macros on C++, turns out they behave
  differently about temporary lifetime of arrays.
+ 1.2.5 - Fixed `SEQ` and `OR` expression macros to compile on both C and C++
  using preprocessor macros and `initializer_list` directly on `macro-on.h`.
+ 1.2.4 - Added `extern "C"` declaration on `pega-texto.h` for using in C++. 
+ 1.2.3 - Fixed validation error code emmited when `pt_is_nullable` returned
  true, as it may find an error other than `PT_VALIDATE_LOOP_EMPTY_STRING`.
+ 1.2.2 - Added `NULL` string check on match.
+ 1.2.1 - Fixed validation error on empty `SEQ` and `OR` Expressions, which
  are valid with a `NULL` pointer.
+ 1.2.0 - Macros for Expressions to not own memory buffers, empty `SEQ` and
  `OR` Expressions don't allocate a 0-byte buffer anymore, fixed validation
  error on Non-terminal cycles.
+ 1.1.1 - Fixed validation error on Non-terminal cycles.
+ 1.1.0 - Added basic error handling support.
+ 1.0.0 - Expressions, Grammars, parsing, validation, actions.

