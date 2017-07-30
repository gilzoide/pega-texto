#include "test-utils.h"

#include <ctype.h>

typedef struct {
	int count;
	int sum;
} count_and_sum;

pt_data count_and_sum_number(const char *str, size_t start, size_t end, int argc, pt_data *argv, void *_data) {
	count_and_sum *data = (count_and_sum *) _data;
	int num = atoi(str + start);
	printf("%d\n", num);
	data->count++;
	data->sum += num;
	return (pt_data){};
}

int main() {
	// Numbers <- ({%d+} / .)*
	pt_expr *e = Q(OR(Q_(&count_and_sum_number, F(isdigit), 1), ANY), 0);
	count_and_sum cs = {};
	pt_match_options opts = {
		.userdata = &cs,
	};

	pt_match_result res = pt_match_expr(e, "857ac5\n2+3=5", &opts);
	if(res.matched > 0) {
		printf("Found %d numbers with sum = %d\nPASS\n", cs.count, cs.sum);
	}
	else puts("FAIL");

	pt_destroy_expr(e);
	return 0;
}

