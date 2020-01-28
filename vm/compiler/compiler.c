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

#include "compiler.h"
#include "compiler_grammar.h"

int pt_init_compiler(pt_compiler *compiler) {
    return compiler 
           && pt_init_compiler_grammar(&compiler->compiler_grammar)
           && (compiler->target_grammar = (pt_grammar){}, 1);
}

void pt_release_compiler(pt_compiler *compiler) {
    if(compiler) {
        pt_release_grammar(&compiler->compiler_grammar);
        pt_release_grammar(&compiler->target_grammar);
    }
}

int pt_compiler_read_grammar(pt_compiler *compiler, const char *grammar_description) {
    return 1;
}
