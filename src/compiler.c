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
#include <string.h>
#include <assert.h>

typedef struct pt_rule_compile_state {
	uint8_t visited;
	uint16_t address;
} pt_rule_compile_state;

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
enum pt_compile_status pt_compile_expr(pt_bytecode *bytecode, pt_grammar *g, pt_expr *expr, pt_rule_compile_state compile_state[]);

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

static int _pt_compile_non_terminal(pt_bytecode *bytecode, pt_grammar *g, pt_expr *expr, pt_rule_compile_state compile_state[]) {
	int N = expr->N;
	if(N < 0) {
		int i;
		const char *name = expr->data.characters, **names = g->names;
		for(i = 0; i < g->N; i++) {
			if(strcmp(name, names[i]) == 0) {
				expr->N = N = i;
				goto found_name;
			}
		}
		return PT_COMPILE_UNDEFINED_RULE;
	}
found_name: {}
	int result = pt_push_byte(bytecode, PT_OP_CALL) && pt_push_byte(bytecode, N);
	return result ? 2 : PT_COMPILE_MEMORY_ERROR;
}

static int _pt_compile_and(pt_bytecode *bytecode, pt_grammar *g, pt_expr *expr, pt_rule_compile_state compile_state[]) {
	pt_expr *subexpr = expr->data.e;
	if(subexpr == NULL) return PT_COMPILE_NULL_POINTER;
	int previous_size = bytecode->chunk.size;
	int res = pt_compile_expr(bytecode, g, subexpr, compile_state);
	if(res > 0) {
		uint8_t *instr = pt_byte_at(bytecode, previous_size);
		*instr ^= PT_OP_AND;
	}
	return res;
}

static int _pt_compile_not(pt_bytecode *bytecode, pt_grammar *g, pt_expr *expr, pt_rule_compile_state compile_state[]) {
	pt_expr *subexpr = expr->data.e;
	if(subexpr == NULL) return PT_COMPILE_NULL_POINTER;
	int previous_size = bytecode->chunk.size;
	int res = pt_compile_expr(bytecode, g, subexpr, compile_state);
	if(res > 0) {
		uint8_t *instr = pt_byte_at(bytecode, previous_size);
		*instr ^= PT_OP_NOT | PT_OP_AND;
	}
	return res;
}

static int _pt_compile_sequence(pt_bytecode *bytecode, pt_grammar *g, pt_expr *expr, pt_rule_compile_state compile_state[]) {
	int N = expr->N;
	if(N > 0 && expr->data.es == NULL) return PT_COMPILE_NULL_POINTER;
	int i, res = 0, previous_size = bytecode->chunk.size;
	for(i = 0; res >= 0 && i < N; i++) {
		res = pt_compile_expr(bytecode, g, expr->data.es[i], compile_state);
	}
	return res >= 0 ? bytecode->chunk.size - previous_size : res;
}

static int _pt_compile_choice(pt_bytecode *bytecode, pt_grammar *g, pt_expr *expr, pt_rule_compile_state compile_state[]) {
	int N = expr->N;
	if(N > 0 && expr->data.es == NULL) return PT_COMPILE_NULL_POINTER;
	if(!pt_reserve_bytes(bytecode, 3)) return PT_COMPILE_MEMORY_ERROR;
	int i, res, previous_size = bytecode->chunk.size - 3;
	for(i = 0; res >= 0 && i < N; i++) {
		res = pt_compile_expr(bytecode, g, expr->data.es[i], compile_state);
		if(res >= 0) res = pt_push_byte(bytecode, PT_OP_RETURN_ON_SUCCESS);
	}
	if(res >= 0) {
		if(!pt_push_byte(bytecode, PT_OP_POP_AND_FAIL)) return PT_COMPILE_MEMORY_ERROR;
		int current_size = bytecode->chunk.size;
		uint8_t *push_op_ptr = pt_byte_at(bytecode, previous_size);
		push_op_ptr[0] = PT_OP_PUSH_ADDRESS;
		*((uint16_t *)(push_op_ptr + 1)) = (uint16_t)current_size;
		return current_size - previous_size;
	}
	else return res;
}

///////////////////////////////////////////////////////////////////////////////
//  Grammar compilers
///////////////////////////////////////////////////////////////////////////////
enum pt_compile_status pt_compile_expr(pt_bytecode *bytecode, pt_grammar *g, pt_expr *expr, pt_rule_compile_state compile_state[]) {
	int op = expr->op;
	switch(op) {
		// Primary
		case PT_BYTE: return _pt_compile_byte(bytecode, expr);
		case PT_LITERAL: return _pt_compile_literal(bytecode, expr);
		case PT_CHARACTER_CLASS: return _pt_compile_char_class(bytecode, expr);
		case PT_SET: return _pt_compile_set(bytecode, expr);
		case PT_RANGE: return _pt_compile_range(bytecode, expr);
		case PT_ANY: return _pt_compile_any(bytecode, expr);
		// Unary
		case PT_NON_TERMINAL: return _pt_compile_non_terminal(bytecode, g, expr, compile_state);
		case PT_AND: return _pt_compile_and(bytecode, g, expr, compile_state);
		case PT_NOT: return _pt_compile_not(bytecode, g, expr, compile_state);
		// N-Ary
		case PT_SEQUENCE: return _pt_compile_sequence(bytecode, g, expr, compile_state);
		case PT_CHOICE: return _pt_compile_choice(bytecode, g, expr, compile_state);
		default: return PT_COMPILE_INVALID_EXPR;
	}
}
enum pt_compile_status pt_compile_grammar(pt_bytecode *bytecode, pt_grammar *g) {
	// TODO: tratar erros
	int i, result = 0, N = g->N;
	pt_rule_compile_state compile_state[N];
	memset(compile_state, 0, N);
	pt_list_push_n_as(&bytecode->rule_addresses, N, uint16_t);
	for(i = 0; result >= 0 && i < N; i++) {
		compile_state[i] = (pt_rule_compile_state){
			.visited = 1,
			.address = bytecode->chunk.size,
		};
		*pt_list_at(&bytecode->rule_addresses, i, uint16_t) = bytecode->chunk.size;
		pt_expr *expr = g->es[i];
		result = pt_compile_expr(bytecode, g, expr, compile_state) && pt_push_byte(bytecode, PT_OP_RETURN);
	}

	return result > 0 ? PT_COMPILE_SUCCESS : result;
}

