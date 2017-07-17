/* Find all '#include<header>' there are in a file.
 * It does support <> and "" delimiters, and spaces between '#', 'include' and '{<"}header name{">}'
 * It does NOT check for comments (//#include<header> is counted)
 * Print the ocurrences along with a count
 */

#include "pega-texto.h"
#include "macro-on.h"

#include <stdio.h>
#include <ctype.h>
#include <assert.h>

void print_includes(const pt_match_state_stack *s, const char *str, size_t begin, size_t end, void *data) {
	int *count = data;
	printf("%.*s\n", end - begin, str + begin);
	(*count)++;
}

int main(int argc, char **argv) {
	// Read whole file
	assert(argc > 1 && "Need a file to search for includes");
	FILE *fp = fopen(argv[1], "r");
	assert(fp != NULL && "Error reading file");
	fseek(fp, 0, SEEK_END);
	long size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	char buffer[size + 1];
	fread(buffer, sizeof(char), size, fp);
	buffer[size] = '\0';
	fclose(fp);

	/* Axiom <- (Include / .)*
	 * Include <- "#" Space "include" Space (Angled / Quoted)
	 * Angled <- "<" (!">" .)+ ">"
	 * Quoted <- "\"" (!"\"" .)+ "\""
	 * Space <- \s*
	 */
	pt_rule R[] = {
		{ "Axiom", Q(OR(V_(print_includes, "Include"), ANY), 0) },
		{ "Include", SEQ(L("#"), V("Space"), L("include"), V("Space"), OR(V("Angled"), V("Quoted"))) },
		{ "Angled", SEQ(L("<"), Q(SEQ(NOT(L(">")), ANY), 1), L(">")) },
		{ "Quoted", SEQ(L("\""), Q(SEQ(NOT(L("\"")), ANY), 1), L("\"")) },
		{ "Space", Q(F(isspace), 0) },
		{ NULL, NULL },
	};
	pt_grammar *g = pt_create_grammar(R, 0);

	int include_count = 0;
	pt_match_options opts = { .userdata = &include_count };
	assert(pt_match_grammar(g, buffer, &opts) >= 0 && "Error on parse: is the string valid?");
	printf("Found %d includes\n", include_count);

	pt_destroy_grammar(g);
	return 0;
}
