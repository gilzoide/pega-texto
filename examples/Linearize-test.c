#include <pega-texto.h>
#include <pega-texto/macro-on.h>

#include "readfile.h"

#include <stdlib.h>
#include <stdio.h>

int main(int argc, char **argv) {
	/* assert(argc > 1 && "Please write something!"); */
	/* char *str = argv[1]; */
	char *str = "WORLD\0 HELLO WORLD";

	pt_rule R[] = {
		/* { "Hello", L("HELLO WORLD") }, */
		/* { "Hello", S("HELO") }, */
		/* { "Hello", C(PT_ALNUM) }, */
		/* { "Hello", B('H') }, */
		/* { "Hello", R('A', 'Z') }, */
		/* { "Hello", SEQ(NOT(B('a')), AND(ANY), L("HELLO"), SEQ(NULL), C(PT_SPACE), V("WORLD"), AND(B('\0'))) }, */
		/* { "HelloOrWorld", SEQ(OR(L("HELLO"), V("WORLD")), B(' ')) }, */
		/* { "WORLD", L("WORLD") }, */
		{ ".+", Q(ANY, 1) },
		{ NULL, NULL },
	};
	pt_grammar g;
	assert(pt_init_grammar(&g, R, 0) && "Failed to initialize Grammar!");

	pt_memory memory;
	assert(pt_memory_init(&memory) && "Failed  to initialize memory manager");
	enum pt_compile_status status;
	pt_list *list = pt_linearize_grammar(&g, &status, &memory);

	printf("%s\n", pt_get_compile_status_description(status));
	if(status == PT_COMPILE_SUCCESS) {
		int i;
		for(i = 0; i < list->size; i++) {
			pt_expr **expr = pt_list_at(list, i, pt_expr *);
			printf("%p ", *expr);
		}
		printf("\n");
	}

	pt_memory_release(&memory);
	pt_release_grammar(&g);
	return 0;
}

