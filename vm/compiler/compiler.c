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

#include "logging.h"

#include <errno.h>
#include <stdio.h>

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
            pt_log(PT_LOG_ERROR, "[pt_grammar_validate] Error on rule \"%s\"",
                   target_grammar->names[validation_result.rule]);
            pt_log(PT_LOG_ERROR, ": %s\n", pt_validate_codes_description[validation_result.status]);
            return 0;
        }
        else return 1;
    }
    else return 0;
}

// Forward declaration
int pt_compile_expr(pt_bytecode *bytecode, pt_expr *expr, pt_bytecode_address **failure_patch_address);

int pt_compile_byte(pt_bytecode *bytecode, pt_expr *expr) {
    return pt_push_bytes(bytecode, 2, BYTE, expr->N) != NULL;
}

int pt_compile_literal(pt_bytecode *bytecode, pt_expr *expr) {
    return pt_push_byte(bytecode, STRING) 
           && pt_push_byte_array(bytecode, expr->N + 1, (const uint8_t *)expr->data.characters);
}

int pt_compile_character_class(pt_bytecode *bytecode, pt_expr *expr) {
    return pt_push_bytes(bytecode, 2, CLASS, expr->N) != NULL;
}

int pt_compile_set(pt_bytecode *bytecode, pt_expr *expr) {
    return pt_push_byte(bytecode, SET) 
           && pt_push_byte_array(bytecode, expr->N + 1, (const uint8_t *)expr->data.characters);
}

int pt_compile_range(pt_bytecode *bytecode, pt_expr *expr) {
    return pt_push_bytes(bytecode, 3, RANGE, ((uint8_t *)&expr->N)[0], ((uint8_t *)&expr->N)[1]) != NULL;
}

int pt_compile_any(pt_bytecode *bytecode, pt_expr *expr) {
    return pt_push_bytes(bytecode, 2, NOT_BYTE, 0) != NULL;
}

int pt_compile_optional(pt_bytecode *bytecode,  pt_expr *expr) {
    pt_push_byte(bytecode, PUSH);
    pt_bytecode_address *failure_patch_address = NULL;
    pt_compile_expr(bytecode, expr, &failure_patch_address);
    pt_push_byte(bytecode, JUMP);
    pt_bytecode_address *success_patch_address = (pt_bytecode_address *)pt_reserve_bytes(bytecode, sizeof(pt_bytecode_address));
    if(failure_patch_address) {
        pt_patch_address(failure_patch_address, pt_current_address(bytecode));
    }
    pt_push_byte(bytecode, PEEK);
    if(success_patch_address) {
        pt_patch_address(success_patch_address, pt_current_address(bytecode));
    }
    pt_push_byte(bytecode, POP);
    pt_push_byte(bytecode, SUCCEED);
    return 1;
}

int pt_compile_zero_or_more(pt_bytecode *bytecode,  pt_expr *expr) {
    pt_bytecode_address expression_address = pt_current_address(bytecode);
    pt_bytecode_address *failure_patch_address = NULL;
    pt_compile_expr(bytecode, expr, &failure_patch_address);
    pt_push_byte(bytecode, JUMP);
    pt_push_address(bytecode, expression_address);
    if(failure_patch_address) {
        pt_patch_address(failure_patch_address, pt_current_address(bytecode));
    }
    pt_push_byte(bytecode, SUCCEED);
    return 1;
}

int pt_compile_general_quantifier(pt_bytecode *bytecode,  pt_expr *expr) {
    pt_push_byte(bytecode, PUSH);
    pt_push_byte(bytecode, QC_ZERO);
    pt_bytecode_address expression_address = pt_current_address(bytecode);
    pt_bytecode_address *fail_patch_address = NULL;
    pt_compile_expr(bytecode, expr->data.e, &fail_patch_address);
    pt_push_byte(bytecode, QC_INC);
    pt_push_byte(bytecode, JUMP);
    pt_push_address(bytecode, expression_address);
    if(fail_patch_address) {
        pt_patch_address(fail_patch_address, pt_current_address(bytecode));
    }
    pt_push_bytes(bytecode, 2, FAIL_LESS_THEN, expr->N);
    pt_push_byte(bytecode, POP);
    return 1;
}

int pt_compile_quantifier(pt_bytecode *bytecode, pt_expr *expr) {
    pt_expr *subexpr = expr->data.e;
    switch (expr->N) {
        case -1: return pt_compile_optional(bytecode, subexpr);
        case 0:  return pt_compile_zero_or_more(bytecode, subexpr);
        default: return pt_compile_general_quantifier(bytecode, expr);
    }
}

int pt_compile_and(pt_bytecode *bytecode, pt_expr *expr) {
    pt_push_byte(bytecode, PUSH);
    pt_bytecode_address *continue_patch_address = NULL;
    pt_compile_expr(bytecode, expr->data.e, &continue_patch_address);
    if(continue_patch_address) {
        pt_patch_address(continue_patch_address, pt_current_address(bytecode));
    }
    pt_push_byte(bytecode, PEEK);
    pt_push_byte(bytecode, POP);
    return 1;
}

int pt_compile_expr(pt_bytecode *bytecode, pt_expr *expr, pt_bytecode_address **failure_patch_address) {
    switch (expr->op) {
        case PT_BYTE: pt_compile_byte(bytecode, expr); break;
        case PT_LITERAL: pt_compile_literal(bytecode, expr); break;
        case PT_CHARACTER_CLASS: pt_compile_character_class(bytecode, expr); break;
        case PT_SET: pt_compile_set(bytecode, expr); break;
        case PT_RANGE: pt_compile_range(bytecode, expr); break;
        case PT_ANY: pt_compile_any(bytecode, expr); break;
        case PT_QUANTIFIER: pt_compile_quantifier(bytecode, expr); break;
        case PT_AND: pt_compile_and(bytecode, expr); break;
        
        
        case PT_NON_TERMINAL:
        default:
            pt_log(PT_LOG_WARNING, "Expression operation not implemented yet: %s",
                            pt_opcode_description[expr->op]);
            return 0;
    }
    if(failure_patch_address) {
        pt_push_byte(bytecode, JUMP_IF_FAIL);
        *failure_patch_address = (pt_bytecode_address *)pt_push_address(bytecode, -1);
    }
    return 1;
}

int pt_compile_grammar(pt_compiler *compiler, pt_grammar *grammar) {
    int N = grammar->N;
    pt_bytecode *bytecode = &compiler->bytecode;
    int rule_address[N];
    int i;
    for(i = 0; i < N; i++) {
        // pt_log(LOG_DEBUG, "Compiling %s", grammar->names[i]);
        // rule_address[i] = bytecode->chunk.size;
        pt_compile_expr(bytecode, grammar->es[i], NULL);
        pt_push_byte(bytecode, RET);
    }
    return 1;
}

int pt_try_compile(pt_compiler *compiler, const char *grammar_description, pt_compiler_args *compiler_args) {
    if(!pt_compiler_read_grammar(compiler, grammar_description)) {
        return -1;
    }
    pt_compile_grammar(compiler, &compiler->target_grammar);
    const char *filename = compiler_args->output_filename;
    FILE *outfile = filename ? fopen(filename, "wb") : stdout;
    if(!outfile) return errno;

    pt_bytecode_write_to_file(&compiler->bytecode, outfile);
    fclose(outfile);
    return 0;
}
