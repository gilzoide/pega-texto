#include "pega-texto.h"
#include <stdio.h>

#define PRINT_SIZE(s) \
	printf(#s ": %d\n", sizeof(s))

int main() {
	puts("Struct sizes:");
	puts("=============");
	PRINT_SIZE(pt_expr);
	PRINT_SIZE(pt_rule);
	PRINT_SIZE(pt_grammar);
	PRINT_SIZE(pt_match_state);
	PRINT_SIZE(pt_match_state_stack);
	PRINT_SIZE(pt_match_options);
	return 0;
}
