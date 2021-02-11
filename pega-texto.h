#ifndef PEGA_TEXTO_H
#define PEGA_TEXTO_H

#include <ctype.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#ifndef PTDEF
    #ifdef PT_STATIC
        #define PTDEF static
    #else
        #define PTDEF extern
    #endif
#endif

// Define PT_ELEMENT_TYPE to the string element type, so there can be
// parsers for stuff other than `const char` like `const uint8_t`
#ifndef PT_ELEMENT_TYPE
    typedef const char PT_ELEMENT_TYPE;
#endif
// Define PT_STRING_TYPE to the string type input for `pt_match`
// Needs to support the `+` operator with size_t, like pointers
// By default, it's a constant pointer for PT_ELEMENT_TYPE
#ifndef PT_STRING_TYPE
    typedef PT_ELEMENT_TYPE *const PT_STRING_TYPE;
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Operations for constructing Parsing Expressions.
 */
enum pt_operation {
    PT_OP_END = 0,
    // Primary
    PT_OP_BYTE,             // 'b'
    PT_OP_LITERAL,          // "string"
    PT_OP_CASE_INSENSITIVE, // I"string"
    PT_OP_CHARACTER_CLASS,  // int(char) // If return 0, match fails
                                          // If return non-zero, match succeeds, advance 1
    PT_OP_SET,              // [chars]
    PT_OP_RANGE,            // [c1-c2]
    PT_OP_ANY,              // .
    // Custom match by function
    PT_OP_CUSTOM_MATCHER,  // int(const char *, void *) // Return how many characters were matched
                                                          // Return non-positive values for no match to occur
    // Unary
    PT_OP_NON_TERMINAL,     // <non-terminal> // Recurse to non-terminal expression
    PT_OP_AT_LEAST,            // e^N // Match N or more occurrences of next Expression
    PT_OP_AT_MOST,             // e^-N // Match at most N occurrences of next Expression
    PT_OP_AND,              // &e
    PT_OP_NOT,             // !e
    // N-ary
    PT_OP_SEQUENCE,        // e1 e2
    PT_OP_CHOICE,          // e1 / e2
    PT_OP_ERROR,           // ERROR // Represents a syntactic error
    PT_OP_ACTION,          // Push an action to the stack

    PT_OP_OPERATION_ENUM_COUNT,
};

/// String version of the possible operations.
PTDEF const char* const pt_operation_names[];

/**
 * Character classes supported by pega-texto.
 *
 * Each of them correspond to the `is*` functions defined in `ctype.h` header.
 */
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

/**
 * Possible error codes returned by the `pt_match*` functions.
 */
typedef enum pt_macth_error_code {
    /// Subject string didn't match the given PEG.
    PT_NO_MATCH = -1,
    /// Error while allocating memory for the State/Action Stack.
    PT_NO_STACK_MEM = -2,
    /// Matched an Error Expression.
    PT_MATCHED_ERROR = -3,
    /// Provided string is a NULL pointer.
    PT_NULL_INPUT = -4,
} pt_macth_error_code;

/**
 * Collection of possible types for Expression Actions to return.
 *
 * @note This is not a Tagged Union, so you (developer) are responsible for
 * knowing which type each datum is. This can and should be avoided when
 * structuring the Grammar.
 */
typedef union pt_data {
    void* p;
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
    float f;
    double d;
} pt_data;

/**
 * Facility to return a null #pt_data
 */
#define PT_NULL_DATA ((pt_data){ NULL })

/// A function that receives a string and userdata and match it (positive) or not, advancing the matched number.
typedef int(*pt_custom_matcher_function)(PT_STRING_TYPE, void*);

