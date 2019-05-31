#include <pega-texto.h>
#include <pega-texto/macro-on.h>

#include "readfile.h"

#include <stdlib.h>
#include <stdio.h>

int main(int argc, char **argv) {
	/* assert(argc > 1 && "Please write something!"); */
	/* char *str = argv[1]; */
	char *str = "HELLO WORLD";

	pt_rule R[] = {
		/* { "Hello", L("HELLO WORLD") }, */
		/* { "Hello", S("HELO") }, */
		/* { "Hello", C(PT_ALNUM) }, */
		/* { "Hello", B('H') }, */
		/* { "Hello", R('A', 'Z') }, */
		/* { "Hello", SEQ(NOT(B('a')), AND(ANY), L("HELLO"), SEQ(NULL), C(PT_SPACE), V("WORLD"), AND(B('\0'))) }, */
		/* { "HelloOrWorld", OR(L("HELLO"), V("WORLD")) }, */
		/* { "WORLD", L("WORLD") }, */
		{ ".*", Q(ANY, 0) },
		{ NULL, NULL },
	};
	pt_grammar g;
	assert(pt_init_grammar(&g, R, 0) && "Failed to initialize Grammar!");

	pt_bytecode bytecode;
	pt_init_bytecode(&bytecode);
	int result = pt_compile_grammar(&bytecode, &g);
	printf("Compile result: %s\n", pt_get_compile_status_description(result));

	if(result == PT_COMPILE_SUCCESS) {
		pt_dump_bytecode(&bytecode);
		pt_vm vm;
		pt_init_vm(&vm);
		pt_vm_load_bytecode(&vm, &bytecode);

		pt_match_result match_result = pt_vm_match(&vm, str, NULL);
		printf("Match result: %d\n", match_result.matched);

		pt_release_vm(&vm);
	}

	pt_release_bytecode(&bytecode);
	pt_release_grammar(&g);
	return 0;
}
