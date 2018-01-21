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
 * The `*_O` variants own the `characters` string buffer, so #pt_destroy_expr
 * frees it.
 *
 * The `*_NO` variants don't own the `e` Expression or `es` Expression array
 * buffers, so #pt_destroy_expr don't free them.
 *
 * The `*_` variants set the action to be performed on a match
 * (see @ref pt_expression_action).
 */

#ifndef NULL
#include <stdlib.h>
#endif

/// Create a Literal Expression.
#define L(s)            (pt_create_literal(s, 0, NULL))
#define L_(a, s)        (pt_create_literal(s, 0, a))
#define L_O(s)          (pt_create_literal(s, 1, NULL))
#define L_O_(a, s)      (pt_create_literal(s, 1, a))
/// Create a Set Expression.
#define S(s)            (pt_create_set(s, 0, NULL))
#define S_(a, s)        (pt_create_set(s, 0, a))
#define S_O(s)          (pt_create_set(s, 1, NULL))
#define S_O_(a, s)      (pt_create_set(s, 1, a))
/// Create a Range Expression.
#define R(s)            (pt_create_range(s, 0, NULL))
#define R_(a, s)        (pt_create_range(s, 0, a))
#define R_O(s)          (pt_create_range(s, 1, NULL))
#define R_O_(a, s)      (pt_create_range(s, 1, a))
/// Create a Any Expression.
#define ANY             (pt_create_any(NULL))
#define ANY_(a)         (pt_create_any(a))
/// Create a Non-terminal Expression, indexing it by the Rule name.
#define V(r)            (pt_create_non_terminal(r, 0, NULL))
#define V_(a, r)        (pt_create_non_terminal(r, 0, a))
#define V_O(r)          (pt_create_non_terminal(r, 1, NULL))
#define V_O_(a, r)      (pt_create_non_terminal(r, 1, a))
/// Create a Non-terminal Expression, indexing it by the Rule numerical index.
#define VI(i)           (pt_create_non_terminal_idx(i, NULL))
#define VI_(a, i)       (pt_create_non_terminal_idx(i, a))
/// Create a Quantifier Expression.
#define Q(e, N)         (pt_create_quantifier(e, N, 1, NULL))
#define Q_(a, e, N)     (pt_create_quantifier(e, N, 1, a))
#define Q_NO(e, N)      (pt_create_quantifier(e, N, 0, NULL))
#define Q_NO_(a, e, N)  (pt_create_quantifier(e, N, 0, a))
/// Create an And Expression.
#define AND(e)          (pt_create_and(e, 1))
#define AND_NO(e)       (pt_create_and(e, 0))
/// Create a Not Expression.
#define NOT(e)          (pt_create_not(e, 1))
#define NOT_NO(e)       (pt_create_not(e, 0))
/// Create a Sequence Expression.
/// You can create an empty Sequence with `SEQ(NULL)`.
#define SEQ(...)        (pt__from_nt_array(&pt_create_sequence, (pt_expr **)((pt_expr*[]){__VA_ARGS__, NULL}), 1, NULL))
#define SEQ_(a, ...)    (pt__from_nt_array(&pt_create_sequence, (pt_expr **)((pt_expr*[]){__VA_ARGS__, NULL}), 1, a))
#define SEQ_NO(...)     (pt__from_nt_array(&pt_create_sequence, (pt_expr **)((pt_expr*[]){__VA_ARGS__, NULL}), 0, NULL))
#define SEQ_NO_(a, ...) (pt__from_nt_array(&pt_create_sequence, (pt_expr **)((pt_expr*[]){__VA_ARGS__, NULL}), 0, a))
/// Create a Choice Expression.
/// You can create an empty Choice with `OR(NULL)`.
#define OR(...)         (pt__from_nt_array(&pt_create_choice,   (pt_expr **)((pt_expr*[]){__VA_ARGS__, NULL}), 1, NULL))
#define OR_(a, ...)     (pt__from_nt_array(&pt_create_choice,   (pt_expr **)((pt_expr*[]){__VA_ARGS__, NULL}), 1, a))
#define OR_NO(...)      (pt__from_nt_array(&pt_create_choice,   (pt_expr **)((pt_expr*[]){__VA_ARGS__, NULL}), 0, NULL))
#define OR_NO_(a, ...)  (pt__from_nt_array(&pt_create_choice,   (pt_expr **)((pt_expr*[]){__VA_ARGS__, NULL}), 0, a))
/// Create a Custom Matcher Expression.
#define F(f)            (pt_create_custom_matcher(f, NULL))
#define F_(a, f)        (pt_create_custom_matcher(f, a))
/// Create an Error Expression.
#define E(c, s)         (pt_create_error(c, s, 1))
#define E_NO(c, s)      (pt_create_error(c, s, 0))

/* These are complex Expressions that are widely used.
 * As macros, they're cost-free abstractions, so cool!
 */

/// Create a `!e .` (Any but `e`) Expression
#define BUT(e)          (SEQ(NOT(e), ANY))
#define BUT_(a, e)      (SEQ_(a, NOT(e), ANY))
#define BUT_NO(e)       (SEQ(NOT_NO(e), ANY))
#define BUT_NO_(a, e)   (SEQ_(a, NOT_NO(e), ANY))

