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

/** @file match.h
 * Matching algorithm for the Parsing Expression Grammars.
 */

#ifndef __PEGA_TEXTO_MATCH_H__
#define __PEGA_TEXTO_MATCH_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "pega-texto/action.h"
#include "pega-texto/data.h"
#include "pega-texto/expr.h"
#include "pega-texto/grammar.h"

#include <stdlib.h>

/**
 * Possible error codes returned by the `pt_match*` functions.
 */
typedef enum {
	/// Subject string didn't match the given PEG.
	PT_NO_MATCH = -1,
	/// Error while allocating memory for the State/Action Stack.
	PT_NO_STACK_MEM = -2,
	/// Matched an Error Expression.
	PT_MATCHED_ERROR = -3,
	/// Provided string is a NULL pointer.
	PT_NULL_INPUT = -4,
	/// VM has no bytecode loaded.
	PT_VM_NULL_BYTECODE = -5,
	/// An invalid instruction was found in bytecode.
	PT_VM_INVALID_INSTRUCTION = -6,
} pt_macth_error_code;

/**
 * Match result: a {number of matched chars/match error code, action
 * result/syntactic error code} pair.
 *
 * This is returned by the `pt_match*` functions.
 */
typedef struct pt_match_result {
	/**
	 * If non-negative, represents the number of characters matched;
	 * otherwise, it's an error code.
	 */
	int matched;
	/**
	 * Resulting data from the last top-level Action or first syntactic error
	 * found.
	 *
	 * If `matched` is `PT_MATCHED_ERROR`, `data.i` will be populated with the
	 * first syntactic error code found. This is useful if you don't want to
	 * specify a #pt_error_action just for capturing the error code.
	 *
	 * @note If you need a single result for all top-level Actions, just create
	 * an outer one that folds them (which will always be the last top-level
	 * one).
	 */
	pt_data data;
} pt_match_result;

/**
 * Options passed to the `pt_match*` functions.
 *
 * This contains callbacks to be called, as well as some memory usage control.
 *
 * @note The first field is the userdata, so you can easily initialize it with
 *       an aggregate initializer.
 * @warning Zero initialize every unused option before calling `pt_match*`.
 */
typedef struct {
	void *userdata;  ///< Custom user data for the actions
	pt_iteration_action each_iteration;  ///< The action to be performed on each iteration
	pt_end_action on_end;  ///< The action to be performed when the match algorithm ends
	pt_success_action each_success;  ///< The action to be performed when any match succeeds
	pt_fail_action each_fail;  ///< The action to be performed when the whole match fails
	pt_error_action on_error;  ///< The action to be performed when a syntactic error is found
	int initial_stack_capacity;  ///< The initial capacity for the stack. If 0, stack capacity will begin at a reasonable default
} pt_match_options;

/// Default match options: all 0 or NULL =P
extern const pt_match_options pt_default_match_options;

/**
 * Try to match the string `str` with a PEG composed by Expressions and their names.
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
 * @return Number of matched characters/error code, result of Action folding.
 */
pt_match_result pt_match(pt_expr **es, const char **names, const char *str, const pt_match_options *opts);
/**
 * Try to match the string `str` with a PEG composed by an unamed single Expression.
 *
 * This is useful for simple pattern matching against a single Expression.
 * Calls #pt_match internally, so all of its caveats apply.
 *
 * @param e    Expression.
 * @param str  Subject string to match.
 * @param opts Match options. If NULL, pega-texto will use the default value
 * @return Number of matched characters/error code, result of Action folding.
 */
pt_match_result pt_match_expr(pt_expr *e, const char *str, const pt_match_options *opts);
/**
 * Try to match the string `str` with a PEG.
 *
 * Calls #pt_match internally, so all of its caveats apply.
 *
 * @param g    Grammar.
 * @param str  Subject string to match.
 * @param opts Match options. If NULL, pega-texto will use the default value
 * @return Number of matched characters/error code, result of Action folding.
 */
pt_match_result pt_match_grammar(pt_grammar *g, const char *str, const pt_match_options *opts);

#ifdef __cplusplus
}
#endif

#endif
