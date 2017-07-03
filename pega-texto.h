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

#ifndef __PEGA_TEXTO_H__
#define __PEGA_TEXTO_H__

#include <stdlib.h>
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
	PT_NON_TERMINAL_IDX, // <non-terminal> // Recurse by index in the grammar
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
struct pt_expr_t {
	uint8_t op;  // Operation to be performed
	uint16_t N;  // Quantifier, or array size for N-ary operations
	union {
		// Literals, Character Sets and Ranges.
		// @warning: pt_expr DO NOT own the memory for char buffers
		char *characters;
		// Non-terminal index in the grammar
		int index;
		// Quantifier, And & Not operand
		pt_expr *e;
		// N-ary operators: a N-array of operands
		pt_expr **es;
		// Custom match function
		int (*matcher)(int);
	} data;
};

pt_expr *pt_create_literal(char *str);
pt_expr *pt_create_set(char *str);
pt_expr *pt_create_range(char *str);
pt_expr *pt_create_any();
pt_expr *pt_create_non_terminal(pt_expr *e);
pt_expr *pt_create_non_terminal_idx(int index);
pt_expr *pt_create_quantifier(pt_expr *e, int N);
pt_expr *pt_create_and(pt_expr *e);
pt_expr *pt_create_not(pt_expr *e);
pt_expr *pt_create_sequence(pt_expr **es, int N);
pt_expr *pt_create_choice(pt_expr **es, int N);
pt_expr *pt_create_custom_matcher(pt_custom_matcher f);

void pt_destroy_expr(pt_expr *e);

/// Easy to use macros for building PEGs
#ifdef PEGA_TEXTO_MACROS
pt_expr *pt__from_nt_array(pt_expr *(*f)(pt_expr **, int), pt_expr **nt_exprs);
# define ARRAY(...) ((pt_expr*[]){__VA_ARGS__})
# define L(s)     (pt_create_literal(s))
# define S(s)     (pt_create_set(s))
# define R(s)     (pt_create_range(s))
# define Any      (pt_create_any())
# define V(e)     (pt_create_non_terminal(e))
# define Vi(i)    (pt_create_non_terminal_idx(i))
# define Q(e, N)  (pt_create_quantifier(e, N))
# define And(e)   (pt_create_and(e))
# define Not(e)   (pt_create_not(e))
# define Seq(...) (pt__from_nt_array(&pt_create_sequence, ARRAY(__VA_ARGS__, NULL)))
# define Or(...)  (pt__from_nt_array(&pt_create_choice, ARRAY(__VA_ARGS__, NULL)))
# define F(f)     (pt_create_custom_matcher(f))
#endif

#endif

