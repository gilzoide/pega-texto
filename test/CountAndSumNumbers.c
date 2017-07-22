#include "test-utils.h"

#include <ctype.h>

typedef struct {
	int count;
	int sum;
} count_and_sum;

void on_success(const pt_match_state_stack *s, const char *str, size_t start, size_t end, void *data) {
	puts("Numbers:");
}

void count_and_sum_number(const char *str, size_t start, size_t end, void *_data) {
	count_and_sum *data = (count_and_sum *) _data;
	int num = atoi(str + start);
	printf("%d\n", num);
	data->count++;
	data->sum += num;
}

int main() {
	// Numbers <- ({%d+} / .)*
	pt_expr *e = Q(OR(Q_(&count_and_sum_number, F(isdigit), 1), ANY), 0);
	count_and_sum cs = {};
	pt_match_options opts = {
		.on_success = on_success,
		.userdata = &cs,
	};

	if(pt_match_expr(e, "857ac5\n2+3=5", &opts) > 0) {
		printf("Found %d numbers with sum = %d\nPASS\n", cs.count, cs.sum);
	}
	else puts("FAIL");

	pt_destroy_expr(e);
	return 0;
}

