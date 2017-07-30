#include <pega-texto.h>

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

char *readfile(const char *filename) {
	FILE *fp = fopen(filename, "r");
	assert(fp != NULL && "Error reading file");
	fseek(fp, 0, SEEK_END);
	long size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	char *buffer = malloc((size + 1) * sizeof(char));
	assert(buffer && "[readfile] Couldn't malloc buffer");
	fread(buffer, sizeof(char), size, fp);
	buffer[size] = '\0';
	fclose(fp);
	return buffer;
}

void iter(const pt_match_state_stack *s, const pt_match_action_stack *a, const char *str, void *data) {
	size_t *pos = data;
	size_t current = pt_get_current_state(s)->pos;
	if(current > *pos) *pos = current;
}
void on_end(const pt_match_state_stack *s, const pt_match_action_stack *a, const char *str, pt_match_result res, void *data) {
	if(res.matched >= 0) {
		puts("PASS");
	}
	else {
		size_t *pos = data;
		printf("Parse error @ \"\"\"\n%s\n\"\"\"\n", str + *pos);
		puts("FAIL");
	}
}

int main(int argc, char **argv) {
	char *re_grammar = readfile("re_grammar.txt");
	pt_grammar *g = pt_create_grammar_from_string(re_grammar);
	pt_validate_grammar(g, PT_VALIDATE_ABORT);

	size_t pos = 0;
	pt_match_options opts = { .each_iteration = iter, .on_end = on_end, .userdata = &pos };
	assert(pt_match_grammar(g, re_grammar, &opts).matched >= 0);

	pt_destroy_grammar(g);
	free(re_grammar);
	return 0;
}
