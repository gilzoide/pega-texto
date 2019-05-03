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

/** @file grammar.h
 * Parsing Expression Grammars.
 */

#ifndef __PEGA_TEXTO_GRAMMAR_H__
#define __PEGA_TEXTO_GRAMMAR_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "pega-texto/expr.h"

/**
 * A Rule for the Grammar: `Name <- Expr`.
 */
typedef struct {
	const char *name;  ///< Rule name.
	pt_expr *e;  ///< Rule Expression.
} pt_rule;

/**
 * The Grammar: Rule SOA.
 *
 * It is implemented as a SOA as the names are not used often, so there are less
 * cache miss.
 */
typedef struct pt_grammar {
	const char **names;  ///< Rules' names.
	pt_expr **es;  ///< Rules' Expressions.
	uint16_t N;  ///< Number of Rules.
	uint8_t own_names : 1;  ///< Do Grammar own the names' buffers?
} pt_grammar;

/**
 * Initialize a Grammar with a NULL-terminated array of Rules.
 *
 * @note It is not necessary to initialize `g` with any value.
 * @note Grammar owns the Expressions, and will destroy them on
 *       `pt_release_grammar`.
 *
 * @param g         Grammar being initialized.
 * @param rules     Grammar Rules.
 * @param own_names Should Grammar own the names' buffers?
 * @return 1 on success.
 * @return 0 if there was any memory error.
 */
int pt_init_grammar(pt_grammar *g, pt_rule *rules, uint8_t own_names);
/**
 * Release the memory associated with a Grammar, reinitializing it to zeros.
 *
 * It is safe to pass a `NULL` pointer here.
 *
 * @warning Grammar Expressions will all be destroyed as well as the `g` pointer
 *          itself, as Grammars are supposed to be created by a call to
 *          `pt_create_grammar`.
 *
 * @param g Grammar to be destroyed.
 */
void pt_release_grammar(pt_grammar *g);

#ifdef __cplusplus
}
#endif

#endif

