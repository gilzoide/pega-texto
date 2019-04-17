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

/** @file expr.h
 * Parsing Expressions that can be combined to create PEGs, which can match text.
 *
 * Types of Expressions:
 * - __Literal__ (`"string"`): match a string literally.
 * - __Case Insensitive__ (`I"string"`): match a string literally, insensitive
 *   to case.
 * - __Character class__ (`int(*)(char)`): match character if function applied
 *   to it returns non-zero. Perfect for the `"is?????"` function family from
 *   "ctype.h".
 * - __Set__ (`[chars]`): match any character in a set.
 * - __Range__ (`[c1-c2]`): match a character between a range.
 * - __Any__ (`.`): only fail at end of stream ('\0').
 * - __Non-terminal__ (`Non-terminal-name or index`): match the expression in a
 *   given index in the Grammar.
 * - __Quantifier__ (`e^N`): If N is non-negative, match N or more occurrences
 *   of `e`. If N is negative, match at most `|N|` occurrences of `e`. In
 *   particular, `e^0 == e*`, `e^1 == e+` and `e^-1 == e?` on the original PEG
 *   notation.
 * - __And__ (`&e`): match Expression without consuming the input.
 * - __Not__ (`!e`): match only if input doesn't match `e`.
 * - __Sequence__ (`e1 e2 ...`): match if all Expressions match in sequence.
 *   An empty Sequence will always match.
 * - __Choice__ (`e1 / e2 / ...`): match either one of the Expressions, trying
 *   from the first one to the last. An empty Choice will always fail.
 * - __Custom Matcher__ (`int(*)(const char *, void *)`): matches the number of
 *   characters as the function returns, if positive. If it returns
 *   zero or negative values, there's no match. Note that if zero was accepted
 *   as match, there was no way to tell if a grammar containing this Expression
 *   was valid. Also note that the function will receive the string at current
 *   match position and should not match more than the string's capacity, which
 *   would cause a segmentation fault.
 * - __Error__: represents a syntactic error, with optional syncronization.
 *   Every error has a numeric code, which should most likely all differ.
 */

#ifndef __PEGA_TEXTO_EXPR_H__
#define __PEGA_TEXTO_EXPR_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "pega-texto/action.h"

#include <stdint.h>

/**
 * Operations for constructing Parsing Expressions.
 */
enum pt_operation {
	// Primary
	PT_LITERAL = 0,          // "string"
	PT_CASE_INSENSITIVE = 1, // I"string"
	PT_CHARACTER_CLASS = 2,  // int(char) // If return 0, match fails
	                                      // If return non-zero, match succeeds, advance 1
	PT_SET = 3,              // [chars]
	PT_RANGE = 4,            // [c1-c2]
	PT_ANY = 5,              // .
	// Unary
	PT_NON_TERMINAL = 6,     // <non-terminal> // Recurse to non-terminal expression
	PT_QUANTIFIER = 7,       // e^N // If positive, match N or more occurrences of `e`
	                                // If negative, match at most N occurrences of `e`
	                                // e^0  == e*
	                                // e^1  == e+
	                                // e^-1 == e?
	PT_AND = 8,              // &e
	PT_NOT = 9,              // !e
	// N-ary
	PT_SEQUENCE = 10,        // e1 e2
	PT_CHOICE = 11,          // e1 / e2
	// Custom match by function
	PT_CUSTOM_MATCHER = 12,  // int(const char *, void *) // Return how many characters were matched
	                                                      // Return non-positive values for no match to occur
	PT_ERROR = 13,           // ERROR // Represents a syntactic error
};

/// String version of the possible operations.
extern const char * const pt_operation_names[];

/// A function that receives a string and userdata and match it (positive) or not, advancing the matched number.
typedef int(*pt_custom_matcher_function)(const char *, void *);
/// A function that receives a character (int) and match it (non-zero) or not (0).
typedef int(*pt_character_class_function)(int);

