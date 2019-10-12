#include <pega-texto.h>
#include <pega-texto/macro-on.h>

#include <stdio.h>
#include <ctype.h>

enum {
	IDENTIFIER_EXPECTED = 0,
	COMMA_EXPECTED,
};

const char *error_messages[] = {
	"Expecting an identifier",
	"Expecting ','",
};

void print_error(const char *str, size_t where, int code, void *data) {
	fprintf(stderr, "Syntatic error found at position %zd: %s\n", where, error_messages[code]);
}

void on_end(const pt_match_state_stack *s, const pt_match_action_stack *a,
            const char *str, pt_match_result res, void *data) {
	puts("");
}

int main() {
	pt_rule R[] = {
		{ "S", SEQ(V("Id"), V("List")) },
		{ "List", OR(NOT(ANY), SEQ(V("Comma"), V("Id"), V("List"))) },
		{ "Id", OR(SEQ(V("Sp"), Q(C(PT_LOWER), 1)), E(IDENTIFIER_EXPECTED, L(","))) },
		{ "Comma", OR(SEQ(V("Sp"), L(",")), E(COMMA_EXPECTED, Q(C(PT_LOWER), 1))) },
		{ "Sp", Q(C(PT_SPACE), 0) },
		{ NULL, NULL },
	};
	pt_grammar g;
	pt_init_grammar(&g, R, 0);
	pt_validate_grammar(&g, PT_VALIDATE_ABORT);
	pt_match_options opts = { .on_error = print_error, .on_end = on_end };

	if(pt_match_grammar(&g, "one,two", NULL).matched < 0) {
		puts("FAIL");
	}
	else if(pt_match_grammar(&g, "one\n two", &opts).matched != PT_MATCHED_ERROR) {
		puts("FAIL - no error!");
	}
	else if(pt_match_grammar(&g, "one\n, two,", &opts).matched != PT_MATCHED_ERROR) {
		puts("FAIL - no error!");
	}
	else if(pt_match_grammar(&g, "one\n two,", &opts).data.i != COMMA_EXPECTED) {
		puts("FAIL - wrong error code!");
	}
	else {
		puts("PASS");
	}

	pt_release_grammar(&g);
	return 0;
}
