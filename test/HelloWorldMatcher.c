#include "pega-texto.h"
#include "macro-on.h"
#include <stdio.h>
#include <assert.h>
#include <ctype.h>

int main(int argc, char **argv) {
	assert(argc > 1);
	/* pt_expr *hello = SEQ(L("Hello"), Q(F(isspace), 1), S("Ww"), L("orld"), Q(L("!"), -1)); */
	pt_expr *hello = L("Hello world!");
	puts(pt_match_expr(hello, argv[1], NULL) > 0 ? "PASS" : "FAIL");
	pt_destroy_expr(hello);
	return 0;
}

