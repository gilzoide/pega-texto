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

/** @file match.h
 * Matching algorithm for the Parsing Expression Grammars.
 */

#ifndef __PEGA_TEXTO_MATCH_H__
#define __PEGA_TEXTO_MATCH_H__

#include "expr.h"
#include "grammar.h"
#include "action.h"

#include <stdlib.h>

/**
 * Result of the `pt_match*` functions.
 *
 * This is used only for easing the error handling, as any non-negative result
 * is considered a successful match.
 */
typedef enum {
	PT_NO_STACK_MEM = -2,  ///< Error while allocating memory for the State Stack.
	PT_NO_MATCH = -1,  ///< Subject string didn't match the given PEG.
} pt_match_result;

/**
 * Options passed to the `pt_match*` functions.
 *
 * This contains callbacks to be called, as well as some memory usage control.
 */
typedef struct {
	int initial_stack_capacity;  ///< The initial capacity for the stack. If 0, stack capacity will begin at a reasonable default
	pt_iteration_action each_iteration;  ///< The action to be performed on each iteration
	pt_capture_action on_capture;  ///< The action to be performed for the captures, when the whole match succeeds
	pt_success_action on_success;  ///< The action to be performed when the whole match succeeds
	pt_success_action each_success;  ///< The action to be performed when any match succeeds
	pt_fail_action on_fail;  ///< The action to be performed when the whole match fails
	pt_fail_action each_fail;  ///< The action to be performed when the whole match fails
	void *userdata;  ///< Custom user data for the actions
} pt_match_options;

/// Default match options: all 0 or NULL =P
extern pt_match_options pt_default_match_options;

/**
 * Try to match the string `str` with a PEG.
 *
 * @warning This function doesn't check for ill-formed grammars, so it's advised
 *          that you validate it before running the match algorithm.
 * @warning When searching for a Non-terminal numerical index from Rule name,
 *          the `names` array existance or bounds are not checked, possibly
 *          resulting in a SEGFAULT. These can be taken care of by validating
 *          the grammar.
 *
 * @param es    Expression array of arbitrary size. For a single Expression,
 *              just pass a pointer to it.
 * @param names Rule names, for indexing Non-terminal Expressions.
 * @param str   Subject string to match.
 * @param opts  Match options. If NULL, pega-texto will use the default value
 *              @ref pt_default_match_options.
 * @return Number of characters matched, if non-negative. If negative, it's an
 *         error code.
 */
pt_match_result pt_match(pt_expr **es, const char **names, const char *str, pt_match_options *opts);
/**
 * Try to match the string `str` with a PEG composed by an unamed single Expression.
 *
 * This is useful for simple pattern matching against a single Expression.
 * Calls @ref pt_match internally, so all of its caveats apply.
 *
 * @param e    Expression.
 * @param str  Subject string to match.
 * @param opts Match options. If NULL, pega-texto will use the default value
 * @return Number of characters matched, if non-negative. If negative, it's an
 *         error code.
 */
pt_match_result pt_match_expr(pt_expr *e, const char *str, pt_match_options *opts);
/**
 * Try to match the string `str` with a PEG.
 *
 * Calls @ref pt_match internally, so all of its caveats apply.
 *
 * @param g    Grammar.
 * @param str  Subject string to match.
 * @param opts Match options. If NULL, pega-texto will use the default value
 * @return Number of characters matched, if non-negative. If negative, it's an
 *         error code.
 */
pt_match_result pt_match_grammar(pt_grammar *g, const char *str, pt_match_options *opts);

#endif
