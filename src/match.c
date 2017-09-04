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

#include <pega-texto/match.h>
#include "_match-state.h"
#include "_action.h"

#include <stdlib.h>
#include <string.h>

/// Look for a name in the array.
// @warning This doesn't check for NULLs, nor array size: `name` is better be in `names`!
static inline int pt_find_non_terminal_index(const char *name, const char **names) {
	int i;
	for(i = 0; strcmp(name, names[i]) != 0; i++);
	return i;
}

/// Propagate success back until reach a Quantifier, Sequence, And or Not, changing it's position
static pt_match_state *pt_match_succeed(pt_match_state_stack *s, pt_match_action_stack *a,
		int *matched, const char *str, size_t new_pos, pt_match_options *opts) {
	int i;
	pt_match_state *state = s->states + s->size - 1;
	if(opts->each_success) {
		opts->each_success(s, a, str, state->pos, new_pos, opts->userdata);
	}
	if(state->e->action) {
		if(pt_push_action(a, state->e->action, state->pos, new_pos) == NULL) {
			*matched = PT_NO_STACK_MEM;
			return NULL;
		}
	}
	for(i = s->size - 2; i >= 0; i--) {
		state = s->states + i;
		switch(state->e->op) {
			case PT_QUANTIFIER:
			case PT_SEQUENCE:
				state->r2 = new_pos - state->pos; // mark current match accumulator
				state->ac = a->size; // keep queried actions
				goto backtrack;

			case PT_AND:
				new_pos = state->pos; // don't consume input...
				a->size = state->ac; // ...nor keep queried actions (still succeeds, though)
				break;

			case PT_NOT:
				state->r1 = -1; // NOT success = fail
				a->size = state->ac; // discard queried actions
				goto backtrack;

			case PT_ERROR:
				pt_destroy_expr(s->states[i + 1].e);  // destroy the syncronization expression wrapper
				break;

			default: // query action, if there is any
				if(state->e->action) {
					if(pt_push_action(a, state->e->action, state->pos, new_pos) == NULL) {
						*matched = PT_NO_STACK_MEM;
						return NULL;
					}
				}
				break;
		}
	}
	// for ended normally: no more states left
	*matched = new_pos;
	return NULL;

backtrack:
	s->size = i + 1;
	return state;
}

/// Return to a backtrack point: either Quantifier, Choice or Not
static pt_match_state *pt_match_fail(pt_match_state_stack *s, pt_match_action_stack *a,
		const char *str, pt_match_options *opts) {
	int i;
	pt_match_state *state;
	if(opts->each_fail) {
		opts->each_fail(s, a, str, opts->userdata);
	}
	for(i = s->size - 2; i >= 0; i--) {
		state = s->states + i;
		switch(state->e->op) {
			case PT_QUANTIFIER:
				state->r1 = -(state->r1); // mark end of quantifier matching
			case PT_CHOICE:
				goto backtrack;

			case PT_NOT:
				state->r1 = 1; // NOT fail = success
				goto backtrack;

			case PT_ERROR:
				pt_destroy_expr(s->states[i + 1].e);  // destroy the syncronization expression wrapper
				break;
		}
	}
	// for ended normally: no more states left
	return NULL;

backtrack:
	// rewind stacks to backtrack point
	s->size = i + 1;
	a->size = state->ac;
	return s->states + i;
}

/// An error was found: push a wrapper of the syncronization Expression, if there is any
#include <pega-texto/macro-on.h>
static pt_match_state *pt_match_error(pt_match_state_stack *s, pt_match_action_stack *a) {
	pt_match_state *state = pt_get_current_state(s);
	if(state->e->data.e) {
		// don't double free the sync Expression, as Error Expression owns it
		pt_expr *but_expr = BUT_NO(state->e->data.e);
		state = pt_push_state(s, Q(but_expr, 0), state->pos, a->size);
		return state;
	}
	else {
		return NULL;
	}
}
#include <pega-texto/macro-off.h>

