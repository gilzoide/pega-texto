/* Regex-like syntax for creating pega-texto PEGs, parsed using pega-texto itself.
 * Creates the parser for this language from a textual self representation,
 * and then match itself.
 */

#include <pega-texto.h>
#include <pega-texto/macro-on.h>

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>

// Parser Actions

pt_data tonumber(const char *str, size_t start, size_t end, int argc, pt_data *argv, void *data) {
	return (pt_data){ .i = atoi(str + start) };
}
pt_data identifier(const char *str, size_t start, size_t end, int argc, pt_data *argv, void *data) {
	assert(argc == 0 && "Identifier");
	return (pt_data){ .p = strndup(str + start, end - start) };
}
pt_data defined(const char *str, size_t start, size_t end, int argc, pt_data *argv, void *data) {
	assert(argc == 0 && "Defined");
	int (*f)(int);
	char c = str[start];
	switch(c) {
		case 'w': case 'W': f = isalnum; break;
		case 'a': case 'A': f = isalpha; break;
		case 'c': case 'C': f = iscntrl; break;
		case 'd': case 'D': f = isdigit; break;
		case 'g': case 'G': f = isgraph; break;
		case 'l': case 'L': f = islower; break;
		case 'p': case 'P': f = ispunct; break;
		case 's': case 'S': f = isspace; break;
		case 'u': case 'U': f = isupper; break;
		case 'x': case 'X': f = isxdigit; break;
		default: assert(0 && "No way the class is not in [wWaAcCdDgGlLpPsSuUxX]"); break;
	}
	pt_expr *e = F(f);
	if(isupper(c)) {
		e = BUT(e);
	}
	return (pt_data){ .p = e };
}
pt_data forward_character(const char *str, size_t start, size_t end, int argc, pt_data *argv, void *data) {
	return (pt_data){ .c = str[start] };
}
pt_data character(const char *str, size_t start, size_t end, int argc, pt_data *argv, void *data) {
	char c = str[start];
	if(c == '\\') {
		c = str[start + 1];
		if(isdigit(c)) {
			int i;
			c = c - '0';
			for(i = 0; i < argc; i++) {
				c <<= 3;
				c += argv[i].c - '0';
			}
		}
		else {
			switch(c) {
				case 'a': c = '\a'; break;
				case 'b': c = '\b'; break;
				case 'f': c = '\f'; break;
				case 'n': c = '\n'; break;
				case 'r': c = '\r'; break;
				case 't': c = '\t'; break;
				case 'v': c = '\v'; break;
				default: break;
			}
		}
	}
	return (pt_data){ .c = c };
}
pt_data range(const char *str, size_t start, size_t end, int argc, pt_data *argv, void *data) {
	assert(end - start == 3 && "Range");
	char *r;
	pt_expr *e = NULL;
	if(r = malloc(3 * sizeof(char))) {
		r[0] = str[start];
		r[1] = str[start + 2];
		r[2] = '\0';
		e = R_O(r);
	}
	return (pt_data){ .p = e };
}
pt_data one_char(const char *str, size_t start, size_t end, int argc, pt_data *argv, void *data) {
	assert(argc == 1 && "One char @ Item");
	char *l;
	pt_expr *e = NULL;
	if(l = malloc(2 * sizeof(char))) {
		l[0] = argv[0].c;
		l[1] = '\0';
		e = L_O(l);
	}
	return (pt_data){ .p = e };
}
pt_data literal(const char *str, size_t start, size_t end, int argc, pt_data *argv, void *data) {
	char *l;
	pt_expr *e = NULL;
	if(l = malloc((argc + 1) * sizeof(char))) {
		int i;
		for(i = 0; i < argc; i++) {
			l[i] = argv[i].c;
		}
		l[i] = '\0';
		e = L_O(l);
	}
	return (pt_data){ .p = e };
}
pt_data char_class(const char *str, size_t start, size_t end, int argc, pt_data *argv, void *data) {
	assert(argc > 0 && "Class");
	pt_expr *e;
	if(argc > 1) {
		pt_expr **es;
		if(es = malloc(argc * sizeof(pt_expr *))) {
			int i;
			for(i = 0; i < argc; i++) {
				es[i] = argv[i].p;
			}
			e = pt_create_choice(es, argc, 1, NULL);
		}
	}
	else {
		e = argv[0].p;
	}
	// it's a but: [^...]
	if(str[start + 1] == '^') {
		e = BUT(e);
	}
	return (pt_data){ .p = e };
}
pt_data non_terminal(const char *str, size_t start, size_t end, int argc, pt_data *argv, void *data) {
	return (pt_data){ .p = V_O(argv[0].p) };
}
pt_data build_grammar(const char *str, size_t start, size_t end, int argc, pt_data *argv, void *data) {
	int i, current, N = argc / 2;
	pt_rule rules[N + 1];
	for(i = 0; i < N; i++) {
		current = i * 2;
		rules[i].name = argv[current].p;
		rules[i].e = argv[current + 1].p;
	}
	rules[i].name = NULL;
	rules[i].e = NULL;
	return (pt_data){ .p = pt_create_grammar(rules, 1) };
}
pt_data create_any(const char *str, size_t start, size_t end, int argc, pt_data *argv, void *data) {
	return (pt_data){ .p = ANY };
}
pt_data with_action(const char *str, size_t start, size_t end, int argc, pt_data *argv, void *data) {
	va_list *args = data;
	((pt_expr *) argv[0].p)->action = va_arg(*args, pt_expression_action);
	return argv[0];
}
pt_data quantifier(const char *str, size_t start, size_t end, int argc, pt_data *argv, void *data) {
	int N;
	switch(str[start]) {
		case '+': N = 1; break;
		case '*': N = 0; break;
		case '?': N = -1; break;
		default: assert(0 && "No way the quantifier is not in [+*?]"); break;
	}
	return (pt_data){ .i = N };
}
pt_data maybe_prefix(const char *str, size_t start, size_t end, int argc, pt_data *argv, void *data) {
	assert(argc > 0 && "Prefix");
	pt_expr *e = argv[0].p;
	switch(str[start]) {
		case '&': e = AND(e); break;
		case '!': e = NOT(e); break;
		default: break;
	}
	return (pt_data){ .p = e };
}
pt_data maybe_quantifier(const char *str, size_t start, size_t end, int argc, pt_data *argv, void *data) {
	assert(argc > 0 && "Suffixed");
	pt_expr *e = argv[0].p;
	if(argc > 1) {
		e = Q(e, argv[1].i);
	}
	return (pt_data){ .p = e };
}
pt_data maybe_seq(const char *str, size_t start, size_t end, int argc, pt_data *argv, void *data) {
	assert(argc > 0 && "Seq");
	pt_expr *e;
	if(argc <= 1) {
		e = argv[0].p;
	}
	else {
		int i;
		pt_expr **es;
		if(es = malloc(argc * sizeof(pt_expr *))) {
			for(i = 0; i < argc; i++) {
				es[i] = argv[i].p;
			}
			e = pt_create_sequence(es, argc, 1, NULL);
		}
	}
	return (pt_data){ .p = e };
}
pt_data maybe_choice(const char *str, size_t start, size_t end, int argc, pt_data *argv, void *data) {
	assert(argc > 0 && "Exp");
	pt_expr *e;
	if(argc <= 1) {
		e = argv[0].p;
	}
	else {
		int i;
		pt_expr **es;
		if(es = malloc(argc * sizeof(pt_expr *))) {
			for(i = 0; i < argc; i++) {
				es[i] = argv[i].p;
			}
			e = pt_create_choice(es, argc, 1, NULL);
		}
	}
	return (pt_data){ .p = e };
}

