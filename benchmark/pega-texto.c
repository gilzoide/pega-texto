/* Regex-like syntax for creating pega-texto PEGs, parsed using pega-texto itself.
 * Creates the parser for this language from a textual self representation,
 * and then match itself.
 */

#define PT_DEFINE_SHORTCUTS
#define PEGA_TEXTO_IMPLEMENTATION
#include "pega-texto.h"

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>


enum {
    R_GRAMMAR,
    R_DEFINITION,
    R_EXP,
    R_SEQ,
    R_PREFIX,
    R_SUFFIXED,
    R_SUFFIX,
    R_PRIMARY,
    R_STRING,
    R_LITERAL,
    R_CASE,
    R_CHAR_SET,
    R_ITEM,
    R_RANGE,
    R_CHAR,
    R_DEFINED,
    R_S,
    R_IDENTIFIER,
    R_ARROW,
    R_NUMBER,
};

pt_rule GRAMMAR = PT_RULE(
    V(R_S), ONE_OR_MORE(V(R_DEFINITION)), NOT(ANY())
);
pt_rule DEFINITION = PT_RULE(
    V(R_IDENTIFIER), V(R_ARROW), V(R_EXP)
);
pt_rule EXP = PT_RULE(
    V(R_SEQ), ZERO_OR_MORE(B('/'), V(R_S), V(R_SEQ))
);
pt_rule SEQ = PT_RULE(
    ONE_OR_MORE(V(R_PREFIX))
);
pt_rule PREFIX = PT_RULE(
    OPTIONAL(S("&!"), V(R_S)),
    V(R_SUFFIXED)
);
pt_rule SUFFIXED = PT_RULE(
    V(R_PRIMARY), OPTIONAL(V(R_SUFFIX))
);
pt_rule SUFFIX = PT_RULE(
    EITHER(
        SEQ(S("+*?"), V(R_S)),
        SEQ(B('^'), V(R_NUMBER))
    )
);
pt_rule PRIMARY = PT_RULE(
    EITHER(
        SEQ(B('('), V(R_S), V(R_EXP), B(')'), V(R_S)),
        SEQ(B('{'), V(R_S), V(R_EXP), B('}'), V(R_S)),
        V(R_DEFINED),
        V(R_LITERAL),
        V(R_CASE),
        V(R_CHAR_SET),
        SEQ(B('.'), V(R_S)),
        SEQ(V(R_IDENTIFIER), NOT(V(R_ARROW)))
    )
);
pt_rule STRING = PT_RULE(
    EITHER(
        SEQ(B('\"'), ZERO_OR_MORE(NOT(B('\"')), V(R_CHAR)), B('\"'), V(R_S)),
        SEQ(B('\''), ZERO_OR_MORE(NOT(B('\'')), V(R_CHAR)), B('\''), V(R_S))
    )
);
pt_rule LITERAL = PT_RULE(V(R_STRING));
pt_rule CASE = PT_RULE(
    B('I'), V(R_STRING)
);
pt_rule CHAR_SET = PT_RULE(
    B('['),
    OPTIONAL(B('^')),
    V(R_ITEM),
    ZERO_OR_MORE(
        NOT(B(']')),
        V(R_ITEM)
    ),
    B(']'),
    V(R_S)
);
pt_rule ITEM = PT_RULE(
    EITHER(
        V(R_DEFINED),
        V(R_RANGE),
        V(R_CHAR)
    )
);
pt_rule RANGE = PT_RULE(
    ANY(), B('-'), ANY_BUT(B(']'))
);
pt_rule CHAR = PT_RULE(
    EITHER(
        SEQ(B('\\'), S("abfnrtv\'\"[]\\")),
        SEQ(B('\\'), R('0', '2'), R('0', '7'), R('0', '7')),
        SEQ(B('\\'), R('0', '7'), OPTIONAL(R('0', '7'))),
        ANY()
    )
);
pt_rule DEFINED = PT_RULE(
    B('\\'), S("wWaAcCdDgGlLpPsSuUxX"), V(R_S)
);
pt_rule _S = PT_RULE(
    ZERO_OR_MORE(
        EITHER(
            SPACE(),
            SEQ(B('#'), ZERO_OR_MORE(ANY_BUT(B('\n'))))
        )
    )
);
pt_rule IDENTIFIER = PT_RULE(
    EITHER(ALPHA(), B('_')),
    ZERO_OR_MORE(
        EITHER(
            ALNUM(),
            S("_-")
        )
    ),
    V(R_S)
);
pt_rule ARROW = PT_RULE(
    L("<-"), V(R_S)
);
pt_rule NUMBER = PT_RULE(
    OPTIONAL(S("+-")),
    ONE_OR_MORE(DIGIT()),
    V(R_S)
);

