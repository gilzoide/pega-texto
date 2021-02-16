/** @file
 * pega-texto.h -- Parsing Expression Grammar (PEG) runtime engine
 *
 * Project URL: https://github.com/gilzoide/pega-texto
 *
 * Do this:
 * ```c
 *    #define PEGA_TEXTO_IMPLEMENTATION
 * ```
 * before you include this file in *one* C or C++ file to create the implementation.
 *
 * i.e.:
 * ```c
 *    #include ...
 *    #include ...
 *    #define PEGA_TEXTO_IMPLEMENTATION
 *    #include "pega-texto.h"
 * ```
 *
 * Optionally provide the following defines with your own implementations:
 *
 * - PT_MALLOC(size, userdata):
 *   Your own malloc function (default: `malloc(size)`)
 * - PT_REALLOC(p, size, userdata):
 *   Your own realloc function (default: `realloc(p, size)`)
 * - PT_FREE(p, userdata):
 *   Your own free function (default: `free(p)`)
 * - PT_ASSERT(cond, msg, userdata):
 *   Your own assert function (default: `assert(cond && msg)`)
 * - PT_STATIC:
 *   If defined and PT_DECL is not defined, functions will be declared `static` instead of `extern`
 * - PT_DECL:
 *   Function declaration prefix (default: `extern` or `static` depending on PT_STATIC)
 * - PT_DEFINE_SHORTCUTS:
 *   Define some shorcut macros for building grammars.
 *   They are not prefixed by `PT_`, so beware with define clashes!
 * - PT_ELEMENT_TYPE:
 *   Type that describes each element in input string (default: `const char`)
 * - PT_DATA:
 *   Data type to be returned by Actions (default: simple union with several primitive types)
 */
#ifndef PEGA_TEXTO_H
#define PEGA_TEXTO_H

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#ifndef PT_DECL
    #ifdef PT_STATIC
        #define PT_DECL static
    #else
        #define PT_DECL extern
    #endif
#endif

// Define PT_ELEMENT_TYPE to the string element type, so there can be
// parsers for stuff other than `const char` like `const uint8_t`
#ifndef PT_ELEMENT_TYPE
    typedef const char PT_ELEMENT_TYPE;
#endif

typedef PT_ELEMENT_TYPE *pt_element_string;

