/*
 * Copyright 2017-2020 Gil Barbosa Reis <gilzoide@gmail.com>
 * This file is part of pega-texto.
 * 
 * Pega-texto is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Pega-texto is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with pega-texto.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Any bugs should be reported to <gilzoide@gmail.com>
 */

#include "compiler_grammar.h"

#include <pega-texto.h>
#include <pega-texto/macro-on.h>

#include <assert.h>
#include <stdlib.h>

pt_data tonumber(const char *str, size_t length, int argc, pt_data *argv, void *data) {
	return (pt_data){ .i = atoi(str) };
}
pt_data identifier(const char *str, size_t length, int argc, pt_data *argv, void *data) {
	assert(argc == 0 && "Identifier");
	return (pt_data){ .p = strndup(str, length) };
}
pt_data defined(const char *str, size_t length, int argc, pt_data *argv, void *data) {
	assert(argc == 0 && "Defined");
	char c = str[0];
	pt_expr *e = C(c);
	if(isupper(c)) {
		e = BUT(e);
	}
	return (pt_data){ .p = e };
}
pt_data forward_character(const char *str, size_t length, int argc, pt_data *argv, void *data) {
	return (pt_data){ .c = str[0] };
}
pt_data character(const char *str, size_t length, int argc, pt_data *argv, void *data) {
	char c = str[0];
	if(c == '\\') {
		c = str[1];
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
pt_data range(const char *str, size_t length, int argc, pt_data *argv, void *data) {
	assert(length == 3 && "Range");
	pt_expr *e = R(str[0], str[2]);
	return (pt_data){ .p = e };
}
pt_data one_char(const char *str, size_t length, int argc, pt_data *argv, void *data) {
	assert(argc == 1 && "One char @ Item");
	pt_expr *e = B(str[0]);
	return (pt_data){ .p = e };
}
pt_data concat_characters(const char *str, size_t length, int argc, pt_data *argv, void *data) {
	char *l;
	if(l = malloc((argc + 1) * sizeof(char))) {
		int i;
		for(i = 0; i < argc; i++) {
			l[i] = argv[i].c;
		}
		l[i] = '\0';
	}
	return (pt_data){ .p = l };
}
pt_data literal(const char *str, size_t length, int argc, pt_data *argv, void *data) {
	return (pt_data){ .p = L_O(argv[0].p) };
}
pt_data case_insensitive(const char *str, size_t length, int argc, pt_data *argv, void *data) {
	return (pt_data){ .p = I_O(argv[0].p) };
}
pt_data char_set(const char *str, size_t length, int argc, pt_data *argv, void *data) {
	assert(argc > 0 && "CharacterSet");
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
	if(str[1] == '^') {
		e = BUT(e);
	}
	return (pt_data){ .p = e };
}
pt_data non_terminal(const char *str, size_t length, int argc, pt_data *argv, void *data) {
	return (pt_data){ .p = V_O(argv[0].p) };
}
pt_data build_grammar(const char *str, size_t length, int argc, pt_data *argv, void *data) {
	int i, current, N = argc / 2;
	pt_rule rules[N + 1];
	for(i = 0; i < N; i++) {
		current = i * 2;
		rules[i].name = argv[current].p;
		rules[i].e = argv[current + 1].p;
	}
	rules[i].name = NULL;
	rules[i].e = NULL;
    pt_grammar *grammar = (pt_grammar *)data;
    pt_init_grammar(grammar, rules, 1);
	return (pt_data){ .p = grammar };
}
pt_data create_any(const char *str, size_t length, int argc, pt_data *argv, void *data) {
	return (pt_data){ .p = ANY };
}
// pt_data with_action(const char *str, size_t length, int argc, pt_data *argv, void *data) {
// 	va_list *args = data;
// 	((pt_expr *) argv[0].p)->action = va_arg(*args, pt_expression_action);
// 	return argv[0];
// }
pt_data quantifier(const char *str, size_t length, int argc, pt_data *argv, void *data) {
	int N;
	switch(str[0]) {
		case '+': N = 1; break;
		case '*': N = 0; break;
		case '?': N = -1; break;
		default: assert(0 && "No way the quantifier is not in [+*?]"); break;
	}
	return (pt_data){ .i = N };
}
pt_data maybe_prefix(const char *str, size_t length, int argc, pt_data *argv, void *data) {
	assert(argc > 0 && "Prefix");
	pt_expr *e = argv[0].p;
	switch(str[0]) {
		case '&': e = AND(e); break;
		case '!': e = NOT(e); break;
		default: break;
	}
	return (pt_data){ .p = e };
}
pt_data maybe_quantifier(const char *str, size_t length, int argc, pt_data *argv, void *data) {
	assert(argc > 0 && "Suffixed");
	pt_expr *e = argv[0].p;
	if(argc > 1) {
		e = Q(e, argv[1].i);
	}
	return (pt_data){ .p = e };
}
pt_data maybe_seq(const char *str, size_t length, int argc, pt_data *argv, void *data) {
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
        else e = NULL;
	}
	return (pt_data){ .p = e };
}
pt_data maybe_choice(const char *str, size_t length, int argc, pt_data *argv, void *data) {
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
        else e = NULL;
	}
	return (pt_data){ .p = e };
}

