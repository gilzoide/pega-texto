#include "test-utils.h"
#include "macro-on.h"

#include <ctype.h>

typedef struct {
	int count;
	int sum;
} count_and_sum;

void on_success(const pt_match_state_stack *s, const char *str, size_t start, size_t end, void *data) {
	puts("Numbers:");
}

void count_and_sum_number(const pt_match_state_stack *s, const char *str, size_t start, size_t end, count_and_sum *data) {
	int num = atoi(str + start);
	printf("%d\n", num);
	data->count++;
	data->sum += num;
}

int main() {
	// Numbers <- ({%d+} / .)*
	pt_expr *e = Q(OR(Q_(F(isdigit), 1, (pt_success_action) count_and_sum_number), ANY), 0);
	count_and_sum cs = {};
	pt_match_options opts = {
		.on_success = on_success,
		.userdata = &cs,
	};

	if(pt_match_expr(e, "857ac5\n2+3=5", &opts) > 0) {
		printf("Found %d numbers with sum = %d\nPASS\n", cs.count, cs.sum);
	}

	pt_destroy_expr(e);
	return 0;
}

