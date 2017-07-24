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

/** @file expr.h
 * Parsing Expressions.
 *
 * Parsing Expressions can be combined to create PEGs, which can match text.
 *
 * Types of Expressions:
 * - __Literal__ (`"string"`): match a string literally.
 * - __Set__ (`[chars]`): match any character in a set.
 * - __Range__ (`[c1-c2]`): match a character between a range.
 * - __Any__ (`.`): only fail at end of stream ('\0').
 * - __Non-terminal__ (`Non-terminal-name`): match the expression in a given
 *   index in the grammar.
 * - __Quantifier__ (`e^N`): If N is non-negative, match N or more occurrences
 *   of `e`. If N is negative, match at most `|N|` occurrences of `e`. In
 *   particular, `e^0 == e*`, `e^1 == e+` and `e^-1 == e?` on the original PEG
 *   notation.
 * - __And__ (`&e`): match Expression without consuming the input.
 * - __Not__ (`!e`): match only if input doesn't match `e`.
 * - __Sequence__ (`e1 e2 ...`): match if all Expressions match in sequence.
 * - __Choice__ (`e1 / e2 / ...`): match either one of the Expressions, trying from
 *   the first one to the last.
 * - __Custom Matcher__: match if function `matcher` applied to next character
 *   returns non-zero.
 */

#ifndef __PEGA_TEXTO_EXPR_H__
#define __PEGA_TEXTO_EXPR_H__

#include "pega-texto/action.h"

#include <stdint.h>

/**
 * Operations for constructing Parsing Expressions.
 */
typedef enum {
	// Primary
	PT_LITERAL,          // "string"
	PT_SET,              // [chars]
	PT_RANGE,            // [c1-c2]
	PT_ANY,              // .
	// Unary
	PT_NON_TERMINAL,     // <non-terminal> // Recurse to non-terminal expression
	PT_QUANTIFIER,       // e^N // If positive, match N or more occurrences of `e`
	                            // If negative, match at most N occurrences of `e`
	                            // e^0  == e*
	                            // e^1  == e+
	                            // e^-1 == e?
	PT_AND,              // &e
	PT_NOT,              // !e
	// N-ary
	PT_SEQUENCE,         // e1 e2
	PT_CHOICE,           // e1 / e2
	// Custom match by function
	PT_CUSTOM_MATCHER,   // function(c) // If return 0, match fails
	                                    // If return nonzero, match succeeds, advance 1
} pt_operation;

/// String version of the possible operations.
extern const char * const pt_operation_names[];

/// A function that receives a character (int) and match it (non-zero) or not (0).
typedef int(*pt_custom_matcher)(int);

/// Parsing Expressions.
typedef struct pt_expr_t {
	/// Expression data, depending on it's operation.
	union {
		/// Literals, Character Sets, Ranges and Non-Terminal names.
		const char *characters;
		/// Quantifier, And & Not: operand.
		struct pt_expr_t *e;
		/// N-ary operators: a N-array of operands.
		struct pt_expr_t **es;
		/// Custom match function.
		pt_custom_matcher matcher;
	} data;
	pt_expression_action action;  ///< Action to be called when the whole match succeeds.
	int16_t N;  ///< Quantifier, array size for N-ary operations, Non-Terminal index or Literal length.
	uint8_t op;  ///< Operation to be performed.
	uint8_t own_characters : 1;  ///< Do Expression own the `characters` buffer?
} pt_expr;

/**
 * Create a Literal Expression.
 *
 * @param str            Literal string to be matched.
 * @param own_characters Should Expression own the `characters` buffer?
 * @param action         Action associated to the Expression.
 */
pt_expr *pt_create_literal(const char *str, uint8_t own_characters, pt_expression_action action);
/**
 * Create a Set Expression.
 *
 * @param str            Character set to be matched.
 * @param own_characters Should Expression own the `characters` buffer?
 * @param action         Action associated to the Expression.
 */
pt_expr *pt_create_set(const char *str, uint8_t own_characters, pt_expression_action action);
/**
 * Create a Range Expression.
 *
 * @param str            Character range to be matched.
 * @param own_characters Should Expression own the `characters` buffer?
 * @param action         Action associated to the Expression.
 */
pt_expr *pt_create_range(const char *str, uint8_t own_characters, pt_expression_action action);
/**
 * Create a Any Expression.
 *
 * @param action Action associated to the Expression.
 */
pt_expr *pt_create_any(pt_expression_action action);
/**
 * Create a Non-terminal Expression, indexing it by the Rule name.
 *
 * @param rule           Rule name.
 * @param own_characters Should Expression own the `characters` buffer?
 * @param action         Action associated to the Expression.
 */
pt_expr *pt_create_non_terminal(const char *rule, uint8_t own_characters, pt_expression_action action);
/**
 * Create a Non-terminal Expression, indexing it by the Rule numerical index.
 *
 * @param index  Rule index.
 * @param action Action associated to the Expression.
 */
pt_expr *pt_create_non_terminal_idx(int index, pt_expression_action action);
/**
 * Create a Quantifier Expression.
 *
 * @param e      Expression.
 * @param N      Quantifier.
 * @param action Action associated to the Expression.
 */
pt_expr *pt_create_quantifier(pt_expr *e, int N, pt_expression_action action);
/**
 * Create an And Expression.
 *
 * @param e Expression.
 */
pt_expr *pt_create_and(pt_expr *e);
/**
 * Create a Not Expression.
 *
 * @param e Expression.
 */
pt_expr *pt_create_not(pt_expr *e);
/**
 * Create a Sequence Expression.
 *
 * @param es     `N` sized array of Expressions.
 * @param N      Array size.
 * @param action Action associated to the Expression.
 */
pt_expr *pt_create_sequence(pt_expr **es, int N, pt_expression_action action);
/**
 * Create a Choice Expression.
 *
 * @param es     `N` sized array of Expressions.
 * @param N      Array size.
 * @param action Action associated to the Expression.
 */
pt_expr *pt_create_choice(pt_expr **es, int N, pt_expression_action action);
/**
 * Create a Custom Matcher Expression.
 *
 * @param f      Custom Matcher function.
 * @param action Action associated to the Expression.
 */
pt_expr *pt_create_custom_matcher(pt_custom_matcher f, pt_expression_action action);

/**
 * Destroy an Expression, freeing the memory used.
 *
 * @warning Every subexpression for unary or N-ary operations will also be
 *          freed.
 * @warning The pointer `e` itself will also be freed, as Expressions are
 *          supposed to be created by the `pt_create_*` functions.
 *
 * @param e Expression to be destroyed.
 */
void pt_destroy_expr(pt_expr *e);

/**
 * Function for creating Sequences and Choices from NULL-terminated arrays.
 *
 * This exists on the sole purpose of making the expression constructor macros
 * for Sequences and Choices possible (`macro-on.h`).
 */
pt_expr *pt__from_nt_array(pt_expr *(*f)(pt_expr **, int, pt_expression_action), pt_expr **nt_exprs, pt_expression_action);

#endif

