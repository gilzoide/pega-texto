#include <pega-texto.h>
#include <pega-texto/macro-on.h>

#include "readfile.h"

#include <stdlib.h>
#include <stdio.h>

int main(int argc, char **argv) {
	/* assert(argc > 1 && "Please write something!"); */
	/* char *str = argv[1]; */
	char *str = "TODO";

	pt_rule R[] = {
		{ "Hello", L("HELLO WORLD") },
		/* { "Hello", SEQ(L("HELLO WORLD"), NOT(ANY)) }, */
		{ NULL, NULL },
	};
	pt_grammar g;
	assert(pt_init_grammar(&g, R, 0) && "Failed to initialize Grammar!");

	pt_bytecode bytecode;
	pt_init_bytecode(&bytecode);
	int result = pt_compile_grammar(&bytecode, &g);
	printf("Compile result: %s\n", pt_compile_status_description[result]);

	pt_release_bytecode(&bytecode);
	pt_release_grammar(&g);
	return 0;
}

