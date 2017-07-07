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

#ifndef __PEGA_TEXTO_MATCH_H__
#define __PEGA_TEXTO_MATCH_H__

#include "expr.h"
#include "grammar.h"

#include <stdlib.h>

typedef enum {
	PT_NO_STACK_MEM = -2,
	PT_NO_MATCH = -1,
	PT_MATCH_SUCCESS = 0,
} pt_match_result;

typedef struct {
	size_t *stack_size;
} pt_match_options;

pt_match_result pt_match(pt_expr **es, const char **names, const char *str, pt_match_options *opts);
pt_match_result pt_match_expr(pt_expr *e, const char *str, pt_match_options *opts);
pt_match_result pt_match_grammar(pt_grammar *g, const char *str, pt_match_options *opts);

#endif
