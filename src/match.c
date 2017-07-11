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
#include "_match-state.h"

#include <stdlib.h>
#include <string.h>

/// Look for a name in the array.
// @warning This doesn't check for NULLs, nor array size: `name` is better be in `names`!
static int pt_find_non_terminal_index(const char *name, const char **names) {
	int i;
	for(i = 0; strcmp(name, names[i]) != 0; i++);
	return i;
}

/// Propagate success back until reach a Quantifier, Sequence or Not, changing it's position
pt_match_state *pt_match_succeed(pt_match_state_stack *s, const char *str, size_t new_pos, pt_match_options *opts) {
	int i, op;
	if(opts->each_success) {
		opts->each_success(s, str, new_pos - s->states[s->size - 1].pos, opts->userdata);
	}
	for(i = s->size - 2; i >= 0; i--) {
		op = s->states[i].e->op;
		switch(op) {
			case PT_QUANTIFIER:
			case PT_SEQUENCE:
				s->states[i].pos = new_pos;
				goto end;

			case PT_AND:
				new_pos = s->states[i].pos;
				break;

			case PT_NOT:
				s->states[i].reg = -1;
				goto end;
		}
	}
end:
	if(i >= 0) {
		s->size = i + 1;
		return s->states + i;
	}
	else {
		// aaaaaand ACTION!
		if(opts->on_success) {
			opts->on_success(s, str, new_pos, opts->userdata);
		}
		s->states[0].pos = new_pos;
		return NULL;
	}
}

/// Return to a backtrack point: either Quantifier or Choice
pt_match_state *pt_match_fail(pt_match_state_stack *s, const char *str, pt_match_options *opts) {
	int i, op;
	if(opts->each_fail) {
		opts->each_fail(s, str, opts->userdata);
	}
	for(i = s->size - 2; i >= 0; i--) {
		op = s->states[i].e->op;
		switch(op) {
			case PT_QUANTIFIER:
				s->states[i].reg = -(s->states[i].reg);
			case PT_CHOICE:
				goto end;

			case PT_NOT:
				s->states[i].reg = 1;
				goto end;
		}
	}
end:
	if(i >= 0) {
		s->size = i + 1;
		return s->states + i;
	}
	else {
		// aaaaaand ACTION!
		if(opts->on_fail) {
			opts->on_fail(s, str, opts->userdata);
		}
		return NULL;
	}
}

pt_match_result pt_match(pt_expr **es, const char **names, const char *str, pt_match_options *opts) {
	pt_match_state_stack S;
	if(opts == NULL) opts = &pt_default_match_options;
	if(!pt_initialize_state_stack(&S, opts->initial_stack_capacity)) return PT_NO_STACK_MEM;

	// iteration variables
	pt_match_state *state = pt_push_state(&S, es[0], 0);
	pt_expr *e;
	const char *ptr;
	int matched;

	// match loop
	while(state) {
		if(opts->each_iteration) opts->each_iteration(&S, str, opts->userdata);
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

			case PT_QUANTIFIER:
				// "at least N" quantifier
				if(e->N >= 0) {
					if(state->reg >= 0) goto iterate_quantifier;
					else if(-(state->reg) > e->N) matched = 0;
				}
				// "at most N" quantifier
				else {
					if(state->reg >= 0) {
						if(state->reg < -(e->N)) goto iterate_quantifier;
						else matched = 0;
					}
					else if(state->reg >= e->N - 1) matched = 0;
				}
				break;
iterate_quantifier:
				state->reg++;
				state = pt_push_state(&S, e->data.e, state->pos);
				continue;

			case PT_NOT:
				// was failing, so succeed!
				if(state->reg > 0) {
					matched = 0;
					break;
				}
				// was succeeding, so fail!
				else if(state->reg < 0) break;
				// none, fallthrough
			case PT_AND: case PT_CAPTURE:
				state = pt_push_state(&S, e->data.e, state->pos);
				continue;

			case PT_SEQUENCE:
				if(state->reg < e->N) {
					state = pt_push_state(&S, e->data.es[state->reg++], state->pos);
					continue;
				}
				else matched = 0;
				break;

			case PT_CHOICE:
				if(state->reg < e->N) {
					state = pt_push_state(&S, e->data.es[state->reg++], state->pos);
					continue;
				}
				break;

			case PT_CUSTOM_MATCHER:
				if(e->data.matcher(*ptr)) {
					matched = 1;
				}
				break;

			// Unknown operation: always fail
			default: break;
		}

		state = matched < 0 ? pt_match_fail(&S, str, opts) : pt_match_succeed(&S, str, state->pos + matched, opts);
	}

	if(matched >= 0) {
		matched = S.states[0].pos;
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

pt_match_options pt_default_match_options = {};
