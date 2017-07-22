#include <pega-texto/pega-texto.h>
#include <pega-texto/macro-on.h>

#include <ctype.h>
#include <string.h>

int main() {
	pt_expr *e[] = {
		SEQ(L("hi there,"), VI(1), Q(VI(2), 1)),
		F(isspace), // space
		F(isalpha), // letters
		L_O(strdup("Expression may own strings, which will be `free`d")),
		L_O(strdup("just use the `*_O` macros =]")),
		V("Memory management is important in C, be careful with"),
		V_O(strdup("which memory you say you own")),
	};

	int i;
	for(i = 0; i < sizeof(e) / sizeof(pt_expr *); i++) {
		pt_destroy_expr(e[i]);
	}
	return 0;
}
