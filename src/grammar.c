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

