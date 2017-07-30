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

/** @file re.h
 * Regex-like syntax for creating Grammars.
 *
 * @todo Explain the syntax
 * @todo Explain Action call order
 * @todo Example
 * @todo Error handling
 */

#ifndef __PEGA_TEXTO_RE_H__
#define __PEGA_TEXTO_RE_H__

#include "pega-texto/grammar.h"

/**
 * Create a Grammar from a string, formated using a regex-like syntax.
 *
 * @param str Grammar definition string.
 * @param ... Expression Actions, in order the marked expressions are matched.
 * @return The created Grammar, or NULL on any errors.
 */
pt_grammar *pt_create_grammar_from_string(const char *str, ...);

#endif

