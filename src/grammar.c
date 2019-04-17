/*
 * Copyright 2017, 2018 Gil Barbosa Reis <gilzoide@gmail.com>
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

int pt_init_grammar(pt_grammar *g, pt_rule *rules, uint8_t own_names) {
	if(g) {
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
			if(g->es) {
				free(g->es);
			}
			*g = (pt_grammar){};
			return 0;
		}
	}
	return g != NULL;
}

void pt_release_grammar(pt_grammar *g) {
	if(g) {
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
		*g = (pt_grammar){};
	}
}

