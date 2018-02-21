#include "test-utils.h"

#include <ctype.h>

int alphabetically_ordered(const char *str, void *data) {
	int i = 0;
	while(str[i] && str[i] <= str[i + 1]) i++;
	return i ? i + 1 : i;
}

int whole_alphabet(const char *str, void *data) {
	int i = 0;
	while(str[i] && str[i + 1] == str[i] + 1) i++;
	return i == 25 ? i : 0;
}

int main() {
	pt_expr *ordered = SEQ(F(alphabetically_ordered), NOT(ANY));
	pt_expr *alphabet = F(whole_alphabet);

	puts(
		Yes(ordered, "abcd") && No(ordered, "abdc") &&
		Yes(ordered, "ABab") && No(ordered, "abAB") &&
		Yes(alphabet, "abcdefghijklmnopqrstuvwxyz") && No(alphabet, "abcdefghijklmnopqrstuvwxy") &&
		Yes(alphabet, "ABCDEFGHIJKLMNOPQRSTUVWXYZ") && No(alphabet, "ABCDEFGHIJKLMNOPQRSTUVWXYz")
		? "PASS" : "FAIL");

	pt_destroy_expr(ordered);
	pt_destroy_expr(alphabet);
	return 0;
}