pt_grammar G = {
    [R_GRAMMAR] = GRAMMAR,
    [R_DEFINITION] = DEFINITION,
    [R_EXP] = EXP,
    [R_SEQ] = SEQ,
    [R_PREFIX] = PREFIX,
    [R_SUFFIXED] = SUFFIXED,
    [R_SUFFIX] = SUFFIX,
    [R_PRIMARY] = PRIMARY,
    [R_STRING] = STRING,
    [R_LITERAL] = LITERAL,
    [R_CASE] = CASE,
    [R_CHAR_SET] = CHAR_SET,
    [R_ITEM] = ITEM,
    [R_RANGE] = RANGE,
    [R_CHAR] = CHAR,
    [R_DEFINED] = DEFINED,
    [R_S] = _S,
    [R_IDENTIFIER] = IDENTIFIER,
    [R_ARROW] = ARROW,
    [R_NUMBER] = NUMBER,
};

char *readfile(const char *filename) {
	FILE *fp = fopen(filename, "r");
	assert(fp != NULL && "Error reading file");
	fseek(fp, 0, SEEK_END);
	long size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	char *buffer = malloc((size + 1) * sizeof(char));
	assert(buffer && "[readfile] Couldn't malloc buffer");
	fread(buffer, sizeof(char), size, fp);
	buffer[size] = '\0';
	fclose(fp);
	return buffer;
}

#include "benchmark.h"

int main(int argc, char **argv) {
	char *input = readfile(argc > 1 ? argv[1] : "grammar.txt");

	pt_match_options opts = { .initial_stack_capacity = 99999 };

	int matched;
	BENCHMARK(
		matched = pt_match(G, input, &opts).matched;
	)
	puts(matched >= 0
		 ? "Matched"
		 : "No match");

	free(input);
	return 0;
}

// Parser Actions

/* pt_data tonumber(const char *str, size_t start, size_t end, int argc, pt_data *argv, void *data) { */
	/* return (pt_data){ .i = atoi(str + start) }; */
/* } */
/* pt_data identifier(const char *str, size_t start, size_t end, int argc, pt_data *argv, void *data) { */
	/* assert(argc == 0 && "Identifier"); */
	/* return (pt_data){ .p = strndup(str + start, end - start) }; */
/* } */
/* pt_data defined(const char *str, size_t start, size_t end, int argc, pt_data *argv, void *data) { */
	/* assert(argc == 0 && "Defined"); */
	/* int (*f)(int); */
	/* char c = str[start]; */
	/* switch(c) { */
		/* case 'w': case 'W': f = isalnum; break; */
		/* case 'a': case 'A': f = isalpha; break; */
		/* case 'c': case 'C': f = iscntrl; break; */
		/* case 'd': case 'D': f = isdigit; break; */
		/* case 'g': case 'G': f = isgraph; break; */
		/* case 'l': case 'L': f = islower; break; */
		/* case 'p': case 'P': f = ispunct; break; */
		/* case 's': case 'S': f = isspace; break; */
		/* case 'u': case 'U': f = isupper; break; */
		/* case 'x': case 'X': f = isxdigit; break; */
		/* default: assert(0 && "No way the class is not in [wWaAcCdDgGlLpPsSuUxX]"); break; */
	/* } */
	/* pt_expr *e = C(f); */
	/* if(isupper(c)) { */
		/* e = BUT(e); */
	/* } */
	/* return (pt_data){ .p = e }; */
