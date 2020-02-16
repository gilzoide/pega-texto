/*
 * Copyright 2017-2020 Gil Barbosa Reis <gilzoide@gmail.com>
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

/** @file compiler_grammar.h
 * Parser for pega-texto textual Grammar representation.
 */

#ifndef __PEGA_TEXTO_COMPILER_GRAMMAR_H__
#define __PEGA_TEXTO_COMPILER_GRAMMAR_H__

typedef struct pt_grammar pt_grammar;

/// @note When matching this grammar, pass pt_compiler as userdata
int pt_init_compiler_grammar(pt_grammar *grammar);

#endif
