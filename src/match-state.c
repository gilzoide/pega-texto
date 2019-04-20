/*
 * Copyright 2017-2019 Gil Barbosa Reis <gilzoide@gmail.com>
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

#include "_match-state.h"


int pt_initialize_state_stack(pt_match_state_stack *s, size_t initial_capacity) {
	if(initial_capacity == 0) {
		initial_capacity = PT_DEFAULT_INITIAL_STACK_CAPACITY;
	}
	if(s->states = malloc(initial_capacity * sizeof(pt_match_state))) {
		s->size = 0;
		s->capacity = initial_capacity;
		return 1;
	}
	else {
		return 0;
	}
}

void pt_destroy_state_stack(pt_match_state_stack *s) {
	free(s->states);
}

pt_match_state *pt_push_state(pt_match_state_stack *s, pt_expr *e, size_t pos, size_t ac) {
	pt_match_state *state;
	// Double capacity, if reached
	if(s->size == s->capacity) {
		size_t new_capacity = s->capacity * 2;
		if(state = realloc(s->states, new_capacity * sizeof(pt_match_state))) {
			s->capacity = new_capacity;
			s->states = state;
		}
		else {
			return NULL;
		}
	}
	state = s->states + (s->size)++;
	state->e = e;
	state->pos = pos;
	state->r1 = state->r2 = 0;
	state->ac = ac;
	state->qa = 0;

	return state;
}

pt_match_state *pt_get_current_state(const pt_match_state_stack *s) {
	int i = s->size - 1;
	return i >= 0 ? s->states + i : NULL;
}

int pt_initialize_action_stack(pt_match_action_stack *a, size_t initial_capacity) {
	if(initial_capacity == 0) {
		initial_capacity = PT_DEFAULT_INITIAL_STACK_CAPACITY;
	}
	if(a->actions = malloc(initial_capacity * sizeof(pt_match_action))) {
		a->size = 0;
		a->capacity = initial_capacity;
		return 1;
	}
	else {
		return 0;
	}
}

void pt_destroy_action_stack(pt_match_action_stack *a) {
	free(a->actions);
}

pt_match_action *pt_push_action(pt_match_action_stack *a, pt_expression_action f, size_t start, size_t end, int argc) {
	pt_match_action *action;
	// Double capacity, if reached
	if(a->size == a->capacity) {
		int new_capacity = a->capacity * 2;
		if(action = realloc(a->actions, new_capacity * sizeof(pt_match_action))) {
			a->capacity = new_capacity;
			a->actions = action;
		}
		else {
			return NULL;
		}
	}
	action = a->actions + (a->size)++;
	action->f = f;
	action->start = start;
	action->end = end;
	action->argc = argc;

	return action;
}
