#include "test-utils.h"
#include "macro-on.h"

int main() {
	pt_rule R[] = {
		/* { "Axiom", SEQ(L("("), Q(OR(SEQ(NOT(S("()")), ANY), V("Axiom")), 0), L(")")) }, */
		{ "Axiom", V("Balanced") },
		{ "Balanced", SEQ(L("("), Q(V("MaybeBalanced"), 0), L(")")) },
		{ "MaybeBalanced", OR(V("NotParen"), V("Balanced")) },
		{ "NotParen", SEQ(NOT(S("()")), ANY) },
		{ NULL, NULL },
	};
	pt_grammar *g = pt_create_grammar(R, 0);

	puts(
		gYes(g, "()") && gYes(g, "(hell() there, f()lks)") &&
		gYes(g, "(let ((a 1) (b 2)) (+ a b))") && gYes(g, "())") &&
		gNo(g, "(()") && gNo(g, "(hi-there (fella) ()")
		? "PASS" : "FAIL");

	pt_destroy_grammar(g);
	return 0;
}
