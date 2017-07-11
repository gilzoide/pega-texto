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

/** @file macro-on.h
 * Turn on Parsing Expression constructor macros
 */

#ifndef NULL
#include <stdlib.h>
#endif

/// Create a Literal Expression.
#define L(s)     (pt_create_literal(s, 0))
/// Create a Literal Expression, owning the string buffer.
#define L_O(s)   (pt_create_literal(s, 1))
/// Create a Set Expression.
#define S(s)     (pt_create_set(s, 0))
/// Create a Set Expression, owning the string buffer.
#define S_O(s)   (pt_create_set(s, 1))
/// Create a Range Expression.
#define R(s)     (pt_create_range(s, 0))
/// Create a Range Expression, owning the string buffer.
#define R_O(s)   (pt_create_range(s, 1))
/// Create a Any Expression.
#define ANY      (pt_create_any())
/// Create a Non-terminal Expression, indexing it by the Rule name.
#define V(r)     (pt_create_non_terminal(r, 0))
/// Create a Non-terminal Expression, indexing it by the Rule name, owning the string buffer.
#define V_O(r)   (pt_create_non_terminal(r, 1))
/// Create a Non-terminal Expression, indexing it by the Rule numerical index.
#define VI(i)    (pt_create_non_terminal_idx(i))
/// Create a Quantifier Expression.
#define Q(e, N)  (pt_create_quantifier(e, N))
/// Create an And Expression.
#define AND(e)   (pt_create_and(e))
/// Create a Not Expression.
#define NOT(e)   (pt_create_not(e))
/// Create a Capture Expression.
#define C(e)     (pt_create_capture(e))
/// Create a Sequence Expression.
#define SEQ(...) (pt__from_nt_array(&pt_create_sequence, ((pt_expr*[]){__VA_ARGS__, NULL})))
/// Create a Choice Expression.
#define OR(...)  (pt__from_nt_array(&pt_create_choice,   ((pt_expr*[]){__VA_ARGS__, NULL})))
/// Create a Custom Matcher Expression.
#define F(f)     (pt_create_custom_matcher(f))

