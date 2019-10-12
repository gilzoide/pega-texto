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

/**
 * Compiler first pass: linearize Grammar, validating it according to
 * [Ford (2014)](https://pdos.csail.mit.edu/~baford/packrat/popl04/peg-popl04.pdf).
 */

#include <pega-texto/expr.h>
#include <pega-texto/list.h>
#include <pega-texto/grammar.h>
#include <pega-texto/memory.h>
#include <pega-texto/compiler.h>

#include <stdlib.h>
#include <string.h>
#include <stdint.h>

// Auxiliary structure for keeping validations per Rule, to avoid Cycles between non-terminals.
typedef struct {
	uint8_t was_visited;
	uint8_t is_nullable_visited;
	uint8_t cannot_be_nullable;
} pt_validation_per_rule;

/// Look for a name in the array.
static int pt_safely_find_non_terminal_index(const char *name, const char **names, int N) {
	int i;
	for(i = 0; i < N; i++) {
		if(strcmp(name, names[i]) == 0) {
			return i;
		}
	}
	return -1;
}

// Forward declaration.
static int pt_validate_expr_in_grammar(pt_grammar *g, pt_expr *e, uint16_t *rule, pt_validation_per_rule *visited_rules);

/// Is an Expression Nullable (does it accept the empty string)?
static int pt_is_nullable(pt_grammar *g, pt_expr *e, uint16_t *rule, pt_validation_per_rule *visited_rules) {
	int i, res;
	uint16_t cur_rule = *rule;
	visited_rules[cur_rule].is_nullable_visited = 1;

	switch(e->op) {
		case PT_AND: case PT_NOT:
			return PT_COMPILE_LOOP_EMPTY_STRING;

		case PT_QUANTIFIER:
			return e->N <= 0 ? PT_COMPILE_LOOP_EMPTY_STRING : PT_COMPILE_SUCCESS;

		case PT_NON_TERMINAL:
			// Maybe non-terminal wasn't seen yet: validate it first (to correct this)
			if(e->N < 0) {
				if((res = pt_validate_expr_in_grammar(g, e, rule, visited_rules)) != PT_COMPILE_SUCCESS) {
					return res;
				}
			}
			// Only visit non-terminal if it hasn't yet been visited
			if(visited_rules[e->N].is_nullable_visited == 0) {
				*rule = e->N;
				if((res = pt_is_nullable(g, g->es[e->N], rule, visited_rules)) != PT_COMPILE_SUCCESS) {
					return res;
				}
				*rule = cur_rule;
			}
			break;

		case PT_SEQUENCE:
			for(i = 0; i < e->N; i++) {
				res = pt_is_nullable(g, e->data.es[i], rule, visited_rules);
				if(res == PT_COMPILE_SUCCESS || res != PT_COMPILE_LOOP_EMPTY_STRING) {
					return res;
				}
			}
			return PT_COMPILE_LOOP_EMPTY_STRING;
		
		case PT_CHOICE:
			for(i = 0; i < e->N; i++) {
				if((res = pt_is_nullable(g, e->data.es[i], rule, visited_rules)) != PT_COMPILE_SUCCESS) {
					return res;
				}
			}
			break;

		default: break;
	}
	return PT_COMPILE_SUCCESS;
}

/// Validates a single expression in the Grammar, to be called recursively.
static int pt_validate_expr_in_grammar(pt_grammar *g, pt_expr *e, uint16_t *rule, pt_validation_per_rule *visited_rules) {
	if(e == NULL) {
		return PT_COMPILE_NULL_POINTER;
	}

	int i, res;
	uint16_t cur_rule = *rule;
	visited_rules[cur_rule].was_visited = 1;

	switch(e->op) {
		case PT_LITERAL: case PT_CASE_INSENSITIVE: case PT_CHARACTER_CLASS: case PT_CUSTOM_MATCHER:
			if(e->data.characters == NULL) {
				return PT_COMPILE_NULL_POINTER;
			}
			break;

		case PT_RANGE: {}
			uint8_t *ptr = (uint8_t *)&e->N;
			if(ptr[0] > ptr[1]) {
				return PT_COMPILE_INVALID_RANGE;
			}
			break;

		case PT_NON_TERMINAL:
			if(e->N < 0) {
				if(e->data.characters == NULL) {
					return PT_COMPILE_NULL_POINTER;
				}
				else if((e->N = pt_safely_find_non_terminal_index(e->data.characters, g->names, g->N)) == -1) {
					return PT_COMPILE_UNDEFINED_RULE;
				}
			}
			else if(e->N >= g->N) {
				return PT_COMPILE_OUT_OF_BOUNDS;
			}
			// Only visit non-terminal if it hasn't yet been visited
			if(visited_rules[e->N].was_visited == 0) {
				*rule = e->N;
				if((res = pt_validate_expr_in_grammar(g, g->es[e->N], rule, visited_rules)) != PT_COMPILE_SUCCESS) {
					return res;
				}
				*rule = cur_rule;
			}
			break;

		case PT_QUANTIFIER:
			if(e->data.e == NULL) {
				return PT_COMPILE_NULL_POINTER;
			}
			else if((res = pt_validate_expr_in_grammar(g, e->data.e, rule, visited_rules)) != PT_COMPILE_SUCCESS) {
				return res;
			}
			else if(e->N == 0 && (res = pt_is_nullable(g, e->data.e, rule, visited_rules))) {
				return res;
			}
			break;

		case PT_AND: case PT_NOT:
			if(e->data.e == NULL) {
				return PT_COMPILE_NULL_POINTER;
			}
			else {
				return pt_validate_expr_in_grammar(g, e->data.e, rule, visited_rules);
			}

		case PT_SEQUENCE: case PT_CHOICE:
            // NULL pointer is valid if there are zero Expressions
			if(e->data.es == NULL && e->N != 0) {
				return PT_COMPILE_NULL_POINTER;
			}
			for(i = 0; i < e->N; i++) {
				if(e->data.es[i] == NULL) {
					return PT_COMPILE_NULL_POINTER;
				}
				else if((res = pt_validate_expr_in_grammar(g, e->data.es[i], rule, visited_rules)) != PT_COMPILE_SUCCESS) {
					return res;
				}
			}
			break;

		case PT_ERROR:
			if(e->data.e != NULL) {
				if((res = pt_validate_expr_in_grammar(g, e->data.e, rule, visited_rules)) != PT_COMPILE_SUCCESS) {
					return res;
				}
				else if(res = pt_is_nullable(g, e->data.e, rule, visited_rules)) {
					return res;
				}
			}
			break;
	}
	return PT_COMPILE_SUCCESS;
}

