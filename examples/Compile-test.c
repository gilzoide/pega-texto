/* A CSV file parser
 */

#include <pega-texto.h>
#include <pega-texto/macro-on.h>

#include "readfile.h"

#include <stdlib.h>
#include <stdio.h>

int main(int argc, char **argv) {
	assert(argc > 1 && "Please write something!");
	char *str = argv[1];

	pt_rule R[] = {
		{ "Hello", L("HELLO WORLD") },
		/* { "Hello", SEQ(L("HELLO WORLD"), NOT(ANY)) }, */
		{ NULL, NULL },
	};
	pt_grammar g;
	assert(pt_init_grammar(&g, R, 0) && "Failed to initialize Grammar!");

	pt_bytecode bytecode = {};
	assert(pt_compile_grammar(&bytecode, &g) == PT_COMPILE_SUCCESS && "Failed to initialize Bytecode!");

	pt_release_bytecode(&bytecode);
	pt_release_grammar(&g);
	return 0;
}

