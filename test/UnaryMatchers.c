#include "test-utils.h"
#include "macro-on.h"

int main() {
	pt_expr *G[] = {
		V("."),
		ANY,
	};
	const char *names[] = {
		"Axiom",
		".",
	};

	puts(_Res(G, names, "hi!", 1) && _No(G, names, "")
		? "PASS" : "FAIL");

	int i;
	for(i = 0; i < sizeof(G) / sizeof(pt_expr *); i++) {
		pt_destroy_expr(G[i]);
	}
	return 0;
}
