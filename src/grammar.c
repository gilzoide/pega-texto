/*
 * Copyright 2017 Gil Barbosa Reis <gilzoide@gmail.com>
 * This file is part of pega-texto.
 * 
 * Pega-texto is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Pega-texto is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with pega-texto.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Any bugs should be reported to <gilzoide@gmail.com>
 */

#include <pega-texto/grammar.h>

#include <stdlib.h>
#include <string.h>

pt_grammar *pt_create_grammar(pt_rule *rules, uint8_t own_names) {
	pt_grammar *g;
	if(g = malloc(sizeof(pt_grammar))) {
		int i;
		// find size
		for(i = 0; rules[i].e != NULL; i++);
		g->N = i;
		if((g->es = malloc(i * sizeof(pt_expr *))) && (g->names = malloc(i * sizeof(const char *)))) {
			for(i = 0; i < g->N; i++) {
				g->names[i] = rules[i].name;
				g->es[i] = rules[i].e;
			}
			g->own_names = own_names;
		}
		// malloc error
		else {
			if(g->es) free(g->es);
			free(g);
			g = NULL;
		}
	}
	return g;
}

void pt_destroy_grammar(pt_grammar *g) {
	int i;
	// expressions
	for(i = 0; i < g->N; i++) {
		pt_destroy_expr(g->es[i]);
	}
	free(g->es);
	// names
	if(g->own_names) {
		for(i = 0; i < g->N; i++) {
			free((void *) g->names[i]);
		}
	}
	free(g->names);
	
	free(g);
}

/// Look for a name in the array.
static inline int pt_find_non_terminal_index(const char *name, const char **names, int N) {
	int i;
	for(i = 0; i < N; i++) {
		if(strcmp(name, names[i]) == 0) return i;
	}
	return -1;
}

/// Validates a single expression in the Grammar, to be called recursively.
static int pt_expr_in_grammar_validate(pt_grammar *g, pt_expr *e, const char *rule) {
	int i, res;
	switch(e->op) {
		case PT_RANGE:
			if(e->data.characters == NULL) return PT_VALIDATE_NULL_STRING;
			else if(strlen(e->data.characters) < 2) return PT_VALIDATE_RANGE_BUFFER;
			else if(e->data.characters[0] > e->data.characters[1]) return PT_VALIDATE_INVALID_RANGE;
			break;

		case PT_NON_TERMINAL:
			if(e->N < 0) { 
				if((e->N = pt_find_non_terminal_index(e->data.characters, g->names, g->N)) == -1) {
					return PT_VALIDATE_UNDEFINED_RULE;
				}
			}
			else {
				if(e->N >= g->N) return PT_VALIDATE_OUT_OF_BOUNDS;
			}
			break;

		case PT_QUANTIFIER:
			// TODO
			break;

		case PT_AND: case PT_NOT:
			return pt_expr_in_grammar_validate(g, e->data.e, rule);

		case PT_SEQUENCE:
			// TODO
			break;

		case PT_CHOICE:
			for(i = 0; i < e->N; i++) {
				if((res = pt_expr_in_grammar_validate(g, e->data.es[i], rule)) != PT_VALIDATE_SUCCESS) {
					return res;
				}
			}
			break;
	}
	return PT_VALIDATE_SUCCESS;
}

int pt_grammar_validate(pt_grammar *g) {
	return pt_expr_in_grammar_validate(g, g->es[0], g->names[0]);
}

