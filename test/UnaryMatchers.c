#include "test-utils.h"

int main() {
	pt_expr *G[] = {
		V("."),
		ANY,
	};
	const char *names[] = {
		"Axiom",
		".",
	};
	pt_expr *and_h = AND(L("hello"));
	pt_expr *not_h = NOT(L("hello"));

	puts(
		_Res(G, names, "hi!", 1) && _No(G, names, "") &&
		Res(and_h, "hello", 0) && Res(and_h, "hello there", 0) &&
		No(and_h, "hellO") && No(and_h, "nah...") &&
		Res(not_h, "hi!", 0) && Yes(not_h, "") &&
		No(not_h, "hello") && No(not_h, "hello there")
		? "PASS" : "FAIL");

	int i;
	for(i = 0; i < sizeof(G) / sizeof(pt_expr *); i++) {
		pt_destroy_expr(G[i]);
	}
	pt_destroy_expr(and_h);
	pt_destroy_expr(not_h);
	return 0;
}
