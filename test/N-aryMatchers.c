#include "test-utils.h"
#include "macro-on.h"

#include <ctype.h>

int main() {
	pt_expr *integer = Q(F(isdigit), 1);
	pt_expr *abc3 = Q(S("abc"), -3);
	pt_expr *this_or_that = OR(L("this"), L("that"));
	pt_expr *num_letter = SEQ(F(isdigit), F(isalpha));

	puts(
		Yes(integer, "0") && Res(integer, "123", 3) && Res(integer, "1only", 1) &&
		No(integer, "b5") && No(integer, "") &&
		Res(abc3, "abc", 3) && Res(abc3, "aaq", 2) && Yes(abc3, "b") && Yes(abc3, "") &&
		Res(abc3, "dab", 0) && Res(abc3, "adb", 1) && Res(abc3, "aaaaaa", 3) &&
		Yes(this_or_that, "this is cool") && Yes(this_or_that, "that also") &&
		No(this_or_that, "thiz") && No(this_or_that, "not this one") &&
		Yes(num_letter, "1a") && Yes(num_letter, "6c") && Yes(num_letter, "0zero") &&
		No(num_letter, "a1") && No(num_letter, "5") && No(num_letter, "no number")
		? "PASS" : "FAIL");

	pt_destroy_expr(integer);
	pt_destroy_expr(abc3);
	pt_destroy_expr(this_or_that);
	pt_destroy_expr(num_letter);
	return 0;
}