/// Parsing Expressions.
typedef struct pt_expr {
	/// Expression data, depending on it's operation.
	union {
		/// Literals, Character Sets, Ranges and Non-Terminal names.
		const char *characters;
		/// Quantifier, And & Not: operand, Error sync Expression.
		struct pt_expr *e;
		/// N-ary operators: a N-array of operands.
		struct pt_expr **es;
		/// Custom match function.
		pt_custom_matcher_function matcher;
		/// Character class function.
		pt_character_class_function test_character_class;
	} data;
	pt_expression_action action;  ///< Action to be called when the whole match succeeds.
	int16_t N;  ///< Quantifier, array size for N-ary operations, Non-Terminal index, Literal length, Error code.
	uint8_t op;  ///< Operation to be performed.
	uint8_t own_memory : 1;  ///< Do Expression own the `characters`, `e` or `es` data buffer?
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
 * Create a Case Insensitive Expression.
 *
 * @param str            Literal string to be matched ignoring case.
 * @param own_characters Should Expression own the `characters` buffer?
 * @param action         Action associated to the Expression.
 */
pt_expr *pt_create_case_insensitive(const char *str, uint8_t own_characters, pt_expression_action action);
/**
 * Create a Character Class Expression.
 *
 * @param f      Custom function that returns true for characters in the class.
 * @param action Action associated to the Expression.
 */
pt_expr *pt_create_character_class(pt_character_class_function f, pt_expression_action action);
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
 * @param e              Expression.
 * @param N              Quantifier.
 * @param own_expression Should Expression own the `e` buffer?
 * @param action         Action associated to the Expression.
 */
pt_expr *pt_create_quantifier(pt_expr *e, int N, uint8_t own_expression, pt_expression_action action);
/**
 * Create an And Expression.
 *
 * @param e              Expression.
 * @param own_expression Should Expression own the `e` buffer?
 */
pt_expr *pt_create_and(pt_expr *e, uint8_t own_expression);
/**
 * Create a Not Expression.
 *
 * @param e              Expression.
 * @param own_expression Should Expression own the `e` buffer?
 */
pt_expr *pt_create_not(pt_expr *e, uint8_t own_expression);
/**
 * Create a Sequence Expression.
 *
 * @param es              `N` sized array of Expressions.
 * @param N               Array size.
 * @param own_expressions Should Expression own the `es` buffer?
 * @param action          Action associated to the Expression.
 */
pt_expr *pt_create_sequence(pt_expr **es, int N, uint8_t own_expressions, pt_expression_action action);
/**
 * Create a Choice Expression.
 *
 * @param es              `N` sized array of Expressions.
 * @param N               Array size.
 * @param own_expressions Should Expression own the `es` buffer?
 * @param action          Action associated to the Expression.
 */
pt_expr *pt_create_choice(pt_expr **es, int N, uint8_t own_expressions, pt_expression_action action);
/**
 * Create a Custom Matcher Expression.
 *
 * @param f      Custom Matcher function.
 * @param action Action associated to the Expression.
 */
pt_expr *pt_create_custom_matcher(pt_custom_matcher_function f, pt_expression_action action);
/**
 * Create an Error Expression.
 *
 * @warning As syncronization is done using a combination of a Quantifier and
 *          the syncronization Expression, it __must not__ accept empty string.
 *          This is checked for in #pt_validate_grammar.
 *
 * @param code           Numeric error code.
 * @param sync           Syncronization Expression, may be `NULL` if no syncing is wanted.
 * @param own_expression Should Expression own the `e` buffer?
 */
pt_expr *pt_create_error(int code, pt_expr *sync, uint8_t own_expression);

/**
 * Destroy an Expression, freeing the memory used.
 *
 * It is safe to pass a `NULL` pointer here.
 *
 * @warning The passed pointer itself will also be freed, as Expressions are
 *          supposed to be created by the `pt_create_*` functions.
 *
 * @param e Expression to be destroyed.
 */
void pt_destroy_expr(pt_expr *e);

#ifdef __cplusplus
}
#endif

#endif