/**
 * Action to be called on an Expression, after the whole match succeeds.
 *
 * This is the action to be set to an Expression individually, and will be
 * called only if the whole match succeeds, in the order the Expressions were
 * matched.
 *
 * Expression Actions reduce inner Actions' result into a single value.
 *
 * Parameters:
 * - Pointer to the start of the match/capture
 * - Number of bytes contained in the match/capture
 * - Number of #pt_data arguments 
 * - #pt_data arguments, processed on inner Actions. Currently, this array is
 *   reused, so you should not rely on it after Action has returned
 * - User custom data from match options
 *
 * Return:
 *   Anything you want.
 *   This result will be used as argument for other actions below in the stack.
 */
typedef pt_data(*pt_expression_action)(
    PT_STRING_TYPE str,
    size_t size,
    int argc,
    pt_data* argv,
    void* userdata
);

/**
 * Action to be called when an Error Expression is matched (on syntatic errors).
 *
 * Parameters:
 * - The original subject string
 * - Position where the error was encountered
 * - Error code
 * - User custom data from match options
 */
typedef void(*pt_error_action)(
    PT_STRING_TYPE str,
    size_t where,
    int code,
    void* userdata
);

/// Parsing Expressions.
typedef struct pt_expr {
    /// Operation to be performed
    uint8_t op;
    /// At Least and At Most quantifiers, Range byte pair, Literal length, Character Class index.
    int16_t N;
    /// Literal and Character Set strings, Custom Matcher functions.
    union {
        void *data;
        PT_STRING_TYPE str;
        pt_custom_matcher_function matcher;
        pt_expression_action action;
        uintptr_t quantifier;
    };
} pt_expr;

/// Rule typedef, array of expressions (Expression[]).
typedef pt_expr pt_rule[];
/// Grammar typedef, a 2D array of expressions, or array of Rules.
typedef pt_expr* pt_grammar[];

#define PT_RANGE_PACK(from, to) (from | (to << 8))
#define PT_RANGE_UNPACK(r, into_from, into_to) { into_from = r & 0xff; into_to = (r >> 8); }

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
#define PT_BYTE(b)  ((pt_expr){ PT_OP_BYTE, b })
#define PT_LITERAL(str, size)  ((pt_expr){ PT_OP_LITERAL, size, str })
#define PT_LITERAL_S(str)  ((pt_expr){ PT_OP_LITERAL, sizeof(str) - 1, str })
#define PT_LITERAL_0(str)  ((pt_expr){ PT_OP_LITERAL, strlen(str), str })
#define PT_CASE(str, size)  ((pt_expr){ PT_OP_CASE_INSENSITIVE, size, str })
#define PT_CASE_S(str)  ((pt_expr){ PT_OP_CASE_INSENSITIVE, sizeof(str) - 1, str })
#define PT_CASE_0(str)  ((pt_expr){ PT_OP_CASE_INSENSITIVE, strlen(str), str })
#define PT_ALNUM()  ((pt_expr){ PT_OP_CHARACTER_CLASS, PT_CLASS_ALNUM })
#define PT_ALPHA()  ((pt_expr){ PT_OP_CHARACTER_CLASS, PT_CLASS_ALPHA })
#define PT_CNTRL()  ((pt_expr){ PT_OP_CHARACTER_CLASS, PT_CLASS_CNTRL })
#define PT_DIGIT()  ((pt_expr){ PT_OP_CHARACTER_CLASS, PT_CLASS_DIGIT })
#define PT_GRAPH()  ((pt_expr){ PT_OP_CHARACTER_CLASS, PT_CLASS_GRAPH })
#define PT_LOWER()  ((pt_expr){ PT_OP_CHARACTER_CLASS, PT_CLASS_LOWER })
#define PT_PUNCT()  ((pt_expr){ PT_OP_CHARACTER_CLASS, PT_CLASS_PUNCT })
#define PT_SPACE()  ((pt_expr){ PT_OP_CHARACTER_CLASS, PT_CLASS_SPACE })
#define PT_UPPER()  ((pt_expr){ PT_OP_CHARACTER_CLASS, PT_CLASS_UPPER })
#define PT_XDIGIT()  ((pt_expr){ PT_OP_CHARACTER_CLASS, PT_CLASS_XDIGIT })
#define PT_SET(str, size)  ((pt_expr){ PT_OP_SET, size, str })
#define PT_SET_S(str)  ((pt_expr){ PT_OP_SET, sizeof(str) - 1, str })
#define PT_SET_0(str)  ((pt_expr){ PT_OP_SET, strlen(str), str })
#define PT_RANGE(from, to)  ((pt_expr){ PT_OP_RANGE, PT_RANGE_PACK(from, to) })
#define PT_ANY()  ((pt_expr){ PT_OP_ANY, 0 })
#define PT_RULE(index)  ((pt_expr){ PT_OP_NON_TERMINAL, index })
#define PT_AT_LEAST(n, ...)  ((pt_expr){ PT_OP_AT_LEAST, PT_NARG(__VA_ARGS__), (void *) n }), __VA_ARGS__
#define PT_AT_MOST(n, ...)  ((pt_expr){ PT_OP_AT_MOST, PT_NARG(__VA_ARGS__), (void *) n }), __VA_ARGS__
#define PT_AND(...)  ((pt_expr){ PT_OP_AND, PT_NARG(__VA_ARGS__) }), __VA_ARGS__
#define PT_NOT(...)  ((pt_expr){ PT_OP_NOT, PT_NARG(__VA_ARGS__) }), __VA_ARGS__
#define PT_SEQUENCE(...)  ((pt_expr){ PT_OP_SEQUENCE, PT_NARG(__VA_ARGS__) }), __VA_ARGS__
#define PT_CHOICE(...)  ((pt_expr){ PT_OP_CHOICE, PT_NARG(__VA_ARGS__) }), __VA_ARGS__
#define PT_CUSTOM_MATCHER(f)  ((pt_expr){ PT_OP_CUSTOM_MATCHER, 0, (void *) f })
#define PT_ERROR(index)  ((pt_expr){ PT_OP_ERROR, 0 })
#define PT_ACTION(action, ...)  ((pt_expr){ PT_OP_ACTION, PT_NARG(__VA_ARGS__), (void *) action }), __VA_ARGS__

