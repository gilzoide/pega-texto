/* Infix Calculator, with support for simple arithmetic operations and
 * expression grouping with parenthesis. The operator precedence is guaranteed
 * by the Grammar structure, and the `calc` Action will give the expected
 * results.
 */

#include <pega-texto.h>
#include <pega-texto/macro-on.h>

#include <stdio.h>
#include <ctype.h>
#include <assert.h>

// Parse a number.
pt_data tonumber(const char *str, size_t start, size_t end, int argc, pt_data *argv, void *data) {
	return (pt_data){ .d = atof(str + start) };
}

// Get the operation to be performed.
pt_data get_op(const char *str, size_t start, size_t end, int argc, pt_data *argv, void *data) {
	return (pt_data){ .c = str[start] };
}

// Fold numbers using the arithmetic operations.
// Operator precedence is guaranteed by the Grammar structure.
pt_data calc(const char *str, size_t start, size_t end, int argc, pt_data *argv, void *data) {
	assert(argc % 2 == 1 && "No way there are even arguments");

	int i;
	double res = argv[0].d;
	for(i = 1; i < argc; i += 2) {
		switch(argv[i].c) {
			case '+': res += argv[i + 1].d; break;
			case '-': res -= argv[i + 1].d; break;
			case '*': res *= argv[i + 1].d; break;
			case '/': res /= argv[i + 1].d; break;
			default: assert(0 && "No way the operation is not in '+-*/'");
		}
	}
	return (pt_data){ .d = res };
}

int main(int argc, char **argv) {
	assert(argc > 1 && "Please, supply an expression");

	pt_rule R[] = {
		{ "Axiom", SEQ(V("Space"), V("Exp"), NOT(ANY)) },
		{ "Exp", SEQ_(calc, V("Term"), Q(SEQ(V("TermOp"), V("Term")), 0)) },
		{ "Term", SEQ_(calc, V("Factor"), Q(SEQ(V("FactorOp"), V("Factor")), 0)) },
		{ "Factor", OR(V("Number"), SEQ(V("OpenParen"), V("Exp"), V("CloseParen"))) },
		// Lexical elements
		{ "Space", Q(C(isspace), 0) },
		{ "Number", SEQ_(tonumber, Q(S("+-"), -1), Q(C(isdigit), 1), V("Space")) },
		{ "TermOp", SEQ_(get_op, S("+-"), V("Space")) },
		{ "FactorOp", SEQ_(get_op, S("*/"), V("Space")) },
		{ "OpenParen", SEQ(L("("), V("Space")) },
		{ "CloseParen", SEQ(L(")"), V("Space")) },
		{ NULL, NULL },
	};
	pt_grammar *g = pt_create_grammar(R, 0);
	pt_validate_grammar(g, PT_VALIDATE_ABORT);

	pt_match_result res = pt_match_grammar(g, argv[1], NULL);
	if(res.matched >= 0) {
		printf("Result: %g\n", res.data.d);
	}
	else {
		puts("Invalid expression!");
	}

	pt_destroy_grammar(g);
	return 0;
}

