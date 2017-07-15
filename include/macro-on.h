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
 * Turn on Parsing Expression constructor macros.
 *
 * @todo TODO: find a better way for actions for SEQ and OR.
 */

#ifndef NULL
#include <stdlib.h>
#endif

/// Create a Literal Expression.
#define L(s)         (pt_create_literal(s, 0, NULL))
#define L_(s, a)     (pt_create_literal(s, 0, a))
/// Create a Literal Expression, owning the string buffer.
#define L_O(s)       (pt_create_literal(s, 1, NULL))
#define L_O_(s, a)   (pt_create_literal(s, 1, a))
/// Create a Set Expression.
#define S(s)         (pt_create_set(s, 0, NULL))
#define S_(s, a)     (pt_create_set(s, 0, a))
/// Create a Set Expression, owning the string buffer.
#define S_O(s)       (pt_create_set(s, 1, NULL))
#define S_O_(s, a)   (pt_create_set(s, 1, a))
/// Create a Range Expression.
#define R(s)         (pt_create_range(s, 0, NULL))
#define R_(s, a)     (pt_create_range(s, 0, a))
/// Create a Range Expression, owning the string buffer.
#define R_O(s)       (pt_create_range(s, 1, NULL))
#define R_O_(s, a)   (pt_create_range(s, 1, a))
/// Create a Any Expression.
#define ANY          (pt_create_any(NULL))
#define ANY_(a)      (pt_create_any(a))
/// Create a Non-terminal Expression, indexing it by the Rule name.
#define V(r)         (pt_create_non_terminal(r, 0, NULL))
#define V_(r, a)     (pt_create_non_terminal(r, 0, a))
/// Create a Non-terminal Expression, indexing it by the Rule name, owning the string buffer.
#define V_O(r)       (pt_create_non_terminal(r, 1, NULL))
#define V_O_(r, a)   (pt_create_non_terminal(r, 1, a))
/// Create a Non-terminal Expression, indexing it by the Rule numerical index.
#define VI(i)        (pt_create_non_terminal_idx(i, NULL))
#define VI_(i, a)    (pt_create_non_terminal_idx(i, a))
/// Create a Quantifier Expression.
#define Q(e, N)      (pt_create_quantifier(e, N, NULL))
#define Q_(e, N, a)  (pt_create_quantifier(e, N, a))
/// Create an And Expression.
#define AND(e)       (pt_create_and(e))
/// Create a Not Expression.
#define NOT(e)       (pt_create_not(e))
/// Create a Sequence Expression.
#define SEQ(...)     (pt__from_nt_array(&pt_create_sequence, ((pt_expr*[]){__VA_ARGS__, NULL}), NULL))
#define SEQ_(a, ...) (pt__from_nt_array(&pt_create_sequence, ((pt_expr*[]){__VA_ARGS__, NULL}), a))
/// Create a Choice Expression.
#define OR(...)      (pt__from_nt_array(&pt_create_choice,   ((pt_expr*[]){__VA_ARGS__, NULL}), NULL))
#define OR_(a, ...)  (pt__from_nt_array(&pt_create_choice,   ((pt_expr*[]){__VA_ARGS__, NULL}), a))
/// Create a Custom Matcher Expression.
#define F(f)         (pt_create_custom_matcher(f, NULL))
#define F_(f, a)     (pt_create_custom_matcher(f, a))