/**
 * Match result: a {number of matched chars/match error code, action
 * result/syntactic error code} pair.
 *
 * This is returned by the `pt_match*` functions.
 */
typedef struct pt_match_result {
    /**
     * If non-negative, represents the number of characters matched;
     * otherwise, it's an error code.
     */
    int matched;
    /**
     * Resulting data from the last top-level Action or first syntactic error
     * found.
     *
     * If `matched` is `PT_MATCHED_ERROR`, `data.i` will be populated with the
     * first syntactic error code found. This is useful if you don't want to
     * specify a #pt_error_action just for capturing the error code.
     *
     * @note If you need a single result for all top-level Actions, just create
     * an outer one that folds them (which will always be the last top-level
     * one).
     */
    pt_data data;
} pt_match_result;

/**
 * Options passed to the `pt_match*` functions.
 *
 * This contains callbacks to be called, as well as some memory usage control.
 *
 * @note The first field is the userdata, so you can easily initialize it with
 *       an aggregate initializer.
 * @warning Zero initialize every unused option before calling `pt_match*`.
 */
typedef struct pt_match_options {
    void* userdata;  ///< Custom user data for the actions
    pt_error_action on_error;  ///< The action to be performed when a syntactic error is found
    size_t initial_stack_capacity;  ///< The initial capacity for the stack. If 0, stack capacity will begin at a reasonable default
} pt_match_options;


/// Default match options: all 0 or NULL =P
PTDEF const pt_match_options pt_default_match_options;

/**
 * Try to match the string `str` with a PEG composed by Expressions.
 *
 * @warning This function doesn't check for ill-formed grammars, so it's advised
 *          that you validate it before running the match algorithm.
 *
 * @param grammar  Expression array of arbitrary size. For a single Expression,
 *                 just pass a pointer to it.
 * @param str   Subject string to match.
 * @param opts  Match options. If NULL, pega-texto will use the default value
 *              @ref pt_default_match_options.
 * @return Number of matched characters/error code, result of Action folding.
 */
