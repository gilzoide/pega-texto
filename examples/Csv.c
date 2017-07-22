/* A CSV file parser =]
 */

#include <pega-texto/pega-texto.h>
#include <pega-texto/macro-on.h>

#include "readfile.h"

#include <stdlib.h>
#include <stdio.h>

int main(int argc, char **argv) {
	// Read whole file
	assert(argc > 1 && "Need a CSV file to parse");
	char *str = readfile(argv[1]);

	/* CSV <- Line*
	 * Line <- Field ("," Field)* (EOL / !.)
	 * Field <- [^",\r\n]+ / Quoted
	 * Quoted <- "\"" ("\"\"" / [^"])* "\""
	 * EOL <- \r? \n
	 */
	pt_rule R[] = {
		{ "CSV", Q(V("Line"), 0) },
		{ "Line", SEQ(V("Field"), Q(SEQ(L(","), V("Field")), 0), OR(V("EOL"), NOT(ANY))) },
		{ "Field", OR(Q(BUT(S("\",\r\n")), 1), V("Quoted")) },
		{ "Quoted", SEQ(L("\""), Q(OR(L("\"\""), BUT(L("\""))), 0), L("\"")) },
		{ "EOL", SEQ(Q(L("\r"), -1), L("\n")) },
		{ NULL, NULL },
	};
	pt_grammar *g = pt_create_grammar(R, 0);

	assert(pt_match_grammar(g, str, NULL) >= 0 && "Error on parse: is the CSV file valid?");

	pt_destroy_grammar(g);
	free(str);
	return 0;
}
