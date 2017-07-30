/* A simple parser for Lisp-like languages, grouping S-expressions in Cons lists.
 * Parenthesis create nested Cons lists, everything after a ";" until a new line
 * is ignored (comment), and everything else is captured as a symbol.
 */
#include <pega-texto.h>
#include <pega-texto/macro-on.h>

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>

// Cons cell, a linked list that keeps the nested symbol lists
typedef struct Cons_t {
	void *car;  // If `is_atom`, car is it's symbol (char *); otherwise it's a nested list (Cons *)
	struct Cons_t *cdr;  // Pointer to next Cons cell
	uint8_t is_atom : 1;  // Is `car` an atom or a nested list?
} Cons;

Cons *create_cons_cell(void *car, Cons *cdr, uint8_t is_atom) {
	Cons *new_cell;
	if((new_cell = malloc(sizeof(Cons))) == NULL) {
		fprintf(stderr, "Couldn't allocate memory for Cons cell\n");
		exit(-1);
	}
	new_cell->car = car;
	new_cell->cdr = cdr;
	new_cell->is_atom = is_atom;
	return new_cell;
}

void print_cons_list(Cons *cell) {
	Cons *aux;
	if(cell == NULL) printf("nil");
	else {
		if(cell->is_atom) {
			printf("%s", cell->car != NULL ? cell->car : "nil");
		}
		else {
			printf("(");
			for(aux = cell->car; aux; aux = aux->cdr) print_cons_list(aux);
			printf(")");
		}
		printf("%c", cell->cdr != NULL ? ' ' : '\0');
	}
}

void destroy_cons_list(Cons *cell) {
	Cons *next;
	while(cell) {
		next = cell->cdr;
		if(cell->is_atom) free(cell->car);
		else destroy_cons_list(cell->car);
		free(cell);
		cell = next;
	}
}

pt_data atom(const char *str, size_t start, size_t end, int argc, pt_data *argv, void *data) {
	char *symbol = strndup(str + start, end - start);
	return (pt_data){ .p = create_cons_cell(symbol, NULL, 1) };
}

pt_data list(const char *str, size_t start, size_t end, int argc, pt_data *argv, void *data) {
	int is_empty = argc <= 0;
	Cons *list = create_cons_cell(is_empty ? NULL : argv[0].p, NULL, is_empty);
	int i;
	// Link Cons cells
	for(i = 0; i < argc - 1; i++) {
		Cons *cons_i = argv[i].p;
		cons_i->cdr = argv[i + 1].p;
	}
	return (pt_data){ .p = list };
}

int main(int argc, char **argv) {
	assert(argc > 1 && "Need a string to parse!");

	/* LispCode <- Spaces Sexp* !.
	 * Sexp <- (List / Atom) Spaces
	 * List <- "(" Spaces Sexp* ")"
	 * Atom <- (!("(" / ")" / Space) .)+
	 * Spaces <- Space*
	 * Space <- Comment / \s
	 * Comment <- ";" [^\n]*
	 */
	pt_rule R[] = {
		{ "LispCode", SEQ(V("Spaces"), Q(V("Sexp"), 0), NOT(ANY)) },
		{ "Sexp", SEQ(OR(V("List"), V("Atom")), V("Spaces")) },
		{ "List", SEQ_(list, L("("), V("Spaces"), Q(V("Sexp"), 0), L(")")) },
		{ "Atom", Q_(atom, BUT(OR(S("()"), V("Space"))), 1) },
		{ "Spaces", Q(V("Space"), 0) },
		{ "Space", OR(V("Comment"), F(isspace)) },
		{ "Comment", SEQ(L(";"), Q(BUT(L("\n")), 0)) },
		{ NULL, NULL },
	};
	pt_grammar *g = pt_create_grammar(R, 0);
	pt_grammar_validate(g, PT_VALIDATE_ABORT);

	pt_match_result res = pt_match_grammar(g, argv[1], NULL);
	if(res.matched >= 0) {
		Cons *list = res.data.p;
		print_cons_list(list);
		printf("\n");
		destroy_cons_list(list);
	}
	else {
		puts("Invalid string");
	}

	pt_destroy_grammar(g);
	return 0;
}