PTDEF pt_match_result pt_match(const pt_grammar grammar, PT_STRING_TYPE str, const pt_match_options *const opts);

#ifdef __cplusplus
}
#endif

#endif  // PEGA_TEXTO_H

///////////////////////////////////////////////////////////////////////////////

#ifdef PEGA_TEXTO_IMPLEMENTATION

#ifndef PT_ASSERT
    #include <assert.h>
    #define PT_ASSERT(cond, d) assert(cond)
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
    "PT_OP_BYTE",
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
    #define PT_DEFAULT_INITIAL_STACK_CAPACITY 8
#endif

/// Flags to be OR'ed on match state
typedef enum pt__match_flags {
    PT__MATCH_NONE = 0,
    PT__MATCH_AND = 1 << 1,
    PT__MATCH_NOT = 1 << 2,
    PT__MATCH_CHOICE = 1 << 3,
    PT__MATCH_AT_LEAST = 1 << 4,
    PT__MATCH_AT_MOST = 1 << 5,
    PT__MATCH_ACTION = 1 << 6,
} pt__match_flags;

/**
 * A State on the Matching algorithm.
 */
typedef struct pt__match_state {
    const pt_expr *e;  ///< Current expression being matched.
    size_t pos;  ///< Current position in the stream.
    int matched;
    int N;  ///< General purpose register 1.
    unsigned int ac;  ///< Action counter.
    int flags;
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
    size_t start;  ///< Start point of the match.
    size_t end;  ///< End point of the match.
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
    const pt_match_options* opts;
    pt__match_state_stack state_stack;
    pt__match_action_stack action_stack;
} pt__match_context;

/**
 * Initializes the State Stack, `malloc`ing the stack with `initial_capacity`.
 *
 * @param s                The state stack to be initialized.
 * @param initial_capacity The initial stack capacity. If 0, stack is
 *                         initialized with a default value.
 * @return 1 if the allocation went well, 0 otherwise
 */
static int pt__initialize_state_stack(pt__match_context *context) {
    size_t initial_capacity = context->opts->initial_stack_capacity;
    if(initial_capacity == 0) {
        initial_capacity = PT_DEFAULT_INITIAL_STACK_CAPACITY;
    }
    context->state_stack.states = (pt__match_state *) malloc(initial_capacity * sizeof(pt__match_state));
    if(context->state_stack.states) {
        context->state_stack.size = 0;
        context->state_stack.capacity = initial_capacity;
        return 1;
    }
    else {
        return 0;
    }
}

/**
 * Destroy the State Stack, freeing the memory used.
 *
 * @param s The state stack to be destroyed.
 */
static void pt__destroy_state_stack(pt__match_state_stack *s) {
    free(s->states);
}

/**
 * Push a State into the State Stack, doubling it's capacity, if needed.
 *
 * @param s   The state stack.
 * @param e   The Parsing Expression to be used for the next iteration.
 * @param pos The starting position of the stream for next iteration.
 * @param ac  The new Action counter.
 * @return The newly pushed State.
 */
static pt__match_state *pt__push_state(pt__match_context *context, const pt_expr *const e, size_t pos) {
    pt__match_state *state;
    // Double capacity, if reached
    if(context->state_stack.size == context->state_stack.capacity) {
        size_t new_capacity = context->state_stack.capacity * 2;
        state = (pt__match_state *) realloc(context->state_stack.states, new_capacity * sizeof(pt__match_state));
        if(state) {
            context->state_stack.capacity = new_capacity;
            context->state_stack.states = state;
        }
        else {
            return NULL;
        }
    }
    state = context->state_stack.states + (context->state_stack.size)++;
    state->e = e;
    state->pos = pos;
    state->matched = 0;
    state->N = 0;
    state->ac = context->action_stack.size;

    return state;
}

/**
 * Get the current State on top of the State Stack
 *
 * @param s The state stack.
 * @return Current State, if there is any, `NULL` otherwise.
 */