#ifdef __cplusplus
extern "C" {
#endif

/// Operations for constructing Parsing Expressions.
enum pt_operation {
    PT_OP_END = 0,
    // Primary
    PT_OP_ELEMENT,          // 'b'
    PT_OP_LITERAL,          // "string"
    PT_OP_CASE_INSENSITIVE, // I"string"
    PT_OP_CHARACTER_CLASS,  // int(char) // If return 0, match fails
                                          // If return non-zero, match succeeds, advance 1
    PT_OP_SET,              // [chars]
    PT_OP_RANGE,            // [c1-c2]
    PT_OP_ANY,              // .
    // Custom match by function
    PT_OP_CUSTOM_MATCHER,   // int(const char *, void *) // Return how many characters were matched
                                                         // Return non-positive values for no match to occur
    // Unary
    PT_OP_NON_TERMINAL,     // <non-terminal> // Recurse to non-terminal expression
    PT_OP_AT_LEAST,         // e^N // Match N or more occurrences of next Expression
    PT_OP_AT_MOST,          // e^-N // Match N or less occurrences of next Expression. Always succeeds
    PT_OP_NOT,              // !e
    PT_OP_AND,              // &e
    // N-ary
    PT_OP_SEQUENCE,         // e1 e2
    PT_OP_CHOICE,           // e1 / e2
    PT_OP_ERROR,            // ERROR // Represents a syntactic error
    PT_OP_ACTION,           // Push an action to the stack

    PT_OP_OPERATION_ENUM_COUNT,
};

/// String literals of the operations.
PT_DECL const char* const pt_operation_names[];

/// Character classes supported by pega-texto.
/// 
/// Each of them correspond to the `is*` functions defined in `ctype.h` header.
enum pt_character_class {
    PT_CLASS_ALNUM  = 'w',
    PT_CLASS_ALPHA  = 'a',
    PT_CLASS_CNTRL  = 'c',
    PT_CLASS_DIGIT  = 'd',
    PT_CLASS_GRAPH  = 'g',
    PT_CLASS_LOWER  = 'l',
    PT_CLASS_PUNCT  = 'p',
    PT_CLASS_SPACE  = 's',
    PT_CLASS_UPPER  = 'u',
    PT_CLASS_XDIGIT = 'x',
};

/// Possible error codes returned by `pt_match`.
typedef enum pt_macth_error_code {
    /// Subject string didn't match the given PEG.
    PT_NO_MATCH = -1,
    /// Error while allocating memory for the Action stack.
    PT_NO_STACK_MEM = -2,
    /// Matched an Error Expression.
    PT_MATCHED_ERROR = -3,
    /// Provided string is a NULL pointer.
    PT_NULL_INPUT = -4,
} pt_macth_error_code;

#ifndef PT_DATA
    /// Default data type for Actions to return.
    ///
    /// Define `PT_DATA` before including "pega-texto.h" to provide your own data type
    /// 
    /// @note This is not a Tagged Union, so you (developer) are responsible for
    /// knowing which type each datum is. This can and should be avoided when
    /// structuring the Grammar.
    typedef union PT_DATA {
        void *p;
        char c;
        unsigned char uc;
        short s;
        unsigned short us;
        int i;
        unsigned int ui;
        long l;
        unsigned long ul;
        long long ll;
        unsigned long long ull;
        ssize_t ssz;
        size_t sz;
        float f;
        double d;
    } PT_DATA;
#endif

/// A function that receives a string and userdata and match it (positive) or not, advancing the matched number.
typedef int (*pt_custom_matcher_function)(pt_element_string, void *);

/// Action to be called for a capture after the whole match succeeds.
/// 
/// Actions will be called only if the whole match succeeds, in the order the
/// Actions Expressions were matched.
/// 
/// Expression Actions reduce inner Actions' result into a single value.
/// 
/// Parameters:
/// - Pointer to the start of the match/capture
/// - Number of bytes contained in the match/capture
/// - Number of #PT_DATA arguments 
/// - #PT_DATA arguments, processed on inner Actions. Currently, this array is
///   reused, so you should not rely on it after your function has returned
/// - User custom data from match options
typedef PT_DATA (*pt_expression_action)(
    pt_element_string str,
    size_t size,
    int argc,
    PT_DATA *argv,
    void *userdata
);

/// Action to be called when an Error Expression is matched (on syntatic errors).
/// 
/// Parameters:
/// - The original subject string
/// - Position where the error was encountered
/// - Error code
/// - User custom data from match options
typedef PT_DATA (*pt_error_action)(
    pt_element_string str,
    size_t where,
    int code,
    void *userdata
);

/// Parsing Expressions.
typedef struct pt_expr {
    /// Operation to be performed
    uint8_t op;
    /// Range byte pair, Literal length, Character Class index.
    uint16_t N;
    /// Literal and Character Set strings, Custom Matcher functions.
    union {
        const void *data;
        const pt_element_string str;
        const pt_custom_matcher_function matcher;
        const pt_expression_action action;
        uintptr_t element;
        uintptr_t index;
        uintptr_t range;
        uintptr_t quantifier;
    };
} pt_expr;

/// Rule typedef, an array of expressions.
typedef pt_expr pt_rule[];
/// Grammar typedef, a 2D array of expressions, or array of Rules.
typedef pt_expr* pt_grammar[];

#define PT_RANGE_PACK(from, to) \
    (((uintptr_t) (from)) | (((uintptr_t) (to)) << (8 * sizeof(uintptr_t) / 2)))
#define PT_RANGE_UNPACK_FROM(r) \
    ((PT_ELEMENT_TYPE) ((r) & ((UINTPTR_MAX) >> (8 * sizeof(uintptr_t) / 2))))
#define PT_RANGE_UNPACK_TO(r) \
    ((PT_ELEMENT_TYPE) ((r) >> (8 * sizeof(uintptr_t) / 2)))

// Ref: https://groups.google.com/g/comp.std.c/c/d-6Mj5Lko_s
#define PT_NARG(...) \
         PT_NARG_(__VA_ARGS__, PT_RSEQ_N())
#define PT_NARG_(...) \
         PT_ARG_N(__VA_ARGS__)
#define PT_ARG_N( \
          _1, _2, _3, _4, _5, _6, _7, _8, _9,_10, \
         _11,_12,_13,_14,_15,_16,_17,_18,_19,_20, \
         _21,_22,_23,_24,_25,_26,_27,_28,_29,_30, \
         _31,_32,_33,_34,_35,_36,_37,_38,_39,_40, \
         _41,_42,_43,_44,_45,_46,_47,_48,_49,_50, \
         _51,_52,_53,_54,_55,_56,_57,_58,_59,_60, \
         _61,_62,_63,N,...) N