pt_match_result pt_match(pt_expr **es, const char **names, const char *str, pt_match_options *opts) {
	int matched;
	int matched_error = 0;
	pt_data action_result = {};
	pt_match_state_stack S;
	pt_match_action_stack A;
	if(opts == NULL) {
		opts = &pt_default_match_options;
	}
	if(!pt_initialize_state_stack(&S, opts->initial_stack_capacity)) {
		matched = PT_NO_STACK_MEM;
		goto err_state_stack;
	}
	if(!pt_initialize_action_stack(&A, opts->initial_stack_capacity)) {
		matched = PT_NO_STACK_MEM;
		goto err_action_stack;
	}

	// iteration variables
	pt_match_state *state = pt_push_state(&S, es[0], 0, 0);
	pt_expr *e;
	const char *ptr;

	// match loop
	while(state) {
		if(opts->each_iteration) {
			opts->each_iteration(&S, &A, str, opts->userdata);
		}
		ptr = str + state->pos;
		e = state->e;
		matched = PT_NO_MATCH;

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
				state = pt_push_state(&S, es[e->N], state->pos, A.size);
				continue;

			case PT_QUANTIFIER:
				// "at least N" quantifier
				if(e->N >= 0) {
					if(state->r1 >= 0) {
						goto iterate_quantifier;
					}
					else if(-(state->r1) > e->N) {
						matched = state->r2;
					}
				}
				// "at most N" quantifier
				else {
					if(state->r1 >= 0) {
						if(state->r1 < -(e->N)) {
							goto iterate_quantifier;
						}
						else {
							matched = state->r2;
						}
					}
					else if(state->r1 >= e->N - 1) {
						matched = state->r2;
					}
				}
				break;
iterate_quantifier:
				state->r1++;
				state = pt_push_state(&S, e->data.e, state->pos + state->r2, A.size);
				continue;

			case PT_NOT:
				// was failing, so succeed!
				if(state->r1 > 0) {
					matched = 0;
					break;
				}
				// was succeeding, so fail!
				else if(state->r1 < 0) {
					break;
				}
				// none, fallthrough
			case PT_AND:
				state = pt_push_state(&S, e->data.e, state->pos, A.size);
				continue;

			// N-ary
			case PT_SEQUENCE:
				if(state->r1 < e->N) {
					state = pt_push_state(&S, e->data.es[state->r1++], state->pos + state->r2, A.size);
					continue;
				}
				else {
					matched = state->r2;
				}
				break;

			case PT_CHOICE:
				if(state->r1 < e->N) {
					state = pt_push_state(&S, e->data.es[state->r1++], state->pos, A.size);
					continue;
				}
				break;

			// Others
			case PT_CUSTOM_MATCHER:
				if(e->data.matcher(*ptr)) {
					matched = 1;
				}
				break;

			case PT_ERROR:
				// mark that a syntactic error ocurred, so even syncing we remember this
				matched_error = 1;
				if(opts->on_error) {
					opts->on_error(str, state->pos, e->N, opts->userdata);
				}
				matched = PT_MATCHED_ERROR;
				break;

			// Unknown operation: always fail
			default: break;
		}

		state = matched == PT_NO_MATCH ? pt_match_fail(&S, &A, str, opts)
		      : matched == PT_MATCHED_ERROR ? pt_match_error(&S, &A)
		      : pt_match_succeed(&S, &A, &matched, str, state->pos + matched, opts);
	}

	if(matched_error) {
		matched = PT_MATCHED_ERROR;
	}
	else if(matched >= 0 && A.size > 0) {
		action_result = pt_run_actions(&A, str, opts->userdata);
	}
	if(opts->on_end) {
		opts->on_end(&S, &A, str, (pt_match_result){matched, action_result}, opts->userdata);
	}

	pt_destroy_action_stack(&A);
err_action_stack:
	pt_destroy_state_stack(&S);
err_state_stack:
	return (pt_match_result){matched, action_result};
}

pt_match_result pt_match_expr(pt_expr *e, const char *str, pt_match_options *opts) {
	return pt_match(&e, NULL, str, opts);
}

pt_match_result pt_match_grammar(pt_grammar *g, const char *str, pt_match_options *opts) {
	return pt_match(g->es, g->names, str, opts);
}

pt_match_options pt_default_match_options = {};

