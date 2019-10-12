#include "test-utils.h"

#include <ctype.h>

int main() {
	pt_expr *hello = SEQ(L("Hello"), Q(C(PT_SPACE), 1), S("Ww"), L("orld"), Q(L("!"), -1));
	puts(
		Yes(hello, "Hello world!") && Yes(hello, "Hello World") && Yes(hello, "Hello        world!") &&
		Yes(hello, "Hello\tworld with tab") && Yes(hello, "Hello world!!!!") &&
		No(hello, "Helloworld") && No(hello, "hello world!") && No(hello, "Hello there")
		? "PASS" : "FAIL");
	pt_destroy_expr(hello);
	return 0;
}