/* } */
/* pt_data forward_character(const char *str, size_t start, size_t end, int argc, pt_data *argv, void *data) { */
	/* return (pt_data){ .c = str[start] }; */
/* } */
/* pt_data character(const char *str, size_t start, size_t end, int argc, pt_data *argv, void *data) { */
	/* char c = str[start]; */
	/* if(c == '\\') { */
		/* c = str[start + 1]; */
		/* if(isdigit(c)) { */
			/* int i; */
			/* c = c - '0'; */
			/* for(i = 0; i < argc; i++) { */
				/* c <<= 3; */
				/* c += argv[i].c - '0'; */
			/* } */
		/* } */
		/* else { */
			/* switch(c) { */
				/* case 'a': c = '\a'; break; */
				/* case 'b': c = '\b'; break; */
				/* case 'f': c = '\f'; break; */
				/* case 'n': c = '\n'; break; */
				/* case 'r': c = '\r'; break; */
				/* case 't': c = '\t'; break; */
				/* case 'v': c = '\v'; break; */
				/* default: break; */
			/* } */
		/* } */
	/* } */
	/* return (pt_data){ .c = c }; */
/* } */
/* pt_data range(const char *str, size_t start, size_t end, int argc, pt_data *argv, void *data) { */
	/* assert(end - start == 3 && "Range"); */
	/* char *r; */
	/* pt_expr *e = NULL; */
	/* if(r = malloc(3 * sizeof(char))) { */
		/* r[0] = str[start]; */
		/* r[1] = str[start + 2]; */
		/* r[2] = '\0'; */
		/* e = R_O(r); */
	/* } */
	/* return (pt_data){ .p = e }; */
/* } */
/* pt_data one_char(const char *str, size_t start, size_t end, int argc, pt_data *argv, void *data) { */
	/* assert(argc == 1 && "One char @ Item"); */
	/* char *l; */
	/* pt_expr *e = NULL; */
	/* if(l = malloc(2 * sizeof(char))) { */
		/* l[0] = argv[0].c; */
		/* l[1] = '\0'; */
		/* e = L_O(l); */
	/* } */
	/* return (pt_data){ .p = e }; */
/* } */
/* pt_data concat_characters(const char *str, size_t start, size_t end, int argc, pt_data *argv, void *data) { */
	/* char *l; */
	/* if(l = malloc((argc + 1) * sizeof(char))) { */
		/* int i; */
		/* for(i = 0; i < argc; i++) { */
			/* l[i] = argv[i].c; */
		/* } */
		/* l[i] = '\0'; */
	/* } */
	/* return (pt_data){ .p = l }; */
/* } */
/* pt_data literal(const char *str, size_t start, size_t end, int argc, pt_data *argv, void *data) { */
	/* return (pt_data){ .p = L_O(argv[0].p) }; */
/* } */
/* pt_data case_insensitive(const char *str, size_t start, size_t end, int argc, pt_data *argv, void *data) { */
	/* return (pt_data){ .p = I_O(argv[0].p) }; */
/* } */
/* pt_data char_set(const char *str, size_t start, size_t end, int argc, pt_data *argv, void *data) { */
	/* assert(argc > 0 && "CharacterSet"); */
	/* pt_expr *e; */
	/* if(argc > 1) { */
		/* pt_expr **es; */
		/* if(es = malloc(argc * sizeof(pt_expr *))) { */
			/* int i; */
			/* for(i = 0; i < argc; i++) { */
				/* es[i] = argv[i].p; */
			/* } */
			/* e = pt_create_choice(es, argc, 1, NULL); */
		/* } */
	/* } */
	/* else { */
		/* e = argv[0].p; */
	/* } */
	/* // it's a but: [^...] */
	/* if(str[start + 1] == '^') { */
		/* e = BUT(e); */
	/* } */
	/* return (pt_data){ .p = e }; */
