#include "test-utils.h"
#include "macro-on.h"

#include <ctype.h>

int always_match(int _) {
	return 1;
}
int iseof(int c) {
	return c == 0;
}

int main() {
	pt_expr *alpha = F(isalpha);
	pt_expr *eof = F(iseof);

	puts(Yes(alpha, "j") && Yes(alpha, "x") &&
		No(alpha, "7") && No(alpha, "") &&
		Yes(eof, "") &&
		No(eof, " ")
		? "PASS" : "FAIL");

	pt_destroy_expr(alpha);
	pt_destroy_expr(eof);
	return 0;
}
