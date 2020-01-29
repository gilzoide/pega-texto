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
#include "compiler_log.h"

int pt_init_compiler(pt_compiler *compiler) {
    return compiler 
           && pt_init_compiler_grammar(&compiler->compiler_grammar)
           && pt_init_bytecode(&compiler->bytecode)
           && (compiler->target_grammar = (pt_grammar){}, 1);
}

void pt_release_compiler(pt_compiler *compiler) {
    if(compiler) {
        pt_release_grammar(&compiler->compiler_grammar);
        pt_release_bytecode(&compiler->bytecode);
        pt_release_grammar(&compiler->target_grammar);
    }
}

int pt_compiler_read_grammar(pt_compiler *compiler, const char *grammar_description) {
    pt_grammar *target_grammar = &compiler->target_grammar;
    pt_match_options opts = (pt_match_options){ .userdata = target_grammar };
    pt_match_result result = pt_match_grammar(&compiler->compiler_grammar, grammar_description, &opts);
    if(result.matched >= 0) {
        pt_validate_result validation_result = pt_validate_grammar(target_grammar, PT_VALIDATE_DEFAULT);
        if(validation_result.status != PT_VALIDATE_SUCCESS) {
            pt_compiler_log(LOG_ERROR, "[pt_grammar_validate] Error on rule \"%s\"",
                            target_grammar->names[validation_result.rule]);
            pt_compiler_log(LOG_ERROR, ": %s\n", pt_validate_codes_description[validation_result.status]);
            return 0;
        }
        else return 1;
    }
    else return 0;
}

int pt_try_compile(pt_compiler *compiler, const char *grammar_description, pt_compiler_args *compiler_args) {
    if(!pt_compiler_read_grammar(compiler, grammar_description)) {
        return -1;
    }
    return 0;
}
