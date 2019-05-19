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
	"PT_COMPILE_INVALID_RANGE",
	"PT_COMPILE_OUT_OF_BOUNDS",
	"PT_COMPILE_UNDEFINED_RULE",
	"PT_COMPILE_LOOP_EMPTY_STRING",
	"PT_COMPILE_MEMORY_ERROR",
	"PT_COMPILE_INVALID_EXPR",
};
#ifdef static_assert
static_assert(sizeof(pt_compile_status_description) == PT_COMPILE_STATUS_ENUM_COUNT * sizeof(const char *),
              "Missing compile status descriptions");
#endif

const char *pt_get_compile_status_description(int compile_status) {
	if(compile_status >= 0) return pt_compile_status_description[0];
	else {
		compile_status = -compile_status;
		if(compile_status < PT_COMPILE_STATUS_ENUM_COUNT) return pt_compile_status_description[compile_status];
		else return "PT_COMPILE_?";
	}
}


///////////////////////////////////////////////////////////////////////////////
//  Expression compilers
///////////////////////////////////////////////////////////////////////////////
enum pt_compile_status pt_compile_expr(pt_bytecode *bytecode, pt_expr *expr);

static int _pt_compile_byte(pt_bytecode *bytecode, pt_expr *expr) {
	int res = pt_push_byte(bytecode, PT_OP_BYTE)
	          && pt_push_byte(bytecode, expr->N);
	return res ? 2 : PT_COMPILE_MEMORY_ERROR;
}

static int _pt_compile_literal(pt_bytecode *bytecode, pt_expr *expr) {
	int literal_size = expr->N;
	if(literal_size <= 0) return 0;
	else if(expr->data.characters == NULL) return PT_COMPILE_NULL_POINTER;
	int res = pt_push_byte(bytecode, PT_OP_STRING)
	          && pt_push_bytes(bytecode, literal_size + 1, (const uint8_t *)expr->data.characters);
	return res ? literal_size + 2 : PT_COMPILE_MEMORY_ERROR;
}

static int _pt_compile_set(pt_bytecode *bytecode, pt_expr *expr) {
	int set_size = expr->N;
	if(set_size <= 0) return 0;
	else if(expr->data.characters == NULL) return PT_COMPILE_NULL_POINTER;
	int res = pt_push_byte(bytecode, PT_OP_SET)
	          && pt_push_bytes(bytecode, set_size + 1, (const uint8_t *)expr->data.characters);
	return res ? 2 : PT_COMPILE_MEMORY_ERROR;
}

static int _pt_compile_char_class(pt_bytecode *bytecode, pt_expr *expr) {
	int res = pt_push_byte(bytecode, PT_OP_CHAR_CLASS)
	          && pt_push_byte(bytecode, expr->N);
	return res ? 2 : PT_COMPILE_MEMORY_ERROR;
}

static int _pt_compile_range(pt_bytecode *bytecode, pt_expr *expr) {
	const uint8_t *range = (const uint8_t *)&expr->N;
	if(range[0] > range[1]) return PT_COMPILE_INVALID_RANGE;
	int res = pt_push_byte(bytecode, PT_OP_RANGE)
	          && pt_push_bytes(bytecode, 2, range);
	return res ? 3 : PT_COMPILE_MEMORY_ERROR;
}

static int _pt_compile_any(pt_bytecode *bytecode, pt_expr *expr) {
	int res = pt_push_byte(bytecode, PT_OP_BYTE | PT_OP_NOT)
	          && pt_push_byte(bytecode, '\0');
	return res ? 2 : PT_COMPILE_MEMORY_ERROR;
}

static int _pt_compile_sequence(pt_bytecode *bytecode, pt_expr *expr) {
	int i, res = 0, acc = 0;
	if(expr->N > 0 && expr->data.es == NULL) return PT_COMPILE_NULL_POINTER;
	for(i = 0; res >= 0 && i < expr->N; i++) {
		res = pt_compile_expr(bytecode, expr->data.es[i]);
		if(res < 0) return res;
		acc += res;
	}
	return acc;
}

static int _pt_compile_and(pt_bytecode *bytecode, pt_expr *expr) {
	pt_expr *subexpr = expr->data.e;
	if(subexpr == NULL) return PT_COMPILE_NULL_POINTER;
	int res = pt_compile_expr(bytecode, subexpr);
	if(res > 0) {
		uint8_t *instr = pt_byte_at(bytecode, -res);
		*instr ^= PT_OP_AND;
	}
	return res;
}

static int _pt_compile_not(pt_bytecode *bytecode, pt_expr *expr) {
	pt_expr *subexpr = expr->data.e;
	if(subexpr == NULL) return PT_COMPILE_NULL_POINTER;
	int res = pt_compile_expr(bytecode, subexpr);
	if(res > 0) {
		uint8_t *instr = pt_byte_at(bytecode, -res);
		*instr ^= PT_OP_NOT | PT_OP_AND;
	}
	return res;
}

static int _pt_compile_success(pt_bytecode *bytecode) {
	int res = pt_push_byte(bytecode, PT_OP_SUCCESS);
	return res ? 1 : PT_COMPILE_MEMORY_ERROR;
}

///////////////////////////////////////////////////////////////////////////////
//  Grammar compilers
///////////////////////////////////////////////////////////////////////////////
enum pt_compile_status pt_compile_expr(pt_bytecode *bytecode, pt_expr *expr) {
	int op = expr->op;
	switch(op) {
		// Primary
		case PT_BYTE: return _pt_compile_byte(bytecode, expr);
		case PT_LITERAL: return _pt_compile_literal(bytecode, expr);
		case PT_SET: return _pt_compile_set(bytecode, expr);
		case PT_CHARACTER_CLASS: return _pt_compile_char_class(bytecode, expr);
		case PT_RANGE: return _pt_compile_range(bytecode, expr);
		case PT_ANY: return _pt_compile_any(bytecode, expr);
		// Unary
		case PT_AND: return _pt_compile_and(bytecode, expr);
		case PT_NOT: return _pt_compile_not(bytecode, expr);
		// N-Ary
		case PT_SEQUENCE: return _pt_compile_sequence(bytecode, expr);
		default: return PT_COMPILE_INVALID_EXPR;
	}
}
enum pt_compile_status pt_compile_grammar(pt_bytecode *bytecode, pt_grammar *g) {
	// TODO: tratar erros
	int i, result = PT_COMPILE_SUCCESS;
	for(i = 0; result >= PT_COMPILE_SUCCESS && i < g->N; i++) {
		pt_expr *expr = g->es[i];
		result = pt_compile_expr(bytecode, expr);
	}
	if(result >= PT_COMPILE_SUCCESS) {
		result = _pt_compile_success(bytecode);
		result = PT_COMPILE_SUCCESS;
	}
	return result;
}

