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

/** @file _match-state.h
 * Internal functions for the State Stack, to be used only by the `pt_match`
 * function.
 */

#ifndef __PEGA_TEXTO__MATCH_STATE_H__
#define __PEGA_TEXTO__MATCH_STATE_H__

#include <pega-texto/match-state.h>

/**
 * Initializes the State Stack, `malloc`ing the stack with `initial_capacity`.
 *
 * @param s                The state stack to be initialized.
 * @param initial_capacity The initial stack capacity. If 0, stack is
 *                         initialized with a default value.
 * @return 1 if the allocation went well, 0 otherwise
 */
int pt_initialize_state_stack(pt_match_state_stack *s, size_t initial_capacity);

/**
 * Destroy the State Stack, freeing the memory used.
 *
 * @param s The state stack to be destroyed.
 */
void pt_destroy_state_stack(pt_match_state_stack *s);

/**
 * Push a State into the State Stack, doubling it's capacity, if needed.
 *
 * @param s   The state stack.
 * @param e   The Parsing Expression to be used for the next iteration.
 * @param pos The starting position of the stream for next iteration.
 * @param ac  The new Action counter.
 * @return The newly pushed State.
 */
pt_match_state *pt_push_state(pt_match_state_stack *s, pt_expr *e, size_t pos, size_t ac);


/**
 * Initializes the State Stack, `malloc`ing the stack with `initial_capacity`.
 *
 * @param a                The action stack to be initialized.
 * @param initial_capacity The initial stack capacity. If 0, stack is
 *                         initialized with a default value.
 * @return 1 if the allocation went well, 0 otherwise
 */
int pt_initialize_action_stack(pt_match_action_stack *s, size_t initial_capacity);

/**
 * Destroy the Action Stack, freeing the memory used.
 *
 * @param a The action stack to be destroyed.
 */
void pt_destroy_action_stack(pt_match_action_stack *s);

/**
 * Push an Action into the Action Stack, doubling it's capacity, if needed.
 *
 * @param a     The action stack.
 * @param f     The function to be called as action.
 * @param start The starting position of the stream for action.
 * @param end   The ending position of the stream for action.
 * @param argc  Number of arguments (inner action results) used by this action.
 * @return The newly pushed State.
 */
pt_match_action *pt_push_action(pt_match_action_stack *a,
                                pt_expression_action f,
                                size_t start, size_t end,
                                int argc);

#endif