pt_grammar *create_grammar_from_string(const char *str, ...) {
	/* Grammar <- S Definition+ !.
	 * Definition <- Identifier Arrow Exp
	 *
	 * Exp <- Seq ("/" S Seq)*
	 * Seq <- Prefix+
	 * Prefix <- ([&!] S)? Suffixed
	 * Suffixed <- Primary S Suffix?
	 * Suffix <- [+*?] S
	 *         / "^" Number
	 *
	 * Primary <- "(" S Exp ")" S
	 *          / "{" S Exp "}" S
	 *          / Defined
	 *          / Literal
	 *          / Class
	 *          / "." S
	 *          / Identifier !Arrow
	 *
	 * Literal <- '"' (!'"' Character)* '"' S
	 *          / "'" (!"'" Character)* "'" S
	 * Class <- "[" "^"? Item (!"]" Item)* "]" S
	 * Item <- Defined / Range / .
	 * Range <- . "-" [^]]
	 * Character <- "\\" [abfnrtv'"\[\]\\]
	 *            / "\\" [0-2][0-7][0-7]
	 *            / "\\" [0-7][0-7]?
	 *            / .
	 * Defined <- "\\" [wWaAcCdDgGlLpPsSuUxX] S
	 *
	 * S <- (\s / "#" [^\n]*)*
	 * Identifier <- [A-Za-z_][A-Za-z0-9_-]* S
	 * Arrow <- "<-" S
	 * Number <- [+-]? \d+ S
	 */
	pt_rule rules[] = {
		{ "Grammar", SEQ_(build_grammar, V("S"), Q(V("Definition"), 1), NOT(ANY)) },
		{ "Definition", SEQ(V("Identifier"), V("Arrow"), V("Exp")) },

		{ "Exp", SEQ_(maybe_choice, V("Seq"), Q(SEQ(L("/"), V("S"), V("Seq")), 0)) },
		{ "Seq", Q_(maybe_seq, V("Prefix"), 1) },
		{ "Prefix", SEQ_(maybe_prefix, Q(SEQ(S("&!"), V("S")), -1), V("Suffixed")) },
		{ "Suffixed", SEQ_(maybe_quantifier, V("Primary"), Q(V("Suffix"), -1)) },
		{ "Suffix", OR(SEQ(S_(quantifier, "+*?"), V("S")), SEQ(L("^"), V("Number"))) },

		{ "Primary", OR(SEQ(L("("), V("S"), V("Exp"), L(")"), V("S")),
		                SEQ_(with_action, L("{"), V("S"), V("Exp"), L("}"), V("S")),
		                V("Defined"),
		                V("Literal"),
		                V("Class"),
		                SEQ_(create_any, L("."), V("S")),
		                SEQ(V_(non_terminal, "Identifier"), NOT(V("Arrow")))) },
		{ "Literal", OR_(literal,
		                 SEQ(L("\""), Q(SEQ(NOT(L("\"")), V("Character")), 0), L("\""), V("S")),
		                 SEQ(L("\'"), Q(SEQ(NOT(L("\'")), V("Character")), 0), L("\'"), V("S"))) },
		{ "Class", SEQ_(char_class, L("["), Q(L("^"), -1), V("Item"), Q(SEQ(NOT(L("]")), V("Item")), 0), L("]"), V("S")) },
		{ "Item", OR(V("Defined"), V("Range"), V_(one_char, "Character")) },
		{ "Range", SEQ_(range, ANY, L("-"), BUT(L("]"))) },
		{ "Character", OR_(character,
		                   SEQ(L("\\"), S("abfnrtv\'\"[]\\")),
		                   SEQ(L("\\"), R("02"), R_(forward_character, "07"), R_(forward_character, "07")),
		                   SEQ(L("\\"), R("07"), Q(R_(forward_character, "07"), -1)),
		                   ANY) },
		{ "Defined", SEQ(L("\\"), S_(defined, "wWaAcCdDgGlLpPsSuUxX"), V("S")) },

		{ "S", Q(OR(F(isspace), SEQ(L("#"), Q(BUT(L("\n")), 0))), 0) },
		{ "Identifier", SEQ(SEQ_(identifier, OR(F(isalpha), L("_")), Q(OR(F(isalnum), S("_-")), 0)), V("S")) },
		{ "Arrow", SEQ(L("<-"), V("S")) },
		{ "Number", SEQ_(tonumber, Q(S("+-"), -1), Q(F(isdigit), 1), V("S")) },
		{ NULL, NULL },
	};
	pt_grammar *re = pt_create_grammar(rules, 0);
	pt_validate_grammar(re, PT_VALIDATE_ABORT);

	// Expression Actions
	va_list actions;
	va_start(actions, str);
	pt_match_options opts = { .userdata = &actions };

	pt_match_result res = pt_match_grammar(re, str, &opts);
	pt_grammar *g = NULL;
	if(res.matched >= 0) {
		g = res.data.p;
	}

	va_end(actions);
	pt_destroy_grammar(re);
	return g;
}

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