#define PT_RSEQ_N() \
         63,62,61,60,                   \
         59,58,57,56,55,54,53,52,51,50, \
         49,48,47,46,45,44,43,42,41,40, \
         39,38,37,36,35,34,33,32,31,30, \
         29,28,27,26,25,24,23,22,21,20, \
         19,18,17,16,15,14,13,12,11,10, \
         9,8,7,6,5,4,3,2,1,0

#define PT_END()  ((pt_expr){ PT_OP_END })
#define PT_ELEMENT(e)  ((pt_expr){ PT_OP_ELEMENT, 0, (void *) e })
#define PT_LITERAL(str, size)  ((pt_expr){ PT_OP_LITERAL, size, str })
#define PT_LITERAL_S(str)  ((pt_expr){ PT_OP_LITERAL, sizeof(str) - 1, str })
#define PT_LITERAL_0(str)  ((pt_expr){ PT_OP_LITERAL, strlen(str), str })
#define PT_CASE(str, size)  ((pt_expr){ PT_OP_CASE_INSENSITIVE, size, str })
#define PT_CASE_S(str)  ((pt_expr){ PT_OP_CASE_INSENSITIVE, sizeof(str) - 1, str })
#define PT_CASE_0(str)  ((pt_expr){ PT_OP_CASE_INSENSITIVE, strlen(str), str })
#define PT_CLASS(c)  ((pt_expr){ PT_OP_CHARACTER_CLASS, c })
#define PT_ALNUM()  PT_CLASS(PT_CLASS_ALNUM)
#define PT_ALPHA()  PT_CLASS(PT_CLASS_ALPHA)
#define PT_CNTRL()  PT_CLASS(PT_CLASS_CNTRL)
#define PT_DIGIT()  PT_CLASS(PT_CLASS_DIGIT)
#define PT_GRAPH()  PT_CLASS(PT_CLASS_GRAPH)
#define PT_LOWER()  PT_CLASS(PT_CLASS_LOWER)
#define PT_PUNCT()  PT_CLASS(PT_CLASS_PUNCT)
#define PT_SPACE()  PT_CLASS(PT_CLASS_SPACE)
#define PT_UPPER()  PT_CLASS(PT_CLASS_UPPER)
#define PT_XDIGIT()  PT_CLASS(PT_CLASS_XDIGIT)
#define PT_SET(str, size)  ((pt_expr){ PT_OP_SET, size, str })
#define PT_SET_S(str)  ((pt_expr){ PT_OP_SET, sizeof(str) - 1, str })
#define PT_SET_0(str)  ((pt_expr){ PT_OP_SET, strlen(str), str })
#define PT_RANGE(from, to)  ((pt_expr){ PT_OP_RANGE, 0, (void *) PT_RANGE_PACK(from, to) })
#define PT_ANY()  ((pt_expr){ PT_OP_ANY, 0 })
#define PT_CALL(index)  ((pt_expr){ PT_OP_NON_TERMINAL, 0, (void *) index })
#define PT_AT_LEAST(n, ...)  ((pt_expr){ PT_OP_AT_LEAST, PT_NARG(__VA_ARGS__), (void *) n }), __VA_ARGS__
#define PT_AT_MOST(n, ...)  ((pt_expr){ PT_OP_AT_MOST, PT_NARG(__VA_ARGS__), (void *) n }), __VA_ARGS__
#define PT_AND(...)  ((pt_expr){ PT_OP_AND, PT_NARG(__VA_ARGS__) }), __VA_ARGS__
#define PT_NOT(...)  ((pt_expr){ PT_OP_NOT, PT_NARG(__VA_ARGS__) }), __VA_ARGS__
#define PT_SEQUENCE(...)  ((pt_expr){ PT_OP_SEQUENCE, PT_NARG(__VA_ARGS__) }), __VA_ARGS__
#define PT_CHOICE(...)  ((pt_expr){ PT_OP_CHOICE, PT_NARG(__VA_ARGS__) }), __VA_ARGS__
#define PT_CUSTOM_MATCHER(f)  ((pt_expr){ PT_OP_CUSTOM_MATCHER, 0, (void *) f })
#define PT_ERROR(index)  ((pt_expr){ PT_OP_ERROR, 0 })
#define PT_ACTION(action, ...)  ((pt_expr){ PT_OP_ACTION, PT_NARG(__VA_ARGS__), (void *) action }), __VA_ARGS__

