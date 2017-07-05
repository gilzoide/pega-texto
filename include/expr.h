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

#ifndef __PEGA_TEXTO_EXPR_H__
#define __PEGA_TEXTO_EXPR_H__

#include <stdint.h>

/// Operators for constructing Parsing Expressions
enum {
	// Primary
	PT_LITERAL,          // "string"
	PT_SET,              // [chars]
	PT_RANGE,            // [c1-c2]
	PT_ANY,              // .
	// Unary
	PT_NON_TERMINAL,     // <non-terminal> // Recurse to non-terminal expression
	PT_QUANTIFIER,       // e{N} // If positive, match N or more occurrences of `e`
	                             // If negative, match at most N occurrences of `e`
	                             // e{0}  == e*
	                             // e{1}  == e+
	                             // e{-1} == e?
	PT_AND,              // &e
	PT_NOT,              // !e
	// N-ary
	PT_SEQUENCE,         // e1 e2
	PT_CHOICE,           // e1 / e2
	// Custom match by function
	PT_FUNCTION,         // function(c) // If return 0, match fails
	                                    // If return nonzero, match succeeds, advance 1
};

/// Custom matcher: a function that receives a character (int) and match it or not
typedef int(*pt_custom_matcher)(int);

/// Parsing Expressions
typedef struct pt_expr_t pt_expr;

pt_expr *pt_create_literal(const char *str, uint8_t own_characters);
pt_expr *pt_create_set(const char *str, uint8_t own_characters);
pt_expr *pt_create_range(const char *str, uint8_t own_characters);
pt_expr *pt_create_any();
pt_expr *pt_create_non_terminal(const char *rules, uint8_t own_characters);
pt_expr *pt_create_non_terminal_idx(int index);
pt_expr *pt_create_quantifier(pt_expr *e, int N);
pt_expr *pt_create_and(pt_expr *e);
pt_expr *pt_create_not(pt_expr *e);
pt_expr *pt_create_sequence(pt_expr **es, int N);
pt_expr *pt_create_choice(pt_expr **es, int N);
pt_expr *pt_create_custom_matcher(pt_custom_matcher f);

void pt_destroy_expr(pt_expr *e);

/// Function for creating Sequences and Choices from NULL-terminated arrays
pt_expr *pt__from_nt_array(pt_expr *(*f)(pt_expr **, int), pt_expr **nt_exprs);

#endif

