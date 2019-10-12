#include <pega-texto.h>
#include <pega-texto/macro-on.h>

#include <ctype.h>
#include <string.h>

int main() {
	// No-own
	pt_rule R[] = {
		{ "Axiom", SEQ(L("oi"), V("space"), Q(V("Letters"), 1)) },
		{ "Space", C(PT_SPACE) }, // space
		{ "Letters", C(PT_ALPHA) }, // letters
		{ NULL, NULL },
	};
	pt_grammar g;
	if(pt_init_grammar(&g, R, 0)) {
		pt_release_grammar(&g);
	}

	// Own
	pt_rule R2[] = {
		{ strdup("If you own a name"),   L_O(strdup("I own this string too")) },
		{ strdup("You must own'em all"), L("But for Expressions, it is not necessary") },
		{ NULL, NULL },
	};
	if(pt_init_grammar(&g, R2, 1)) {
		pt_release_grammar(&g);
	}

	// Nothing
	pt_release_grammar(NULL);
	return 0;
}
