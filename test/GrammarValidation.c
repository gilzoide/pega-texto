#include <pega-texto.h>
#include <pega-texto/macro-on.h>

#include <stdio.h>

int test(pt_rule R[], int expected_output) {
	pt_grammar *g = pt_create_grammar(R, 0);
	pt_validate_result res = pt_validate_grammar(g, PT_VALIDATE_DEFAULT);
	pt_destroy_grammar(g);

	if(res.status == expected_output) return 1;
	printf("Results don't match: expected %d, found %d\n", expected_output, res.status);
	return 0;
}

int main() {
	// Invalid Grammar
	puts(
		test((pt_rule[]){
			{ ".", ANY },
			{ NULL, NULL },
		}, PT_VALIDATE_SUCCESS) &&
		test((pt_rule[]){
			{ "1 char @ Range", R("a") },
			{ NULL, NULL },
		}, PT_VALIDATE_RANGE_BUFFER) &&
		test((pt_rule[]){
			{ "Reverse chars @ Range", R("za") },
			{ NULL, NULL },
		}, PT_VALIDATE_INVALID_RANGE) &&
		test((pt_rule[]){
			{ "IndexOutOfBoundsError", VI(5) },
			{ NULL, NULL },
		}, PT_VALIDATE_OUT_OF_BOUNDS) &&
		test((pt_rule[]){
			{ "Call undefined", V("undefined") },
			{ NULL, NULL },
		}, PT_VALIDATE_UNDEFINED_RULE) &&
		test((pt_rule[]){
			{ "(&.)*", Q(AND(ANY), 0) },
			{ NULL, NULL },
		}, PT_VALIDATE_LOOP_EMPTY_STRING)
		? "PASS" : "FAIL");
	return 0;
}
