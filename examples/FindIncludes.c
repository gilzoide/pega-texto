/* Find all '#include header' there are in a file.
 * It does support <> and "" delimiters, and spaces between '#', 'include' and '{<"}header name{">}'
 * It does NOT check for comments (`//#include header` is counted)
 * Print the ocurrences along with a count
 */

#include <pega-texto/pega-texto.h>
#include <pega-texto/macro-on.h>

#include "readfile.h"

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <assert.h>

void print_includes(const char *str, size_t begin, size_t end, void *data) {
	int *count = data;
	printf("%.*s\n", end - begin, str + begin);
	(*count)++;
}

int main(int argc, char **argv) {
	assert(argc > 1 && "Need a file to search for includes");
	char *str = readfile(argv[1]);

	/* Axiom <- (Include / .)*
	 * Include <- "#" Space "include" Space (Angled / Quoted)
	 * Angled <- "<" [^>]+ ">"
	 * Quoted <- "\"" [^"]+ "\""
	 * Space <- \s*
	 */
	pt_rule R[] = {
		{ "Axiom", Q(OR(V_(print_includes, "Include"), ANY), 0) },
		{ "Include", SEQ(L("#"), V("Space"), L("include"), V("Space"), OR(V("Angled"), V("Quoted"))) },
		{ "Angled", SEQ(L("<"), Q(BUT(L(">")), 1), L(">")) },
		{ "Quoted", SEQ(L("\""), Q(BUT(L("\"")), 1), L("\"")) },
		{ "Space", Q(F(isspace), 0) },
		{ NULL, NULL },
	};
	pt_grammar *g = pt_create_grammar(R, 0);

	int include_count = 0;
	pt_match_options opts = { .userdata = &include_count };
	assert(pt_match_grammar(g, str, &opts) >= 0 && "Error on parse: is the string valid?");
	printf("Found %d includes\n", include_count);

	pt_destroy_grammar(g);
	free(str);
	return 0;
}
