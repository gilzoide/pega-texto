#include <pega-texto.h>
#include <pega-texto/macro-on.h>

#include <stdio.h>

int test(pt_rule *R, int expected_output) {
	pt_grammar g;
	pt_init_grammar(&g, R, 0);
	pt_validate_result res = pt_validate_grammar(&g, PT_VALIDATE_DEFAULT);
	pt_release_grammar(&g);

	if(res.status == expected_output) return 1;
	printf("Results don't match: expected %d, found %d\n", expected_output, res.status);
	return 0;
}

int test_skip(pt_rule *R, int expected_output) {
	pt_grammar g;
	pt_init_grammar(&g, R, 0);
	pt_validate_result res = pt_validate_grammar(&g, PT_VALIDATE_SKIP);
	pt_release_grammar(&g);

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
            { "empty SEQ", OR(NULL) },
			{ NULL, NULL },
		}, PT_VALIDATE_SUCCESS) &&
		test((pt_rule[]){
			{ "Reverse chars @ Range", R('z', 'a') },
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
		}, PT_VALIDATE_LOOP_EMPTY_STRING) &&
		test((pt_rule[]){
			{ "A", SEQ(ANY, V("B")) },
			{ "B", Q(V("A"), 0) },
			{ NULL, NULL },
		}, PT_VALIDATE_SUCCESS) &&
		test((pt_rule[]){
			{ "loop empty error", E(0, NOT(ANY)) },
			{ NULL, NULL },
		}, PT_VALIDATE_LOOP_EMPTY_STRING) &&
		test_skip((pt_rule[]){
			{ "even skipping, verify NULL pointers", AND(NULL) },
			{ NULL, NULL },
		}, PT_VALIDATE_NULL_POINTER)
		? "PASS" : "FAIL");
	return 0;
}
