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

#ifndef __PEGA_TEXTO_EXPR_DEF_H__
#define __PEGA_TEXTO_EXPR_DEF_H__

struct pt_expr_t {
	union {
		// Literals, Character Sets, Ranges and Non-Terminal names.
		// @warning: pt_expr DO NOT own the memory for char buffers
		const char *characters;
		// Quantifier, And & Not operand
		pt_expr *e;
		// N-ary operators: a N-array of operands
		pt_expr **es;
		// Custom match function
		int (*matcher)(int);
	} data;
	int16_t N;  // Quantifier, array size for N-ary operations, Non-Terminal index or Literal length
	uint8_t op;  // Operation to be performed
	uint8_t own_characters : 1;  // Do Expression own te characters buffer?
};

#endif

