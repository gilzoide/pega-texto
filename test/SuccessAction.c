#include "test-utils.h"

void print_match(const pt_match_state_stack *s, const pt_match_action_stack *a, const char *str, pt_match_result res, void *data) {
	int matched = res.matched;
	if(matched >= 0) {
		printf("Matched %d char(s) on \"%s\": \"%.*s\"\nPASS\n", matched, str, matched, str);
	}
}

void each_iteration(const pt_match_state_stack *s, const pt_match_action_stack *a, const char *str, void *data) {
	int i;
	for(i = 0; i < s->size - 1; i++) fputc(' ', stdout);
	pt_match_state *state = pt_get_current_state(s);
	printf("%s", pt_operation_names[state->e->op]);
	if(state->e->op == PT_LITERAL) printf(" \"%s\"", state->e->data.characters);
	printf(" on \"%s\"\n", str + state->pos);
}

void each_success(const pt_match_state_stack *s, const pt_match_action_stack *a, const char *str, size_t start, size_t end, void *data) {
	int i;
	for(i = 0; i < s->size - 1; i++) fputc(' ', stdout);
	printf("#success = matched \"%.*s\"\n", end - start, str + start);
}

void each_fail(const pt_match_state_stack *s, const pt_match_action_stack *a, const char *str, void *data) {
	int i;
	for(i = 0; i < s->size - 1; i++) fputc(' ', stdout);
	puts("#fail");
}

int main() {
	pt_expr *e = SEQ(Q(L("a"), 1), Q(L("b"), 1));
	pt_match_options opts = {
		.each_iteration = each_iteration,
		.on_end = print_match,
		.each_success = each_success,
		.each_fail = each_fail,
	};

	pt_match_expr(e, "aaaabcbb", &opts);

	pt_destroy_expr(e);
	return 0;
}