// Aliases
#define PT_ONE_OR_MORE(...)  PT_AT_LEAST(1, __VA_ARGS__)
#define PT_ZERO_OR_MORE(...)  PT_AT_LEAST(0, __VA_ARGS__)
#define PT_OPTIONAL(...)  PT_AT_MOST(1, __VA_ARGS__)
#define PT_ANY_BUT(...) PT_NOT(__VA_ARGS__), PT_ANY()
#define PT_RULE(...)  { __VA_ARGS__, PT_END() }

#ifdef PT_DEFINE_SHORTCUTS
    #define ELEMENT PT_ELEMENT
    #define B PT_ELEMENT
    #define LITERAL PT_LITERAL_S
    #define L PT_LITERAL_S
    #define CASE_INSENSITIVE PT_CASE_S
    #define I PT_CASE_S
    #define CLASS PT_CLASS
    #define C PT_CLASS
    #define ALNUM PT_ALNUM
    #define ALPHA PT_ALPHA
    #define CNTRL PT_CNTRL
    #define DIGIT PT_DIGIT
    #define GRAPH PT_GRAPH
    #define LOWER PT_LOWER
    #define PUNCT PT_PUNCT
    #define SPACE PT_SPACE
    #define UPPER PT_UPPER
    #define XDIGIT PT_XDIGIT
    #define SET PT_SET_S
    #define S PT_SET_S
    #define RANGE PT_RANGE
    #define R PT_RANGE
    #define ANY PT_ANY
    #define CALL PT_CALL
    #define V PT_CALL
    #define AT_LEAST PT_AT_LEAST
    #define AT_MOST PT_AT_MOST
    #define ONE_OR_MORE PT_ONE_OR_MORE
    #define ZERO_OR_MORE PT_ZERO_OR_MORE
    #define OPTIONAL PT_OPTIONAL
    #define OPT PT_OPTIONAL
    #define AND PT_AND
    #define NOT PT_NOT
    #define SEQ PT_SEQUENCE
    #define EITHER PT_CHOICE
    #define CUSTOM_MATCHER PT_CUSTOM_MATCHER
    #define F PT_CUSTOM_MATCHER
    #define ERROR PT_ERROR
    #define E PT_ERROR
    #define ACT PT_ACTION
    #define ANY_BUT PT_ANY_BUT
#endif

/// Match result: a {number of matched chars/match error code, action
/// result} pair.
typedef struct pt_match_result {
    /// If non-negative, represents the number of characters matched;
    /// otherwise, it's an error code.
    int matched;
    /// Resulting data from the last top-level Action.
    /// 
    /// @note If you need a single result for all top-level Actions, just create
    /// an outer one that folds them (which will always be the last top-level
    /// one).
    PT_DATA data;
} pt_match_result;

/// Options passed to `pt_match`.
typedef struct pt_match_options {
    void *userdata;  ///< Custom user data for the actions
    pt_error_action on_error;  ///< The action to be performed when a syntactic error is found
    size_t initial_stack_capacity;  ///< The initial capacity for the stack. If 0, stack capacity will begin at a reasonable default
} pt_match_options;


/// Default match options: all 0 or NULL.
PT_DECL const pt_match_options pt_default_match_options;

/// Try to match the string `str` with a PEG.
/// 
/// @warning This function doesn't check for ill-formed grammars, so it's advised
///          that you validate it before running the match algorithm.
/// 
/// @param grammar  Expression array of arbitrary size. For a single Expression,
///                 just pass a pointer to it.
/// @param str   Subject string to match.
/// @param opts  Match options. If NULL, pega-texto will use the default value
///              @ref pt_default_match_options.
/// @return Number of matched characters/error code, result of Action folding.
PT_DECL pt_match_result pt_match(const pt_grammar grammar, pt_element_string str, const pt_match_options *const opts);

// TODO: grammar validation

#ifdef __cplusplus
}
#endif

#endif  // PEGA_TEXTO_H

