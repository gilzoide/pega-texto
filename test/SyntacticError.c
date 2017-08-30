#include <pega-texto.h>
#include <pega-texto/macro-on.h>

#include <stdio.h>
#include <ctype.h>

const char *error_messages[] = {
	"Expecting an identifier",
	"Expecting ','",
};

void print_error(const char *str, size_t where, int code, void *data) {
	fprintf(stderr, "Syntatic error found at position %d: %s\n", where, error_messages[code]);
}

int main() {
	pt_rule R[] = {
		{ "S", SEQ(V("Id"), V("List")) },
		{ "List", OR(NOT(ANY), SEQ(V("Comma"), V("Id"), V("List"))) },
		{ "Id", OR(SEQ(V("Sp"), Q(F(islower), 1)), E(0, L(","))) },
		{ "Comma", OR(SEQ(V("Sp"), L(",")), E(1, Q(F(islower), 1))) },
		{ "Sp", Q(F(isspace), 0) },
		{ NULL, NULL },
	};
	pt_grammar *g = pt_create_grammar(R, 0);
	pt_validate_grammar(g, PT_VALIDATE_ABORT);
	pt_match_options opts = { .on_error = print_error };

	if(pt_match_grammar(g, "one,two", NULL).matched < 0) {
		puts("FAIL");
	}
	else if(pt_match_grammar(g, "one\n two123, \nthree, ", &opts).matched != PT_MATCHED_ERROR) {
		puts("FAIL - no error!");
	}
	else {
		puts("PASS");
	}

	pt_destroy_grammar(g);
	return 0;
}