enum pt_compile_status pt_linearize_expr(pt_expr *e, pt_memory *memory) {
	enum pt_compile_status status;
	pt_expr *subexpr;
	switch(e->op) {
		case PT_QUANTIFIER: case PT_AND: case PT_NOT:
			subexpr = e->data.e;
			if(subexpr) {
				status = pt_linearize_expr(subexpr, memory);
			}
			else {
				return PT_COMPILE_NULL_POINTER;
			}
			break;

		case PT_SEQUENCE: case PT_CHOICE: {}
			int i;
			for(i = e->N - 1; i >= 0; i--) {
				subexpr = e->data.es[i];
				if(subexpr) {
					status = pt_linearize_expr(e->data.es[i], memory);
				}
				else {
					status = PT_COMPILE_NULL_POINTER;
				}
				if(status != PT_COMPILE_SUCCESS) return status;
			}
			break;

		default:
			break;
	}

	pt_expr **eptr = pt_memory_alloc_top(memory, sizeof(pt_expr *));
	if(!eptr) return PT_COMPILE_MEMORY_ERROR;
	*eptr = e;

	return PT_COMPILE_SUCCESS;
}

#include <stdio.h>
void soprinta(pt_expr *e) {
	printf("%p ", e);
	if(e == NULL) return;
	switch(e->op) {
		case PT_QUANTIFIER: case PT_AND: case PT_NOT:
			return soprinta(e->data.e);

		case PT_SEQUENCE: case PT_CHOICE: {}
			int i;
			for(i = 0; i < e->N; i++) {
				soprinta(e->data.es[i]);
			}
			break;
	}
}

/**
 * Linearize Grammar in top of `memory`, returning the compile status in `status`.
 */
pt_list_(pt_expr *) *pt_linearize_grammar(pt_grammar *g, enum pt_compile_status *status, pt_memory *memory) {
	if(g == NULL) {
		*status = PT_COMPILE_NULL_GRAMMAR;
	}
	else if(g->N == 0) {
		*status = PT_COMPILE_EMPTY_GRAMMAR;
	}
	else {
		/* pt_validation_per_rule visited_rules[g->N]; */
		/* memset(visited_rules, 0, g->N * sizeof(pt_validation_per_rule)); */
		/* uint16_t rule; */
		enum pt_compile_status localStatus = PT_COMPILE_SUCCESS;
		pt_memory_marker previous_top = pt_memory_get_top_marker(memory);
		int i;
		/* for(i = 0; i < g->N && localStatus == PT_COMPILE_SUCCESS; i++) { */
			/* soprinta(g->es[i]); */
		/* } */
		/* printf("\n"); */
		for(i = g->N - 1; i >= 0 && localStatus == PT_COMPILE_SUCCESS; i--) {
			localStatus = pt_linearize_expr(g->es[i], memory);
		}
		if(localStatus == PT_COMPILE_SUCCESS) {
			pt_memory_marker current_top = pt_memory_get_top_marker(memory);
			pt_list_(pt_expr *) *linearized_expressions = pt_memory_alloc_top(memory, sizeof(pt_list));
			if(linearized_expressions) {
				linearized_expressions->arr = memory->buffer + current_top + 1;
				linearized_expressions->size = (previous_top - current_top) / sizeof(pt_expr *);
				*status = localStatus;
			}
			else {
				*status  = PT_COMPILE_MEMORY_ERROR;
			}
			return linearized_expressions;
		}
	}
	return NULL;
}