///////////////////////////////////////////////////////////////////////////////

#ifdef PEGA_TEXTO_IMPLEMENTATION

#include <ctype.h>

#ifndef PT_ASSERT
    #include <assert.h>
    #define PT_ASSERT(cond, message, d) assert(cond && message)
#endif

#ifndef PT_MALLOC
    #define PT_MALLOC(size, d) malloc(size)
#endif
#ifndef PT_REALLOC
    #define PT_REALLOC(p, size, d) realloc(p, size)
#endif
#ifndef PT_FREE
    #define PT_FREE(p, d) free(p)
#endif

const char * const pt_operation_names[] = {
    "PT_OP_END",
    "PT_OP_ELEMENT",
    "PT_OP_LITERAL",
    "PT_OP_CASE_INSENSITIVE",
    "PT_OP_CHARACTER_CLASS",
    "PT_OP_SET",
    "PT_OP_RANGE",
    "PT_OP_ANY",
    "PT_OP_CUSTOM_MATCHER",
    "PT_OP_NON_TERMINAL",
    "PT_OP_AT_LEAST",
    "PT_OP_AT_MOST",
    "PT_OP_AND",
    "PT_OP_NOT",
    "PT_OP_SEQUENCE",
    "PT_OP_CHOICE",
    "PT_OP_ERROR",
    "PT_OP_ACTION",
};

/// A function that receives a character (int) and match it (non-zero) or not (0).
typedef int(*pt__character_class_function)(int);

/**
 * Get the function to be used for matching a Character Class.
 */
static inline pt__character_class_function pt__function_for_character_class(enum pt_character_class c) {
    switch(c) {
        case PT_CLASS_ALNUM: return isalnum;
        case PT_CLASS_ALPHA: return isalpha;
        case PT_CLASS_CNTRL: return iscntrl;
        case PT_CLASS_DIGIT: return isdigit;
        case PT_CLASS_GRAPH: return isgraph;
        case PT_CLASS_LOWER: return islower;
        case PT_CLASS_PUNCT: return ispunct;
        case PT_CLASS_SPACE: return isspace;
        case PT_CLASS_UPPER: return isupper;
        case PT_CLASS_XDIGIT: return isxdigit;
        default: return NULL;
    }
}

const pt_match_options pt_default_match_options = {};

/// Default initial stack capacity.
#ifndef PT_DEFAULT_INITIAL_STACK_CAPACITY
    #define PT_DEFAULT_INITIAL_STACK_CAPACITY 64
#endif

/**
 * A State on the Matching algorithm.
 */
typedef struct pt__match_state {
    const pt_expr *e;  ///< Current expression being matched.
    pt_element_string sp;  ///< string pointer
    unsigned int qc;  ///< Quantifier counter
    unsigned int ac;  ///< Action counter.
} pt__match_state;

/**
 * Dynamic sequential stack of States.
 */
typedef struct pt__match_state_stack {
    pt__match_state *states;  ///< States buffer.
    size_t size;  ///< Current number of States.
    size_t capacity;  ///< Capacity of the States buffer.
} pt__match_state_stack;

/**
 * Queried actions, to be executed on match success.
 */
typedef struct pt__match_action {
    pt_expression_action f;  ///< Action function.
    pt_element_string str;  ///< Pointer to capture start
    size_t size;  ///< Size of a capture
    int argc;  ///< Number of arguments that will be passed when Action is executed.
} pt__match_action;

/**
 * Dynamic sequential stack of Actions.
 */
typedef struct pt__match_action_stack {
    pt__match_action *actions;  ///< Queried Actions buffer.
    size_t size;  ///< Current number of Queried Actions.
    size_t capacity;  ///< Capacity of the Queried Actions buffer.
} pt__match_action_stack;

typedef struct pt__match_context {
    const pt_expr *const *const grammar;
    const pt_match_options* opts;
    pt__match_state_stack state_stack;
    pt__match_action_stack action_stack;
} pt__match_context;


static int pt__initialize_action_stack(pt__match_context *context) {
    size_t initial_capacity = context->opts->initial_stack_capacity;
    if(initial_capacity == 0) {
        initial_capacity = PT_DEFAULT_INITIAL_STACK_CAPACITY;
    }
    context->action_stack.actions = (pt__match_action *) PT_MALLOC(initial_capacity * sizeof(pt__match_action), context->opts->userdata);
    if(context->action_stack.actions) {
        context->action_stack.size = 0;
        context->action_stack.capacity = initial_capacity;
        return 1;
    }
    else {
        return 0;
    }
}

