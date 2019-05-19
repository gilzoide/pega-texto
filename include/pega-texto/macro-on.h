/*
 * Copyright 2017-2019 Gil Barbosa Reis <gilzoide@gmail.com>
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

#ifndef __PEGA_TEXTO_MACRO_ON_H__
#define __PEGA_TEXTO_MACRO_ON_H__

#ifdef __cplusplus
# include <initializer_list>
# include <cstring>
# define __PT_EXPR_FROM_ARRAY_PARAM std::initializer_list<pt_expr *> l
# define __PT_EXPR_FROM_ARRAY_CAST(...) {__VA_ARGS__}
# define __PT_EXPR_FROM_ARRAY_GET_N \
	N = l.size() == 1 && *l.begin() == NULL ? 0 : l.size(); \
	pt_expr *const *nt_exprs = l.begin()
#else
# include <string.h>
# define __PT_EXPR_FROM_ARRAY_PARAM pt_expr **nt_exprs
# define __PT_EXPR_FROM_ARRAY_CAST(...) ((pt_expr*[]){__VA_ARGS__, NULL})
# define __PT_EXPR_FROM_ARRAY_GET_N \
	for(aux = nt_exprs; *aux; aux++); \
	N = aux - nt_exprs
#endif
/**
 * Utility function to create Sequence and Choice Expressions from an array.
 *
 * C and C++ treat arrays differently, so there is one initializer style for each.
 */
static pt_expr *__pt_expr_from_array(pt_expr *(*f)(pt_expr **, int, uint8_t, pt_expression_action),
                                     __PT_EXPR_FROM_ARRAY_PARAM,
                                     uint8_t own_expressions,
                                     pt_expression_action action) {
	int N, byte_size;
	pt_expr **aux;
	__PT_EXPR_FROM_ARRAY_GET_N;
	byte_size = N * sizeof(pt_expr *);
	if(N == 0) {
		return f(NULL, N, own_expressions, action);
	}
	else if(aux = (pt_expr **) malloc(byte_size)) {
		return f((pt_expr **) memcpy(aux, nt_exprs, byte_size), N, own_expressions, action);
	}
	else {
		return NULL;
	}
}
#endif  // __PEGA_TEXTO_MACRO_ON_H__

/// Create a Byte Expression.
#define B(b)            (pt_create_byte(b, NULL))
#define B_(a, b)        (pt_create_byte(b, a))
/// Create a Literal Expression.
#define L(s)            (pt_create_literal(s, 0, NULL))
#define L_(a, s)        (pt_create_literal(s, 0, a))
#define L_O(s)          (pt_create_literal(s, 1, NULL))
#define L_O_(a, s)      (pt_create_literal(s, 1, a))
/// Create a Case Insensitive Expression.
#define I(s)            (pt_create_case_insensitive(s, 0, NULL))
#define I_(a, s)        (pt_create_case_insensitive(s, 0, a))
#define I_O(s)          (pt_create_case_insensitive(s, 1, NULL))
#define I_O_(a, s)      (pt_create_case_insensitive(s, 1, a))
/// Create a Character Class Expression.
#define C(c)            (pt_create_character_class(c, NULL))
#define C_(a, c)        (pt_create_character_class(c, a))
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
/// Create a Custom Matcher Expression.
#define F(f)            (pt_create_custom_matcher(f, NULL))
#define F_(a, f)        (pt_create_custom_matcher(f, a))
/// Create an Error Expression.
#define E(c, s)         (pt_create_error(c, s, 1))
#define E_NO(c, s)      (pt_create_error(c, s, 0))


// Sequence and Choice Expressions.

/// Create a Sequence Expression.
/// You can create an empty Sequence with `SEQ(NULL)`.
#define SEQ(...)        (__pt_expr_from_array(&pt_create_sequence, __PT_EXPR_FROM_ARRAY_CAST(__VA_ARGS__), 1, NULL))
#define SEQ_(a, ...)    (__pt_expr_from_array(&pt_create_sequence, __PT_EXPR_FROM_ARRAY_CAST(__VA_ARGS__), 1, a))
#define SEQ_NO(...)     (__pt_expr_from_array(&pt_create_sequence, __PT_EXPR_FROM_ARRAY_CAST(__VA_ARGS__), 0, NULL))
#define SEQ_NO_(a, ...) (__pt_expr_from_array(&pt_create_sequence, __PT_EXPR_FROM_ARRAY_CAST(__VA_ARGS__), 0, a))
/// Create a Choice Expression.
/// You can create an empty Choice with `OR(NULL)`.
#define OR(...)         (__pt_expr_from_array(&pt_create_choice,   __PT_EXPR_FROM_ARRAY_CAST(__VA_ARGS__), 1, NULL))
#define OR_(a, ...)     (__pt_expr_from_array(&pt_create_choice,   __PT_EXPR_FROM_ARRAY_CAST(__VA_ARGS__), 1, a))
#define OR_NO(...)      (__pt_expr_from_array(&pt_create_choice,   __PT_EXPR_FROM_ARRAY_CAST(__VA_ARGS__), 0, NULL))
#define OR_NO_(a, ...)  (__pt_expr_from_array(&pt_create_choice,   __PT_EXPR_FROM_ARRAY_CAST(__VA_ARGS__), 0, a))

/* These are complex Expressions that are widely used.
 * As macros, they're cost-free abstractions, so cool!
 */

/// Create a `!e .` (Any but `e`) Expression
#define BUT(e)          (SEQ(NOT(e), ANY))
#define BUT_(a, e)      (SEQ_(a, NOT(e), ANY))
#define BUT_NO(e)       (SEQ(NOT_NO(e), ANY))
#define BUT_NO_(a, e)   (SEQ_(a, NOT_NO(e), ANY))

