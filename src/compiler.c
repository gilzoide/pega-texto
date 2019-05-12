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

#include <pega-texto/compiler.h>
#include <pega-texto/bytecode.h>
#include <pega-texto/grammar.h>

#include <stdlib.h>
#include <assert.h>

const char * const pt_compile_status_description[] = {
	"PT_COMPILE_SUCCESS",
	"PT_COMPILE_NULL_GRAMMAR",
	"PT_COMPILE_EMPTY_GRAMMAR",
	"PT_COMPILE_NULL_POINTER",
	"PT_COMPILE_RANGE_BUFFER",
	"PT_COMPILE_INVALID_RANGE",
	"PT_COMPILE_OUT_OF_BOUNDS",
	"PT_COMPILE_UNDEFINED_RULE",
	"PT_COMPILE_LOOP_EMPTY_STRING",
	"PT_COMPILE_MEMORY_ERROR",
};
#ifdef static_assert
static_assert(sizeof(pt_compile_status_description) == PT_COMPILE_STATUS_ENUM_COUNT * sizeof(const char *),
              "Missing compile status descriptions");
#endif


///////////////////////////////////////////////////////////////////////////////
//  Expression compilers
///////////////////////////////////////////////////////////////////////////////
static int _pt_compile_literal(pt_bytecode *bytecode, pt_expr *literal) {
	int res = 1;
	const char *s = literal->data.characters;
	const char *s_end = s + literal->N;
	for(; res && s < s_end; s++) {
		res = pt_push_byte(bytecode, PT_OP_BYTE) && pt_push_byte(bytecode, *s);
	}
	return res ? PT_COMPILE_SUCCESS : PT_COMPILE_MEMORY_ERROR;
}

static int _pt_compile_set(pt_bytecode *bytecode, pt_expr *literal) {
	int res = pt_push_byte(bytecode, PT_OP_SET)
	       && pt_push_constant(bytecode, CONST_STRING(literal->data.characters));
	return res ? PT_COMPILE_SUCCESS : PT_COMPILE_MEMORY_ERROR;
}

static int _pt_compile_success(pt_bytecode *bytecode) {
	int res = pt_push_byte(bytecode, PT_OP_RETURN);
	return res ? PT_COMPILE_SUCCESS : PT_COMPILE_MEMORY_ERROR;
}

///////////////////////////////////////////////////////////////////////////////
//  Grammar compilers
///////////////////////////////////////////////////////////////////////////////
enum pt_compile_status pt_compile_grammar(pt_bytecode *bytecode, pt_grammar *g) {
	// TODO: tratar erros
	int i, result = PT_COMPILE_SUCCESS;
	for(i = 0; result == PT_COMPILE_SUCCESS && i < g->N; i++) {
		pt_expr *expr = g->es[i];
		int op = expr->op;
		switch(op) {
			case PT_LITERAL: result = _pt_compile_literal(bytecode, expr); break;
			case PT_SET: result = _pt_compile_set(bytecode, expr); break;
			default: break;
		}
	}
	if(result == PT_COMPILE_SUCCESS) {
		result = _pt_compile_success(bytecode);
	}
	return result;
}

