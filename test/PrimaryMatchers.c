#include "test-utils.h"

int main() {
	pt_expr *literal = L("Hello");
	pt_expr *set = S("abcde");
	pt_expr *range = R("az");
	pt_expr *any = ANY;
	char control_stuff[] = {1, 0};

	puts(
		Yes(literal, "Hello") && Yes(literal, "Hello world!") &&
		No(literal, "Helo?") && No(literal, "Hell") &&
		Yes(set, "a") && Yes(set, "b") && Yes(set, "c") && Yes(set, "d") && Yes(set, "e") && Yes(set, "blabla") &&
		No(set, "f") && No(set, "A") && No(set, "") &&
		Yes(range, "a") && Yes(range, "h") && Yes(range, "super cool") && Yes(range, "zoboomafoo") &&
		No(range, "A") && No(range, "") && No(range, ".") &&
		Yes(any, "\t") && Yes(any, control_stuff) &&
		No(any, "")
		? "PASS" : "FAIL");

	pt_destroy_expr(literal);
	pt_destroy_expr(set);
	pt_destroy_expr(range);
	pt_destroy_expr(any);
	return 0;
}
