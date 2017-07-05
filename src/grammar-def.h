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

#ifndef __PEGA_TEXTO_GRAMMAR_DEF_H__
#define __PEGA_TEXTO_GRAMMAR_DEF_H__

#include "expr.h"

/// The Grammar: Rule SOA
struct pt_grammar_t {
	const char **names;  // The Rules' names
	pt_expr **es;  // The expressions
	int16_t N;  // Number or Rules
	uint8_t own_names : 1;  // Do Grammar own the names' buffers?
};

#endif

