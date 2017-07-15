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

/** @file action.h
 * Actions to be performed on certain points of the matching algorithm.
 */

#ifndef __PEGA_TEXTO_ACTION_H__
#define __PEGA_TEXTO_ACTION_H__

#include <stdlib.h>

// Forward declaration
typedef struct pt_match_state_stack_t pt_match_state_stack;

/**
 * Action to be called on each match iteration.
 *
 * Parameters:
 * - A const pointer for the current State Stack, so you can examine it (if desired)
 * - The original subject string
 * - User custom data from match options
 */
typedef void (*pt_iteration_action)(const pt_match_state_stack *, const char *,
                                    void *);

/**
 * Action to be called when the whole match succeeds.
 *
 * Parameters:
 * - A const pointer for the current State Stack, so you can examine it (if desired)
 * - The original subject string
 * - The start index of the match/capture
 * - The final index of the match/capture
 * - User custom data from match options
 */
typedef void(*pt_success_action)(const pt_match_state_stack *, const char *,
                                 size_t, size_t, void *);

/**
 * Action to be called when the whole match fails.
 *
 * Parameters:
 * - A const pointer for the current State Stack, so you can examine it (if desired)
 * - The original subject string
 * - User custom data from match options
 */
typedef void(*pt_fail_action)(const pt_match_state_stack *, const char *, void *);

#endif

