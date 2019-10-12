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

/** @file pega-texto.h
 * Main header file: include this and you're good to go!
 *
 * You may also want to include pega-texto/macro-on.h for turning the
 * Expression macros on, easing the writing of PEGs.
 */

#ifndef __PEGA_TEXTO_H__
#define __PEGA_TEXTO_H__

/// Pega-texto major version number
#define PT_VERSION_MAJOR 2
/// Pega-texto minor version number
#define PT_VERSION_MINOR 1
/// Pega-texto patch version number
#define PT_VERSION_PATCH 0
/// Pega-texto version string
#define PT_VERSION "2.1.0"

#include "pega-texto/action.h"
#include "pega-texto/data.h"
#include "pega-texto/expr.h"
#include "pega-texto/grammar.h"
#include "pega-texto/match.h"
#include "pega-texto/match-state.h"
#include "pega-texto/validate.h"

#endif

