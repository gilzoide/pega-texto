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

/** @file grammar.h
 * Parsing Expression Grammars.
 */

#ifndef __PEGA_TEXTO_GRAMMAR_H__
#define __PEGA_TEXTO_GRAMMAR_H__

#include "expr.h"

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
typedef struct {
	const char **names;  ///< Rules' names.
	pt_expr **es;  ///< Rules' Expressions.
	uint16_t N;  ///< Number of Rules.
	uint8_t own_names : 1;  ///< Do Grammar own the names' buffers?
} pt_grammar;

/**
 * Create a Grammar from a NULL-terminated array of Rules.
 *
 * @note Grammar owns the Expressions, and will destroy them on
 *       `pt_destroy_grammar`.
 *
 * @param rules     Grammar Rules.
 * @param own_names Should Grammar own the names' buffers?
 * @return The created Grammar.
 */
pt_grammar *pt_create_grammar(pt_rule *rules, uint8_t own_names);
/**
 * Destroy a Grammar, freeing the memory used.
 *
 * @warning Grammar Expressions will all be destroyed as well as the `g` pointer
 *          itself, as Grammars are supposed to be created by a call to
 *          `pt_create_grammar`.
 *
 * @param g Grammar to be destroyed.
 */
void pt_destroy_grammar(pt_grammar *g);


typedef enum {
	PT_VALIDATE_SUCCESS = 0,
	PT_VALIDATE_NULL_STRING,
	PT_VALIDATE_RANGE_BUFFER,
	PT_VALIDATE_INVALID_RANGE,
	PT_VALIDATE_OUT_OF_BOUNDS,
	PT_VALIDATE_UNDEFINED_RULE,
} pt_validate_result;

/**
 * Validate a Grammar.
 *
 * This checks if a Grammar is well-formed, as described by Ford (2014), and if
 * Non-terminal indexes (either by name, or numerical index) exist and are
 * inbounds.
 *
 * @todo implement this =P
 *
 * @param g Grammar to be validated.
 * @return ?
 */
int pt_grammar_validate(pt_grammar *g);

#endif

