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

#include <pega-texto/expr.h>

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

const char * const pt_operation_names[] = {
	"PT_BYTE",
	"PT_LITERAL",
	"PT_CASE_INSENSITIVE",
	"PT_CHARACTER_CLASS",
	"PT_SET",
	"PT_RANGE",
	"PT_ANY",
	"PT_NON_TERMINAL",
	"PT_QUANTIFIER",
	"PT_AND",
	"PT_NOT",
	"PT_SEQUENCE",
	"PT_CHOICE",
	"PT_CUSTOM_MATCHER",
	"PT_ERROR",
};

#ifdef static_assert
static_assert(sizeof(pt_operation_names) == PT_OPERATION_ENUM_COUNT * sizeof(const char *),
              "Missing expression operation names");
#endif

pt_character_class_function pt_function_for_character_class(enum pt_character_class c) {
	switch(c) {
		case 'w': return isalnum;
		case 'a': return isalpha;
		case 'c': return iscntrl;
		case 'd': return isdigit;
		case 'g': return isgraph;
		case 'l': return islower;
		case 'p': return ispunct;
		case 's': return isspace;
		case 'u': return isupper;
		case 'x': return isxdigit;
		default: return NULL;
	}
}

#define NEW_EXPR(body)                       \
	pt_expr *new_expr;                       \
	if(new_expr = malloc(sizeof(pt_expr))) { \
	    body                                 \
	}                                        \
	return new_expr;

pt_expr *pt_create_byte(uint8_t b, pt_expression_action action) {
	NEW_EXPR(
		new_expr->op = PT_BYTE;
		new_expr->N = b;
		new_expr->action = action;
	)
}

pt_expr *pt_create_literal(const char *str, uint8_t own_characters, pt_expression_action action) {
	NEW_EXPR(
		new_expr->op = PT_LITERAL;
		new_expr->data.characters = str;
		new_expr->N = strlen(str);
		new_expr->own_memory = own_characters;
		new_expr->action = action;
	)
}

pt_expr *pt_create_case_insensitive(const char *str, uint8_t own_characters, pt_expression_action action) {
	NEW_EXPR(
		new_expr->op = PT_CASE_INSENSITIVE;
		new_expr->data.characters = str;
		new_expr->N = strlen(str);
		new_expr->own_memory = own_characters;
		new_expr->action = action;
	)
}

pt_expr *pt_create_character_class(enum pt_character_class c, pt_expression_action action) {
	int (*f)(int);
	if((f = pt_function_for_character_class(c)) == NULL) return NULL;
	NEW_EXPR(
		new_expr->op = PT_CHARACTER_CLASS;
		new_expr->data.test_character_class = f;
		new_expr->N = c;
		new_expr->action = action;
	)
}

pt_expr *pt_create_set(const char *str, uint8_t own_characters, pt_expression_action action) {
	NEW_EXPR(
		new_expr->op = PT_SET;
		new_expr->data.characters = str;
		new_expr->N = strlen(str);
		new_expr->own_memory = own_characters;
		new_expr->action = action;
	)
}

pt_expr *pt_create_range(uint8_t range_min, uint8_t range_max, pt_expression_action action) {
	NEW_EXPR(
		new_expr->op = PT_RANGE;
		uint8_t *range_ptr = (uint8_t *)&new_expr->N;
		range_ptr[0] = range_min;
		range_ptr[1] = range_max;
		new_expr->action = action;
	)
}

pt_expr *pt_create_any(pt_expression_action action) {
	NEW_EXPR(
		new_expr->op = PT_ANY;
		new_expr->action = action;
	)
}

pt_expr *pt_create_non_terminal(const char *rule, uint8_t own_characters, pt_expression_action action) {
	NEW_EXPR(
		new_expr->op = PT_NON_TERMINAL;
		new_expr->data.characters = rule;
		new_expr->own_memory = own_characters;
		new_expr->N = -1;
		new_expr->action = action;
	)
}

pt_expr *pt_create_non_terminal_idx(int index, pt_expression_action action) {
	NEW_EXPR(
		new_expr->op = PT_NON_TERMINAL;
		new_expr->data.characters = NULL;
		new_expr->N = index;
		new_expr->action = action;
	)
}

pt_expr *pt_create_quantifier(pt_expr *e, int N, uint8_t own_expression, pt_expression_action action) {
	NEW_EXPR(
		new_expr->op = PT_QUANTIFIER;
		new_expr->N = N;
		new_expr->data.e = e;
		new_expr->own_memory = own_expression;
		new_expr->action = action;
	)
}

pt_expr *pt_create_and(pt_expr *e, uint8_t own_expression) {
	NEW_EXPR(
		new_expr->op = PT_AND;
		new_expr->data.e = e;
		new_expr->own_memory = own_expression;
		new_expr->action = NULL;
	)
}

pt_expr *pt_create_not(pt_expr *e, uint8_t own_expression) {
	NEW_EXPR(
		new_expr->op = PT_NOT;
		new_expr->data.e = e;
		new_expr->own_memory = own_expression;
		new_expr->action = NULL;
	)
}

pt_expr *pt_create_sequence(pt_expr **es, int N, uint8_t own_expressions, pt_expression_action action) {
	NEW_EXPR(
		new_expr->op = PT_SEQUENCE;
		new_expr->N = N;
		new_expr->data.es = es;
		new_expr->own_memory = own_expressions;
		new_expr->action = action;
	)
}

pt_expr *pt_create_choice(pt_expr **es, int N, uint8_t own_expressions, pt_expression_action action) {
	NEW_EXPR(
		new_expr->op = PT_CHOICE;
		new_expr->N = N;
		new_expr->data.es = es;
		new_expr->own_memory = own_expressions;
		new_expr->action = action;
	)
}

pt_expr *pt_create_custom_matcher(pt_custom_matcher_function f, pt_expression_action action) {
	NEW_EXPR(
		new_expr->op = PT_CUSTOM_MATCHER;
		new_expr->data.matcher = f;
		new_expr->action = action;
	)
}

pt_expr *pt_create_error(int code, pt_expr *sync, uint8_t own_expression) {
	NEW_EXPR(
		new_expr->op = PT_ERROR;
		new_expr->N = code;
		new_expr->data.e = sync;
		new_expr->own_memory = own_expression;
		new_expr->action = NULL;
	)
}

void pt_destroy_expr(pt_expr *e) {
	if(e) {
		int i;
		switch(e->op) {
			case PT_NON_TERMINAL:
				if(!e->data.characters) {
					break;
				}
			case PT_LITERAL: case PT_CASE_INSENSITIVE: case PT_SET:
				if(e->own_memory) {
					free((void *) e->data.characters);
				}
				break;

			case PT_QUANTIFIER: case PT_AND: case PT_NOT: case PT_ERROR:
				if(e->own_memory) {
					pt_destroy_expr(e->data.e);
				}
				break;

			case PT_SEQUENCE: case PT_CHOICE:
				if(e->own_memory) {
					for(i = 0; i < e->N; i++) {
						pt_destroy_expr(e->data.es[i]);
					}
					free(e->data.es);
				}
				break;

			default: break;
		}
		free(e);
	}
}