static pt__match_state *pt__get_current_state(const pt__match_state_stack *s) {
    int i = s->size - 1;
    return i >= 0 ? s->states + i : NULL;
}

static pt__match_state *pt__pop_state(pt__match_state_stack *state_stack, size_t count) {
    if(count < state_stack->size) {
        state_stack->size -= count;
        return &state_stack->states[state_stack->size - 1];
    }
    else {
        state_stack->size = 0;
        return NULL;
    }
}

/**
 * Initializes the State Stack, `malloc`ing the stack with `initial_capacity`.
 *
 * @param a                The action stack to be initialized.
 * @param initial_capacity The initial stack capacity. If 0, stack is
 *                         initialized with a default value.
 * @return 1 if the allocation went well, 0 otherwise
 */
static int pt__initialize_action_stack(pt__match_context *context) {
    size_t initial_capacity = context->opts->initial_stack_capacity;
    if(initial_capacity == 0) {
        initial_capacity = PT_DEFAULT_INITIAL_STACK_CAPACITY;
    }
    context->action_stack.actions = (pt__match_action *) malloc(initial_capacity * sizeof(pt__match_action));
    if(context->action_stack.actions) {
        context->action_stack.size = 0;
        context->action_stack.capacity = initial_capacity;
        return 1;
    }
    else {
        return 0;
    }
}

/**
 * Destroy the Action Stack, freeing the memory used.
 *
 * @param a The action stack to be destroyed.
 */
static void pt__destroy_action_stack(pt__match_action_stack *a) {
    free(a->actions);
}

/**
 * Push an Action into the Action Stack, doubling it's capacity, if needed.
 *
 * @param context  Match context
 * @param f     The function to be called as action.
 * @param start The starting position of the stream for action.
 * @param end   The ending position of the stream for action.
 * @param argc  Number of arguments (inner action results) used by this action.
 * @return The newly pushed State.
 */
static pt__match_action *pt__push_action(pt__match_context *context, pt_expression_action f, size_t start) {
    pt__match_action *action;
    // Double capacity, if reached
    if(context->action_stack.size == context->action_stack.capacity) {
        int new_capacity = context->action_stack.capacity * 2;
        action = (pt__match_action *) realloc(context->action_stack.actions, new_capacity * sizeof(pt__match_action));
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
    action->start = start;
    action->end = 0;
    action->argc = 0;

    return action;
}

/**
 * Get the current State on top of the State Stack
 *
 * @param s The state stack.
 * @return Current State, if there is any, `NULL` otherwise.
 */
static pt__match_action *pt__get_current_action(const pt__match_action_stack *a) {
    int i = a->size - 1;
    return i >= 0 ? a->actions + i : NULL;
}

/**
 * Run all actions in the Action Stack in the right way, folding them into
 * one value.
 */
static pt_data pt__run_actions(pt__match_context *context, PT_STRING_TYPE str) {
    // allocate the data stack
    pt_data *data_stack = (pt_data *) malloc(context->action_stack.size * sizeof(pt_data));
    if(data_stack == NULL) return PT_NULL_DATA;

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
            str + action->start,
            action->end - action->start,
            action->argc,
            data_stack + data_index,
            context->opts->userdata
        );
        // "push" result
        data_index++;
    }
    pt_data res = data_stack[0];
    free(data_stack);
    return res;
}

