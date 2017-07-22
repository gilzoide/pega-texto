#include <pega-texto/pega-texto.h>
#include <pega-texto/macro-on.h>

#include <ctype.h>
#include <string.h>

int main() {
	// No-own
	pt_rule R[] = {
		{ "Axiom", SEQ(L("oi"), V("space"), Q(V("Letters"), 1)) },
		{ "Space", F(isspace) }, // space
		{ "Letters", F(isalpha) }, // letters
		{ NULL, NULL },
	};
	pt_grammar *g;
	if(g = pt_create_grammar(R, 0)) {
		pt_destroy_grammar(g);
	}

	// Own
	pt_rule R2[] = {
		{ strdup("If you own a name"),   L_O(strdup("I own this string too")) },
		{ strdup("You must own'em all"), L("But for Expressions, it is not necessary") },
		{ NULL, NULL },
	};
	if(g = pt_create_grammar(R2, 1)) {
		pt_destroy_grammar(g);
	}
	return 0;
}
