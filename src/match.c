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

#include "match.h"
#include "match-state.h"

#include <stdlib.h>
#include <string.h>

/// Look for a name in the array.
// @warning This doesn't check for NULLs, nor array size: `name` is better be in `names`!
static int pt_find_non_terminal_index(const char *name, const char **names) {
	int i;
	for(i = 0; strcmp(name, names[i]) != 0; i++);
	return i;
}

pt_match_result pt_match(pt_expr **es, const char **names, const char *str, pt_match_options *opts) {
	pt_match_state_stack S;
	if(!pt_initialize_state_stack(&S, 8)) return PT_NO_STACK_MEM;

	// iteration variables
	pt_match_state *state = pt_push_state(&S, es[0], 0);
	pt_expr *e;
	const char *ptr;
	int matched;

	// match loop
	while(state) {
		ptr = str + state->pos;
		e = state->e;
		matched = -1;

		switch(e->op) {
			// Primary
			case PT_LITERAL:
				if(strncmp(ptr, e->data.characters, e->N) == 0) {
					matched = e->N;
				}
				break;
			case PT_SET:
				if(*ptr && strchr(e->data.characters, *ptr)) {
					matched = 1;
				}
				break;
			case PT_RANGE:
				if(*ptr >= e->data.characters[0] && *ptr <= e->data.characters[1]) {
					matched = 1;
				}
				break;
			case PT_ANY:
				if(*ptr) {
					matched = 1;
				}
				break;
			// Unary
			case PT_NON_TERMINAL:
				if(e->N < 0) {
					e->N = pt_find_non_terminal_index(e->data.characters, names);
				}
				state = pt_push_state(&S, es[e->N], state->pos);
				continue;
			/* case PT_QUANTIFIER: */
				/* // match at most N */
				/* if(e->N < 0) { */
				/* } */
				/* // match at least N */
				/* else { */
				/* } */
				/* break; */
		}

		state = matched < 0 ? pt_match_fail(&S) : pt_match_succeed(&S, matched);
	}

	pt_destroy_state_stack(&S);
	return matched;
}

pt_match_result pt_match_expr(pt_expr *e, const char *str, pt_match_options *opts) {
	return pt_match(&e, NULL, str, opts);
}

pt_match_result pt_match_grammar(pt_grammar *g, const char *str, pt_match_options *opts) {
	return pt_match(g->es, g->names, str, opts);
}