/// Propagate success back until reach a Quantifier, Sequence, And or Not, changing it's position
static pt__match_state *pt__match_succeed(pt__match_context *context,
        int *matched, PT_STRING_TYPE str, size_t new_pos) {
    pt__match_state *state = context->state_stack.states + context->state_stack.size - 1;
    //const pt_expr* e = state->e;
    int i;
//#ifdef PT_SUCCESS_CALLBACK
    //PT_SUCCESS_CALLBACK(context, str, state->pos, new_pos);
//#endif
    for(i = context->state_stack.size - 2; i >= 0; i--) {
        state = context->state_stack.states + i;
        switch(state->e->op) {
            case PT_OP_SEQUENCE:
            case PT_OP_AT_LEAST:
            case PT_OP_AT_MOST:
                state->matched = new_pos - state->pos; // mark current match accumulator
                state->ac = context->action_stack.size; // keep queried actions
                goto backtrack;

            case PT_OP_AND:
                new_pos = state->pos; // don't consume input...
                context->action_stack.size = state->ac; // ...nor keep queried actions (still succeeds, though)
                break;

            case PT_OP_NOT:
                state->matched = PT_NO_MATCH; // NOT success = fail
                context->action_stack.size = state->ac; // discard queried actions
                goto backtrack;

            case PT_OP_ERROR:
                //pt_destroy_expr(s->states[i + 1].e);  // destroy the syncronization expression wrapper
                break;
        }
    }
    // for ended normally: no more states left
    *matched = new_pos;
    return NULL;

backtrack:
    context->state_stack.size = i + 1;
    return state;
}

/// Return to a backtrack point: either Quantifier, Choice or Not
static pt__match_state *pt__match_fail(pt__match_context *context, PT_STRING_TYPE str) {
    int i;
    pt__match_state *state;
//#ifdef PT_FAIL_CALLBACK
    //PT_FAIL_CALLBACK(context, str);
//#endif
    for(i = context->state_stack.size - 2; i >= 0; i--) {
        state = context->state_stack.states + i;
        switch(state->e->op) {
            case PT_OP_AT_LEAST:
            case PT_OP_AT_MOST:
                state->matched = -(state->matched); // mark end of quantifier matching
            case PT_OP_CHOICE:
                goto backtrack;

            case PT_OP_NOT:
                state->matched = 1; // NOT fail = success
                goto backtrack;

            case PT_OP_ERROR:
                //pt_destroy_expr(s->states[i + 1].e);  // destroy the syncronization expression wrapper
                break;
        }
    }
    // for ended normally: no more states left
    return NULL;

backtrack:
    // rewind stacks to backtrack point
    context->state_stack.size = i + 1;
    context->action_stack.size = state->ac;
    return context->state_stack.states + i;
}

static pt__match_state *pt__match_error(pt__match_context *context) {
    //pt__match_state *state = pt__get_current_state(s);
    //if(state->e->data.e) {
        //// don't double free the sync Expression, as Error Expression owns it
        //pt_expr *but_expr = BUT_NO(state->e->data.e);
        //state = pt_push_state(s, Q(but_expr, 0), state->pos, a->size);
        //return state;
    //}
    //else {
        return NULL;
    //}
}