static void pt__destroy_action_stack(pt__match_context *context) {
    PT_FREE(context->action_stack.actions, context->opts->userdata);
}

static pt__match_action *pt__push_action(pt__match_context *context, pt_expression_action f, pt_element_string str, size_t size, int argc) {
    pt__match_action *action;
    // Double capacity, if reached
    if(context->action_stack.size == context->action_stack.capacity) {
        int new_capacity = context->action_stack.capacity * 2;
        action = (pt__match_action *) PT_REALLOC(context->action_stack.actions, new_capacity * sizeof(pt__match_action), context->opts->userdata);
        if(action) {
            context->action_stack.capacity = new_capacity;
            context->action_stack.actions = action;
        }
        else {
            return NULL;
        }
    }
    action = context->action_stack.actions + (context->action_stack.size)++;
    action->f = f;
    action->str = str;
    action->size = size;
    action->argc = argc;

    return action;
}

static void pt__run_actions(pt__match_context *context, pt_match_result *result) {
    PT_DATA *data_stack;
    if(sizeof(PT_DATA) > sizeof(pt__match_action)) {
        // Allocate the data stack
        data_stack = (PT_DATA *) PT_MALLOC(context->action_stack.size * sizeof(PT_DATA), context->opts->userdata);
        if(data_stack == NULL) {
            result->matched = PT_NO_STACK_MEM;
            return;
        }
    }
    else {
        // PT_DATA may safely overwrite popped actions memory, as the later will be read just before pushing results
        // This avoids a malloc/free pair
        data_stack = (PT_DATA *) context->action_stack.actions;
    }

    // index to current Data on the stack
    int data_index = 0;

    // Fold It, 'til there are no Actions left.
    // Note that this only works because of how the Actions are layed out in
    // the Action Stack.
    pt__match_action *action;
    for(action = context->action_stack.actions; action < context->action_stack.actions + context->action_stack.size; action++) {
        // "pop" arguments
        data_index -= action->argc;
        // run action with arguments (which are still stacked in `data_stack` in the right position)
        data_stack[data_index] = action->f(
            action->str,
            action->size,
            action->argc,
            data_stack + data_index,
            context->opts->userdata
        );
        // "push" result
        data_index++;
    }
    result->data = data_stack[0];
    if(sizeof(PT_DATA) > sizeof(pt__match_action)) {
        PT_FREE(data_stack, context->opts->userdata);
    }
}

typedef struct pt__match_expr_result {
    int success;
    int sp_advance;
    int e_advance;
} pt__match_expr_result;

static pt__match_expr_result pt__match_expr(pt__match_context *context, const pt_expr *const e, pt_element_string sp);

static pt__match_expr_result pt__match_sequence(pt__match_context *context, const pt_expr *const e, pt_element_string sp) {
    pt__match_expr_result result = { 1, 0, 1 + e->N }, subresult;
    for(int i = 0; result.success && i < e->N; i += subresult.e_advance) {
        subresult = pt__match_expr(context, e + 1 + i, sp + result.sp_advance);
        result.success = subresult.success;
        result.sp_advance += subresult.sp_advance;
    }
    return result;
}

static pt__match_expr_result pt__match_rule(pt__match_context *context, size_t index, pt_element_string sp) {
    pt__match_expr_result result = { 1, 0, 1 }, subresult;
    for(const pt_expr *e = context->grammar[index]; result.success && e->op != PT_OP_END; e += subresult.e_advance) {
        subresult = pt__match_expr(context, e, sp + result.sp_advance);
        result.success = subresult.success;
        result.sp_advance += subresult.sp_advance;
    }
    return result;
}

