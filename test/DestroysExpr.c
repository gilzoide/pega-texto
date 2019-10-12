#include <pega-texto.h>
#include <pega-texto/macro-on.h>

#include <string.h>

int main() {
	pt_expr *e[] = {
		SEQ(L("hi there,"), VI(1), Q(VI(2), 1)),
		C(PT_SPACE), // space
		C(PT_ALPHA), // letters
		L_O(strdup("Expression may own strings, which will be `free`d")),
		L_O(strdup("just use the `*_O` macros =]")),
		V("Memory management is important in C, be careful with"),
		V_O(strdup("which memory you say you own")),
		OR(NULL), // empty Choice
		OR_NO(NULL), // another empty Choice
		NULL, // even NULL pointers are handled nicely
	};

	int i;
	for(i = 0; i < sizeof(e) / sizeof(pt_expr *); i++) {
		pt_destroy_expr(e[i]);
	}
	return 0;
}
