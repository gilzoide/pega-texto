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

/* Auxiliary header for the state of the matching algorithm */

#ifndef __PEGA_TEXTO_MATCH_STATE_H__
#define __PEGA_TEXTO_MATCH_STATE_H__

#include "expr.h"

#include <stdlib.h>

/// The State for the Matching algorithm
typedef struct {
	pt_expr *e;  // Current expression being matched
	size_t pos;  // Current position in the stream
	int reg;  // General purpose register
} pt_match_state;

/// Dynamic sequencial stack of States
typedef struct {
	pt_match_state *states;  // States buffer
	size_t size;  // Current number of States
	size_t capacity;  // Capacity of the States buffer
} pt_match_state_stack;

int pt_initialize_state_stack(pt_match_state_stack *s, size_t initial_capacity);
void pt_destroy_state_stack(pt_match_state_stack *s);

pt_match_state *pt_push_state(pt_match_state_stack *s, pt_expr *e, size_t pos);
/// Propagate success back until reach a Quantifier or Sequence, incrementing it's position
pt_match_state *pt_match_succeed(pt_match_state_stack *s, size_t new_pos);
/// Return to a backtrack point: either Quantifier or Choice
pt_match_state *pt_match_fail(pt_match_state_stack *s);

#endif