int pt_init_compiler_grammar(pt_grammar *grammar) {
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
	 *          / CaseInsensitive
	 *          / CharacterSet
	 *          / "." S
	 *          / Identifier !Arrow
	 *
	 * Literal <- String
	 * CaseInsensitive <- "I" String
	 * String <- '"' (!'"' Character)* '"' S
	 *         / "'" (!"'" Character)* "'" S
	 * CharacterSet <- "[" "^"? Item (!"]" Item)* "]" S
	 * Item <- Defined / Range / Character
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
		                // SEQ_(with_action, L("{"), V("S"), V("Exp"), L("}"), V("S")),
		                V("Defined"),
		                V("Literal"),
		                V("CaseInsensitive"),
		                V("CharacterSet"),
		                SEQ_(create_any, L("."), V("S")),
		                SEQ(V_(non_terminal, "Identifier"), NOT(V("Arrow")))) },
		{ "String", OR_(concat_characters,
		                SEQ(L("\""), Q(SEQ(NOT(L("\"")), V("Character")), 0), L("\""), V("S")),
		                SEQ(L("\'"), Q(SEQ(NOT(L("\'")), V("Character")), 0), L("\'"), V("S"))) },
		{ "Literal", V_(literal, "String") },
		{ "CaseInsensitive", SEQ(L("I"), V_(case_insensitive, "String")) },
		{ "CharacterSet", SEQ_(char_set, L("["), Q(L("^"), -1), V("Item"), Q(SEQ(NOT(L("]")), V("Item")), 0), L("]"), V("S")) },
		{ "Item", OR(V("Defined"), V("Range"), V_(one_char, "Character")) },
		{ "Range", SEQ_(range, ANY, L("-"), BUT(L("]"))) },
		{ "Character", OR_(character,
		                   SEQ(L("\\"), S("abfnrtv\'\"[]\\")),
		                   SEQ(L("\\"), R('0', '2'), R_(forward_character, '0', '7'), R_(forward_character, '0', '7')),
		                   SEQ(L("\\"), R('0', '7'), Q(R_(forward_character, '0', '7'), -1)),
		                   ANY) },
		{ "Defined", SEQ(L("\\"), S_(defined, "wWaAcCdDgGlLpPsSuUxX"), V("S")) },

		{ "S", Q(OR(C(PT_SPACE), SEQ(L("#"), Q(BUT(L("\n")), 0))), 0) },
		{ "Identifier", SEQ(SEQ_(identifier, OR(C(PT_ALPHA), L("_")), Q(OR(C(PT_ALNUM), S("_-")), 0)), V("S")) },
		{ "Arrow", SEQ(L("<-"), V("S")) },
		{ "Number", SEQ_(tonumber, Q(S("+-"), -1), Q(C(PT_DIGIT), 1), V("S")) },
		{ NULL, NULL },
	};
    int result = pt_init_grammar(grammar, rules, 0);
    pt_validate_grammar(grammar, PT_VALIDATE_ABORT);
    return result;
}