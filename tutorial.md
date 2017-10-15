Tutorial
========
This tutorial will give you an overview of how to use *pega-texto*. Don't
forget to also look at the reference manual and examples (I promise they're
very informative!).

Including and compiling
-----------------------
First things first: include the main header.

```c
#include <pega-texto.h>

int main(int argc, char **argv) {
    return 0;
}
```

To compile a program that uses *pega-texto*, just use the `-lpega-texto` linker
flag, also available from the `pkg-config` command. You may also compile
*pega-texto* as a static library by passing `-DBUILD_SHARED_LIBS=OFF` to
*CMake*.


Creating and destroying Expressions
-----------------------------------
Expressions can be created by the `pt_create_*` functions in `pega-texto/expr.h`
and destroyed with `pt_destroy_expr`.
Remember, this is *C*: **always** free the memory you use!

```c
#include <pega-texto.h>

int main(int argc, char **argv) {
    pt_expr *e = pt_create_literal("foo", 0, NULL);
    pt_destroy_expr(e);
    return 0;
}
```

Creating complex Expressions this way is really boring to type and hard to
read. That is why *pega-texto* offers macros for writing Expressions more
easily. They can be turned on by including the `pega-texto/macro-on.h` header
and turned off by including `pega-texto/macro-off.h`.


```c
#include <pega-texto.h>
#include <pega-texto/macro-on.h>

#include <ctype.h>

int main(int argc, char **argv) {
    pt_expr *e = SEQ(Q(S("+-"), -1), Q(F(isdigit), 1)); // Match integer: [+-]? \d+
    pt_destroy_expr(e);
    return 0;
}
```

These macros may be turned off and aren't included by default because they may
clash with other identifiers in the program, and putting the `PT_` prefix would
make them too cumbersome to use.


Matching
--------
With an Expression in hands, you may start to match strings with
`pt_match_expr`. Match results contain the number of characters matched
(non-negative values) or an error code (negative values), and the Action result
(Actions will be covered below).

```c
#include <pega-texto.h>
#include <pega-texto/macro-on.h>

#include <ctype.h>
#include <stdio.h>

int main(int argc, char **argv) {
    pt_expr *e = SEQ(Q(S("+-"), -1), Q(F(isdigit), 1)); // Match integer: [+-]? \d+
    pt_match_result res;
    res = pt_match_expr(e, "42 hello world!", NULL);
    if(res.matched > 0) {
        printf("This will be printed! Matched %d characters.\n", res.matched); // res.matched == 2
    }
    res = pt_match_expr(e, "hello world! 42", NULL);
    if(res.matched > 0) {
        puts("This will not =/");
    }
    pt_destroy_expr(e);
    return 0;
}
```


Grammars
--------
Instead of single Expressions, you may use a Grammar for more complex languages.
Grammars are formed by named Expressions. They are created using the
`pt_create_grammar` function and destroyed with `pt_destroy_grammar`.
Use the `pt_match_grammar` function to match when using Grammars.

```c
#include <pega-texto.h>
#include <pega-texto/macro-on.h>

#include <ctype.h>
#include <stdio.h>

int main(int argc, char **argv) {
    /* List  <- Id ("," Id)*
     * Id    <- Space [A-Za-z]+ Space
     * Space <- \s*
     */
    pt_rule rules[] = {
        { "List", SEQ(V("Id"), Q(SEQ(L(","), V("Id")), 0)) },
        { "Id", SEQ(V("Space"), Q(F(isalpha), 1), V("Space")) },
        { "Space", Q(F(isspace), 0) },
        { NULL, NULL }, // pega-texto uses NULL-terminated arrays for Rules
    };
    pt_grammar *g = pt_create_grammar(rules, 0);
    if(pt_match_grammar(g, "foo, bar, baz", NULL).matched > 0) {
        puts("This will be printed!");
    }
    if(pt_match_grammar(g, ",,", NULL).matched > 0) {
        puts("This will not =/");
    }
    pt_destroy_grammar(g);
    return 0;
}
```