/* } */
/* pt_data non_terminal(const char *str, size_t start, size_t end, int argc, pt_data *argv, void *data) { */
	/* return (pt_data){ .p = V_O(argv[0].p) }; */
/* } */
/* pt_data build_grammar(const char *str, size_t start, size_t end, int argc, pt_data *argv, void *data) { */
	/* int i, current, N = argc / 2; */
	/* pt_rule rules[N + 1]; */
	/* for(i = 0; i < N; i++) { */
		/* current = i * 2; */
		/* rules[i].name = argv[current].p; */
		/* rules[i].e = argv[current + 1].p; */
	/* } */
	/* rules[i].name = NULL; */
	/* rules[i].e = NULL; */
	/* return (pt_data){ .p = pt_create_grammar(rules, 1) }; */
/* } */
/* pt_data create_any(const char *str, size_t start, size_t end, int argc, pt_data *argv, void *data) { */
	/* return (pt_data){ .p = ANY }; */
/* } */
/* pt_data with_action(const char *str, size_t start, size_t end, int argc, pt_data *argv, void *data) { */
	/* va_list *args = data; */
	/* ((pt_expr *) argv[0].p)->action = va_arg(*args, pt_expression_action); */
	/* return argv[0]; */
/* } */
/* pt_data quantifier(const char *str, size_t start, size_t end, int argc, pt_data *argv, void *data) { */
	/* int N; */
	/* switch(str[start]) { */
		/* case '+': N = 1; break; */
		/* case '*': N = 0; break; */
		/* case '?': N = -1; break; */
		/* default: assert(0 && "No way the quantifier is not in [+*?]"); break; */
	/* } */
	/* return (pt_data){ .i = N }; */
/* } */
/* pt_data maybe_prefix(const char *str, size_t start, size_t end, int argc, pt_data *argv, void *data) { */
	/* assert(argc > 0 && "Prefix"); */
	/* pt_expr *e = argv[0].p; */
	/* switch(str[start]) { */
		/* case '&': e = AND(e); break; */
		/* case '!': e = NOT(e); break; */
		/* default: break; */
	/* } */
	/* return (pt_data){ .p = e }; */
/* } */
/* pt_data maybe_quantifier(const char *str, size_t start, size_t end, int argc, pt_data *argv, void *data) { */
	/* assert(argc > 0 && "Suffixed"); */
	/* pt_expr *e = argv[0].p; */
	/* if(argc > 1) { */
		/* e = Q(e, argv[1].i); */
	/* } */
	/* return (pt_data){ .p = e }; */
/* } */
/* pt_data maybe_seq(const char *str, size_t start, size_t end, int argc, pt_data *argv, void *data) { */
	/* assert(argc > 0 && "Seq"); */
	/* pt_expr *e; */
	/* if(argc <= 1) { */
		/* e = argv[0].p; */
	/* } */
	/* else { */
		/* int i; */
		/* pt_expr **es; */
		/* if(es = malloc(argc * sizeof(pt_expr *))) { */
			/* for(i = 0; i < argc; i++) { */
				/* es[i] = argv[i].p; */
			/* } */
			/* e = pt_create_sequence(es, argc, 1, NULL); */
		/* } */
	/* } */
	/* return (pt_data){ .p = e }; */
/* } */
/* pt_data maybe_choice(const char *str, size_t start, size_t end, int argc, pt_data *argv, void *data) { */
	/* assert(argc > 0 && "Exp"); */
	/* pt_expr *e; */
	/* if(argc <= 1) { */
		/* e = argv[0].p; */
	/* } */
	/* else { */
		/* int i; */
		/* pt_expr **es; */
		/* if(es = malloc(argc * sizeof(pt_expr *))) { */
			/* for(i = 0; i < argc; i++) { */
				/* es[i] = argv[i].p; */
			/* } */
			/* e = pt_create_choice(es, argc, 1, NULL); */
		/* } */
	/* } */
	/* return (pt_data){ .p = e }; */
/* } */
