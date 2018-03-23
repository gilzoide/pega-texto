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

/** @file match-state.h
 * States for the matching algorithm, including a State Stack
 */

#ifndef __PEGA_TEXTO_MATCH_STATE_H__
#define __PEGA_TEXTO_MATCH_STATE_H__

#include "pega-texto/expr.h"
#include "pega-texto/action.h"

#include <stdlib.h>

/// Default initial stack capacity.
#define PT_DEFAULT_INITIAL_STACK_CAPACITY 8

/**
 * A State on the Matching algorithm.
 */
typedef struct {
	pt_expr *e;  ///< Current expression being matched.
	size_t pos;  ///< Current position in the stream.
	int r1;  ///< General purpose register 1.
	unsigned int r2;  ///< General purpose register 2.
	unsigned int ac;  ///< Action counter.
	unsigned int qa;  ///< Number of queried Actions.
} pt_match_state;

/**
 * Dynamic sequential stack of States.
 */
typedef struct pt_match_state_stack {
	pt_match_state *states;  ///< States buffer.
	size_t size;  ///< Current number of States.
	size_t capacity;  ///< Capacity of the States buffer.
} pt_match_state_stack;

/**
 * Get the current State on top of the State Stack
 *
 * @param s The state stack.
 * @return Current State, if there is any, `NULL` otherwise.
 */
pt_match_state *pt_get_current_state(const pt_match_state_stack *s);

#endif