Grammars should be well-formed, as described by
[Ford (2014)](https://pdos.csail.mit.edu/~baford/packrat/popl04/peg-popl04.pdf),
for the matching algorithm to not enter an infinite loop. As for the
implementation, Non-terminal names should exist and some pointers should not be
`NULL` for preventing SEGFAULTs. One should always validate the Grammar using
the `pt_validate_grammar` function. This function may only return the result,
print error messages to `stderr`, abort the program, or even not validate the
Grammar at all. This is useful if you want to validate the well-formedness of
the Grammar only on debug builds.

```c
#include <pega-texto.h>
#include <pega-texto/macro-on.h>

#include <ctype.h>
#include <stdio.h>

#ifdef NDEBUG
# define VALIDATION_BEHAVIOUR PT_VALIDATE_SKIP
#else
# define VALIDATION_BEHAVIOUR PT_VALIDATE_ABORT
#endif

int main(int argc, char **argv) {
    /* List  <- Id ("," Id)*
     * Id    <- Space [A-Za-z]+ Space
     * Space <- \s*
     */
    pt_rule rules[] = {
        { "List", SEQ(V("Id"), Q(SEQ(L(","), V("Id")), 0)) },
        { "Id", SEQ(V("Space"), Q(F(isalpha), 1), V("Space")) },
        { "Space", Q(F(isspace), 0) },
        { NULL, NULL }, // pega-texto uses NULL-terminated arrays for Rules
    };
    pt_grammar *g = pt_create_grammar(rules, 0);
    if(pt_validate_grammar(g, VALIDATION_BEHAVIOUR).status != PT_VALIDATE_SUCCESS) {
        // oh no! Grammar is well-formed, so this will be a malloc error
    }
    /* ... */
    pt_destroy_grammar(g);
    return 0;
}
```

Actions
-------
Matching strings make more sense when you can actually use their value for
something. Presenting: Actions! Actions are functions that process
the matched part of the input and return some data, and are run only if the
whole match succeeds. Expressions may be enclosed in others, and the results
produced on inner Actions are passed as arguments for the outer ones, in a
*fold/reduce* style.

```c
#include <pega-texto.h>
#include <pega-texto/macro-on.h>

#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>

#ifdef NDEBUG
# define VALIDATION_BEHAVIOUR PT_VALIDATE_SKIP
#else
# define VALIDATION_BEHAVIOUR PT_VALIDATE_ABORT
#endif

typedef struct {
    int size;
    char **ids;
} List;

pt_data copy_identifier(const char *str, size_t start, size_t end, int argc, pt_data *argv, void *data) {
    char *identifier = strndup(str + start, end - start);
    return (pt_data){ .p = identifier };
}

pt_data generate_list(const char *str, size_t start, size_t end, int argc, pt_data *argv, void *data) {
    List *l = malloc(sizeof(List));
    if(l) {
        l->ids = malloc(argc * sizeof(char *));
        if(l->ids) {
            int i;
            for(i = 0; i < argc; i++) {
                l->ids[i] = argv[i].p;
            }
            l->size = argc;
        }
        else {
            l->size = 0;
        }
    }
    return (pt_data){ .p = l };
}

void destroy_list(List *l) {
    int i;
    for(i = 0; i < l->size; i++) {
        free(l->ids[i]);
    }
    free(l->ids);
    free(l);
}

int main(int argc, char **argv) {
    /* List  <- Id ("," Id)*
     * Id    <- Space [A-Za-z]+ Space
     * Space <- \s*
     */
    pt_rule rules[] = {
        { "List", SEQ_(generate_list, V("Id"), Q(SEQ(L(","), V("Id")), 0)) },
        { "Id", SEQ(V("Space"), Q_(copy_identifier, F(isalpha), 1), V("Space")) },
        { "Space", Q(F(isspace), 0) },
        { NULL, NULL }, // pega-texto uses NULL-terminated arrays for Rules
    };
    pt_grammar *g = pt_create_grammar(rules, 0);
    if(pt_validate_grammar(g, VALIDATION_BEHAVIOUR).status != PT_VALIDATE_SUCCESS) {
        // oh no! Grammar is well-formed, so this will be a malloc error
    }

    pt_match_result res = pt_match_grammar(g, "foo, bar, baz", NULL);
    if(res.matched > 0) {
        List *l = res.data.p;
        int i;
        for(i = 0; i < l->size; i++) {
            printf("%s ", l->ids[i]);
        }
        destroy_list(l);
    }
    else {
        puts("This will not be printed");
    }

    pt_destroy_grammar(g);
    return 0;
}
```


Error Handling
--------------
*Pega-texto* offers an Error Expression that, when matched, immediately run an
error callback receiving the numeric error code provided in the Expression,
with an optional syncronization Expression.
When there is no syncronization Expression, the whole match fails instantly.
Otherwise, *pega-texto* looks for the syncronization Expression and resume the
matching there. Either way, `PT_MATCHED_ERROR` is returned and no Actions are
executed.

```c
#include <pega-texto.h>
#include <pega-texto/macro-on.h>

#include <stdio.h>
#include <ctype.h>
#include <assert.h>

#ifdef NDEBUG
# define VALIDATION_BEHAVIOUR PT_VALIDATE_SKIP
#else
# define VALIDATION_BEHAVIOUR PT_VALIDATE_ABORT
#endif

void error_callback(const char *str, size_t where, int code, void *data) {
    fprintf(stderr, "Invalid identifier found at position %u\n", where + 1);
}

int main(int argc, char **argv) {
    /* List  <- Id ("," Id)*
     * Id    <- Space [A-Za-z]+ Space / Error(1, ",")
     * Space <- \s*
     */
    pt_rule rules[] = {
        { "List", SEQ(V("Id"), Q(SEQ(L(","), V("Id")), 0)) },
        { "Id", SEQ(V("Space"), OR(Q(F(isalpha), 1), E(1, L(","))), V("Space")) },
        { "Space", Q(F(isspace), 0) },
        { NULL, NULL }, // pega-texto uses NULL-terminated arrays for Rules
    };
    pt_grammar *g = pt_create_grammar(rules, 0);
    if(pt_validate_grammar(g, VALIDATION_BEHAVIOUR).status != PT_VALIDATE_SUCCESS) {
        // oh no! Grammar is well-formed, so this will be a malloc error
    }

    pt_match_options opts = { .on_error = error_callback };
    pt_match_result res = pt_match_grammar(g, "foo, 'invalid', baz,,", &opts);
    assert(res.matched == PT_MATCHED_ERROR);

    pt_destroy_grammar(g);
    return 0;
}
```

