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

#include "_match-state.h"

int pt_initialize_state_stack(pt_match_state_stack *s, size_t initial_capacity) {
	if(initial_capacity == 0) initial_capacity = 8;
	if(s->states = malloc(initial_capacity * sizeof(pt_match_state))) {
		s->size = 0;
		s->capacity = initial_capacity;
		return 1;
	}
	else return 0;
}

void pt_destroy_state_stack(pt_match_state_stack *s) {
	free(s->states);
}

pt_match_state *pt_push_state(pt_match_state_stack *s, pt_expr *e, size_t pos) {
	pt_match_state *state;
	// Double capacity, if reached
	if(s->size == s->capacity) {
		int new_capacity = s->capacity << 1;
		if(state = realloc(s->states, new_capacity * sizeof(pt_match_state))) {
			s->capacity = new_capacity;
			s->states = state;
		}
		else return NULL;
	}
	state = s->states + (s->size)++;
	state->e = e;
	state->pos = pos;
	state->reg = 0;

	return state;
}

pt_match_state *pt_get_current_state(const pt_match_state_stack *s) {
	int i = s->size - 1;
	return i >= 0 ? s->states + i : NULL;
}