static pt__match_expr_result pt__match_expr(pt__match_context *context, const pt_expr *const e, pt_element_string sp) {
    pt__match_expr_result result = { 0, 0, 1 };
    switch(e->op) {
        case PT_OP_END:
            result.success = 1;
            break;

        case PT_OP_ELEMENT:
            result.success = (*sp) == (PT_ELEMENT_TYPE) e->element;
            result.sp_advance = 1;
            break;

        case PT_OP_LITERAL:
            result.success = strncmp(sp, e->str, e->N) == 0;
            result.sp_advance = e->N;
            break;

        case PT_OP_CASE_INSENSITIVE:
            result.success = strncasecmp(sp, e->str, e->N) == 0;
            result.sp_advance = e->N;
            break;

        case PT_OP_CHARACTER_CLASS:
            result.success = pt__function_for_character_class((enum pt_character_class) e->N)(*sp) != 0;
            result.sp_advance = 1;
            break;

        case PT_OP_SET:
            result.success = *sp && strchr(e->str, *sp);
            result.sp_advance = 1;
            break;

        case PT_OP_RANGE: {
            PT_ELEMENT_TYPE element = *sp;
            result.success = element >= PT_RANGE_UNPACK_FROM(e->range) && element <= PT_RANGE_UNPACK_TO(e->range);
            result.sp_advance = 1;
            break;
        }

        case PT_OP_ANY:
            result.success = (*sp) != 0;
            result.sp_advance = 1;
            break;

        case PT_OP_CUSTOM_MATCHER: {
            int custom_matcher_result = e->matcher(sp, context->opts->userdata);
            result.success = custom_matcher_result > 0;
            result.sp_advance = custom_matcher_result;
            break;
        }

        case PT_OP_NON_TERMINAL:
            return pt__match_rule(context, e->index, sp);

        case PT_OP_AT_LEAST: {
            unsigned int counter = 0;
            while(1) {
                pt__match_expr_result subresult = pt__match_sequence(context, e, sp + result.sp_advance);
                if(subresult.success) {
                    result.sp_advance += subresult.sp_advance;
                    counter++;
                }
                else {
                    break;
                }
            }
            result.success = counter >= e->quantifier;
            result.e_advance = 1 + e->N;
            break;
        }

        case PT_OP_AT_MOST: {
            for(unsigned int counter = 0; counter < e->quantifier; counter++) {
                pt__match_expr_result subresult = pt__match_sequence(context, e, sp + result.sp_advance);
                if(subresult.success) {
                    result.sp_advance += subresult.sp_advance;
                }
                else {
                    break;
                }
            }
            result.success = 1;
            result.e_advance = 1 + e->N;
            break;
        }

        case PT_OP_NOT: {
            result = pt__match_sequence(context, e, sp);
            result.success = !result.success;
            result.sp_advance = 0;
            break;
        }

        case PT_OP_AND: {
            result = pt__match_sequence(context, e, sp);
            result.sp_advance = 0;
            break;
        }

        case PT_OP_SEQUENCE:
            return pt__match_sequence(context, e, sp);

        case PT_OP_CHOICE: {
            for(int i = 0; !result.success && i < e->N; i += result.e_advance) {
                result = pt__match_expr(context, e + 1 + i, sp);
            }
            result.e_advance = 1 + e->N;
            break;
        }

        case PT_OP_ACTION: {
            size_t previous_action_count = context->action_stack.size;
            result = pt__match_sequence(context, e, sp);
            if(result.success) {
                if(!pt__push_action(context, e->action, sp, result.sp_advance, context->action_stack.size - previous_action_count)) {
                    // TODO: PT_NO_STACK_MEM result
                    PT_ASSERT(0, "FIXME", context->opts->userdata);
                }
            }
            break;
        }

        case PT_OP_ERROR: {
            // TODO: handle syntactic error
            break;
        }

        default:
            // TODO: assert unknown operation?
            break;
    }
    return result;
}

PT_DECL pt_match_result pt_match(const pt_grammar grammar, pt_element_string str, const pt_match_options *const opts) {
    pt_match_result result = {};
    if(str == NULL) {
        result.matched = PT_NULL_INPUT;
        return result;
    }
    pt__match_context context = {
        (const pt_expr *const *const) grammar,
        opts == NULL ? &pt_default_match_options : opts,
    };
    if(!pt__initialize_action_stack(&context)) {
        result.matched = PT_NO_STACK_MEM;
        return result;
    }
    pt__match_expr_result subresult = pt__match_rule(&context, 0, str);
    if(subresult.success) {
        result.matched = subresult.sp_advance;
        if(context.action_stack.size > 0) {
            pt__run_actions(&context, &result);
        }
    }
    else {
        result.matched = PT_NO_MATCH;
    }
    pt__destroy_action_stack(&context);
    return result;
}

#endif  // PEGA_TEXTO_IMPLEMENTATION
