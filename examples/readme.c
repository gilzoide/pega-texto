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