int main(int argc, char **argv) {
	char re_grammar[] =
"Grammar <- S Definition+ !. \n"
"Definition <- Identifier Arrow Exp \n"

"Exp <- Seq (\"/\" S Seq)* \n"
"Seq <- Prefix+ \n"
"Prefix <- ([&!] S)? Suffixed \n"
"Suffixed <- Primary Suffix? \n"
"Suffix <- [+*?] S \n"
"        / \"^\" Number \n"

"Primary <- \"(\" S Exp \")\" S \n"
"         / \"{\" S Exp \"}\" S \n"
"         / Defined \n"
"         / Literal \n"
"         / Class \n"
"         / \".\" S \n"
"         / Identifier !Arrow \n"

"Literal <- \'\"\' (!\'\"\' Character)* \'\"\' S \n"
"         / \"\'\" (!\"\'\" Character)* \"\'\" S \n"
"Class <- \"[\" \"^\"? Item (!\"]\" Item)* \"]\" S \n"
"Item <- Defined / Range / Character \n"
"Range <- . \"-\" [^]] \n"
"Character <- \"\\\\\" [abfnrtv\'\"\\[\\]\\\\] \n"
"           / \"\\\\\" [0-2][0-7][0-7] \n"
"           / \"\\\\\" [0-7][0-7]? \n"
"           / . \n"
"Defined <- \"\\\\\" [wWaAcCdDgGlLpPsSuUxX] S \n"

"S <- (\\s / \"#\" [^\\n]*)* \n"
"Identifier <- [A-Za-z_][A-Za-z0-9_-]* S \n"
"Arrow <- \"<-\" S \n"
"Number <- [+-]? [0-9]+ S";

	pt_grammar *g = create_grammar_from_string(re_grammar);
	pt_validate_grammar(g, PT_VALIDATE_ABORT);

	// Try to match itself
	puts(pt_match_grammar(g, re_grammar, NULL).matched >= 0
	     ? "PASS"
		 : "FAIL");

	pt_destroy_grammar(g);
	return 0;
}
