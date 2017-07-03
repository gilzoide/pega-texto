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

/// Operators for constructing Parsing Expressions
typedef enum {
	// Primary
	PT_LITERAL,      // " "
	PT_SET,          // [ ]
	PT_RANGE,        // [c1-c2]
	PT_ANY,          // .
	// Unary
	PT_OPTIONAL,     // e?
	PT_ZERO_OR_MORE, // e*
	PT_ONE_OR_MORE,  // e+
	PT_AND,          // &e
	PT_NOT,          // !e
	// N-ary
	PT_SEQUENCE,     // e1 e2
	PT_CHOICE,       // e1 / e2
} pt_operator;

/// Parsing Expressions
typedef struct pt_expr {
	pt_operator op;
	union {
		// Literals, Character Sets and Ranges
		char *characters;
		// Non-primary operators: may be a single expression or a
		// NULL-terminated array, depending on operation arity
		struct pt_expr *e;
	} data;
} pt_expr;

#endif
