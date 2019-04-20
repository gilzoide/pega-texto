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

#include <stdlib.h>
#include <stddef.h>
#include <string.h>
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
	"PT_COMPILE_CONSTANTS_LIMIT",
};

#ifdef static_assert
static_assert(sizeof(pt_compile_status_description) == PT_COMPILE_STATUS_ENUM_COUNT * sizeof(const char *),
              "Missing descriptions");
#endif

// Avoid clearing the `constants` array
static inline void _pt_clear_bytecode(pt_bytecode *bytecode) {
	memset(bytecode, 0, offsetof(pt_bytecode, constants));
}

void pt_init_bytecode(pt_bytecode *bytecode) {
	_pt_clear_bytecode(bytecode);
}

void pt_release_bytecode(pt_bytecode *bytecode) {
	if(bytecode) {
		free(bytecode->chunk);
		_pt_clear_bytecode(bytecode);
	}
}

///////////////////////////////////////////////////////////////////////////////
//  Helpers
///////////////////////////////////////////////////////////////////////////////
static int _pt_push_byte(pt_bytecode *bytecode, uint8_t byte) {
	int chunk_size = bytecode->chunk_size;
	if(chunk_size >= bytecode->chunk_capacity) {
		int new_capacity = chunk_size ? bytecode->chunk_capacity * 2 : 8;
		uint8_t *chunk = realloc(bytecode->chunk, new_capacity * sizeof(uint8_t));
		if(!chunk) return PT_COMPILE_MEMORY_ERROR;
		bytecode->chunk = chunk;
		bytecode->chunk_capacity = new_capacity;
	}
	bytecode->chunk[chunk_size] = byte;
	bytecode->chunk_size = chunk_size + 1;
	return PT_COMPILE_SUCCESS;
}

static int _pt_push_constant(pt_bytecode *bytecode, pt_bytecode_constant constant) {
	int constants_size = bytecode->constants_size;
	if(constants_size >= PT_MAX_CONSTANTS) return PT_COMPILE_CONSTANTS_LIMIT;
	bytecode->constants[constants_size] = constant;
	bytecode->constants_size = constants_size + 1;
	return _pt_push_byte(bytecode, constants_size);
}

///////////////////////////////////////////////////////////////////////////////
//  Expression compilers
///////////////////////////////////////////////////////////////////////////////
static int _pt_compile_literal(pt_bytecode *bytecode, pt_expr *literal) {
	int res;
	if(res = _pt_push_byte(bytecode, PT_OP_LITERAL)) return res;
	return _pt_push_constant(bytecode, (pt_bytecode_constant){ .characters = literal->data.characters });
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
			default: break;
		}
	}
	return result;
}

