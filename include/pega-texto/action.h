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

#include "pega-texto/data.h"

#include <stdlib.h>

/// Forward declaration
typedef struct pt_match_state_stack pt_match_state_stack;
typedef struct pt_match_result pt_match_result;

/**
 * Action to be called on an Expression, after the whole match succeeds.
 *
 * This is the action to be set to an Expression individually, and will be
 * called only if the whole match succeeds, in the order the Expressions were
 * matched.
 *
 * Expression Actions reduce inner Actions' result into a single value.
 *
 * Parameters:
 * - The original subject string
 * - The start index of the match/capture
 * - The final index of the match/capture
 * - Number of #pt_data arguments 
 * - #pt_data arguments, processed on inner Actions. This array is static and
 *   __will not__ exist after the Action returns.
 * - User custom data from match options
 *
 * Return:
 *   Anything you want.
 *   This result will be used as argument for other actions below in the stack.
 *
 * @sa @ref InfixCalculator.c
 * @sa @ref Lisp.c
 * @sa @ref Re.c
 */
typedef pt_data(*pt_expression_action)(const char *,
                                       size_t,
                                       size_t,
                                       int,
                                       pt_data *,
                                       void *);

/**
 * Action to be called when an Error Expression is matched (on syntatic errors).
 *
 * Parameters:
 * - The original subject string
 * - The position where the error was encountered
 * - The error code
 * - User custom data from match options
 */
typedef void(*pt_error_action)(const char *,
                               size_t,
                               int,
                               void *);


/**
 * Queried actions, to be executed on match success.
 */
typedef struct {
	pt_expression_action f;  ///< Action function.
	size_t start;  ///< Start point of the match.
	size_t end;  ///< End point of the match.
} pt_match_action;

/**
 * Dynamic sequential stack of Actions.
 */
typedef struct {
	pt_match_action *actions;  ///< Queried Actions buffer.
	size_t size;  ///< Current number of Queried Actions.
	size_t capacity;  ///< Capacity of the Queried Actions buffer.
} pt_match_action_stack;


/**
 * Action to be called on each match iteration.
 *
 * Parameters:
 * - A const pointer for the current State Stack, so you can examine it (if desired)
 * - A const pointer for the current Action Stack, so you can examine it (if desired)
 * - The original subject string
 * - User custom data from match options
 */
typedef void(*pt_iteration_action)(const pt_match_state_stack *,
                                   const pt_match_action_stack *,
                                   const char *,
                                   void *);

/**
 * Action to be called when a match succeeds.
 *
 * Parameters:
 * - A const pointer for the current State Stack, so you can examine it (if desired)
 * - A const pointer for the current Action Stack, so you can examine it (if desired)
 * - The original subject string
 * - The start index of the match/capture
 * - The final index of the match/capture
 * - User custom data from match options
 */
typedef void(*pt_success_action)(const pt_match_state_stack *,
                                 const pt_match_action_stack *,
                                 const char *,
                                 size_t,
                                 size_t,
                                 void *);

/**
 * Action to be called when a match fails.
 *
 * Parameters:
 * - A const pointer for the current State Stack, so you can examine it (if desired)
 * - A const pointer for the current Action Stack, so you can examine it (if desired)
 * - The original subject string
 * - User custom data from match options
 */
typedef void(*pt_fail_action)(const pt_match_state_stack *,
                              const pt_match_action_stack *,
                              const char *,
                              void *);

/**
 * Action to be called when the match algorithm ends, either with success or failure.
 *
 * Parameters:
 * - A const pointer for the current State Stack, so you can examine it (if desired)
 * - A const pointer for the current Action Stack, so you can examine it (if desired)
 * - The original subject string
 * - User custom data from match options
 */
typedef void(*pt_end_action)(const pt_match_state_stack *,
                             const pt_match_action_stack *,
                             const char *,
                             pt_match_result,
                             void *);

/**
 * @example FindIncludes.c
 * Simple example of a #pt_expression_action that prints the matched input.
 *
 * @verbatim
#include <pega-texto.h>
#include <pega-texto/macro-on.h>
#include "readfile.h"
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <assert.h>
Found 7 includes
@endverbatim
 * is the result of applying the program on it's code.
 */

/**
 * @example InfixCalculator.c
 * #pt_expression_action example that folds inner Actions' results into one.
 */

/**
 * @example Lisp.c
 * #pt_expression_action example that folds a Lisp-like nested parentheses
 * language into nested Cons lists.
 */

/**
 * @example Re.c
 * Complex #pt_expression_action example that creates PEGs using a Regex-like
 * syntax, and then matches the syntax described by itself.
 */

#endif

