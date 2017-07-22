#include <pega-texto/pega-texto.h>
#include <pega-texto/macro-on.h>

#include <stdio.h>

int main() {
	// Invalid Grammar
	pt_rule R[] = {
		{ "Axiom", R("za") },
		{ NULL, NULL },
	};
	pt_grammar *g = pt_create_grammar(R, 0);
	pt_grammar_validate(g, PT_VALIDATE_ABORT);

	pt_destroy_grammar(g);
	return 0;
}
