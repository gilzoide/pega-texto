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

/** @file compiler.h
 * Compiler for compiling pega-texto textual Grammar representation.
 */

#ifndef __PEGA_TEXTO_COMPILER_H__
#define __PEGA_TEXTO_COMPILER_H__

#include "compiler_cli_args.h"
#include "compiler_rule.h"
#include "bytecode.h"
#include "table.h"

#include <pega-texto.h>

typedef struct pt_compiler {
    pt_grammar compiler_grammar;
    pt_grammar target_grammar;
    pt_bytecode bytecode;
    pt_table rule_table;
} pt_compiler;

int pt_init_compiler(pt_compiler *compiler);
void pt_release_compiler(pt_compiler *compiler);

int pt_try_compile(pt_compiler *compiler, const char *grammar_description, pt_compiler_args *compiler_args);

pt_rule_info *pt_get_rule_info(pt_compiler *compiler, const char *rule_name, int length);

#endif