PTDEF pt_match_result pt_match(const pt_grammar es, PT_STRING_TYPE str, const pt_match_options *const opts) {
    int matched;
    if(str == NULL) {
        matched = PT_NULL_INPUT;
        goto err_null_input;
    }
    pt__match_context context = {
        opts == NULL ? &pt_default_match_options : opts,
    };
    int matched_error = 0;
    pt_data result_data = {};
    if(!pt__initialize_state_stack(&context)) {
        matched = PT_NO_STACK_MEM;
        goto err_state_stack;
    }
    if(!pt__initialize_action_stack(&context)) {
        matched = PT_NO_STACK_MEM;
        goto err_action_stack;
    }

    // iteration variables
    pt__match_state *state = pt__push_state(&context, es[0], 0);
    pt__match_action *action;
    const pt_expr *e;
    const char *ptr;
    uint8_t range_from, range_to;

    // match loop
    while(state) {
//#ifdef PT_ITERATION_CALLBACK
        //PT_ITERATION_CALLBACK(&context, str);
//#endif
        ptr = str + state->pos;
        e = state->e;
        matched = PT_NO_MATCH;

        switch(e->op) {
            case PT_OP_END:
                break;

            // Primary
            case PT_OP_BYTE:
                if(*ptr == e->N) {
                    matched = 1;
                }
                break;

            case PT_OP_LITERAL:
                if(strncmp(ptr, e->str, e->N) == 0) {
                    matched = e->N;
                }
                break;

            case PT_OP_CASE_INSENSITIVE:
                if(strncasecmp(ptr, e->str, e->N) == 0) {
                    matched = e->N;
                }
                break;

            case PT_OP_CHARACTER_CLASS:
                if(pt__function_for_character_class((enum pt_character_class) e->N)(*ptr)) {
                    matched = 1;
                }
                break;

            case PT_OP_SET:
                if(*ptr && strchr(e->str, *ptr)) {
                    matched = 1;
                }
                break;

            case PT_OP_RANGE:
                PT_RANGE_UNPACK(e->N, range_from, range_to);
                if(*ptr >= range_from && *ptr <= range_to) {
                    matched = 1;
                }
                break;

            case PT_OP_ANY:
                if(*ptr) {
                    matched = 1;
                }
                break;

            case PT_OP_CUSTOM_MATCHER:
                matched = e->matcher(ptr, context.opts->userdata);
                if(matched <= 0) {
                    matched = PT_NO_MATCH;
                }
                break;


            // Unary
            case PT_OP_NON_TERMINAL:
                state = pt__push_state(&context, es[e->N], state->pos);
                continue;

            case PT_OP_AT_LEAST:
                state = pt__push_state(&context, e + 1, state->pos);
                state->flags |= PT__MATCH_AT_LEAST;
                continue;

            case PT_OP_AT_MOST:
                state = pt__push_state(&context, e + 1, state->pos);
                state->flags |= PT__MATCH_AT_MOST;
                continue;

            case PT_OP_NOT:
                if(state->N > 0) {
                    matched = 0;
                    break;
                }
                else if(state->N < 0) {
                    break;
                }
                // fallthrough
            case PT_OP_AND:
                state = pt__push_state(&context, e + 1, state->pos);
                continue;

            // N-ary
            case PT_OP_ACTION:
                if(e->action) {
                    if(state->ac - 1 < context.action_stack.size) {
                        action = &context.action_stack.actions[state->ac - 1];
                        action->argc++;
                    }
                    pt__push_action(&context, e->action, state->pos);
                    state->flags |= PT__MATCH_ACTION;
                }
                // fallthrough
            case PT_OP_SEQUENCE:
                if(state->N < e->N) {
                    state = pt__push_state(&context, e + 1 + state->N, state->pos);
                    state->N++;
                    continue;
                }
                else {
                    matched = state->matched;
                }
                break;

            case PT_OP_CHOICE:
                if(state->N < e->N) {
                    state = pt__push_state(&context, e + 1 + state->N, state->pos);
                    state->N++;
                    continue;
                }
                break;

            // Others
            case PT_OP_ERROR:
                // mark that a syntactic error ocurred, so even syncing we remember this
                if(matched_error == 0) {
                    matched_error = 1;
                    result_data.i = e->N;
                }
                if(context.opts->on_error) {
                    context.opts->on_error(str, state->pos, e->N, context.opts->userdata);
                }
                matched = PT_MATCHED_ERROR;
                break;

            // Unknown operation: always fail
            default: break;
        }

        state = matched == PT_NO_MATCH ? pt__match_fail(&context, str)
              : matched == PT_MATCHED_ERROR ? pt__match_error(&context)
              : pt__match_succeed(&context, &matched, str, state->pos + matched);
    }

    if(matched_error) {
        matched = PT_MATCHED_ERROR;
    }
    else if(matched >= 0 && context.action_stack.size > 0) {
        result_data = pt__run_actions(&context, str);
    }
//#ifdef PT_END_CALLBACK
    //PT_END_CALLBACK(&context, str, (pt_match_result){ matched, result_data });
//#endif

    pt__destroy_action_stack(&context.action_stack);
err_action_stack:
    pt__destroy_state_stack(&context.state_stack);
err_state_stack:
err_null_input:
    return (pt_match_result){ matched, result_data };
}

#endif  // PEGA_TEXTO_IMPLEMENTATION
