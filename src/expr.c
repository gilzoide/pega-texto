/*
 * Copyright 2017 Gil Barbosa Reis <gilzoide@gmail.com>
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

#include <stdlib.h>
#include <assert.h>
#include <string.h>

const char *pt_operation_names[] = {
	"PT_LITERAL",
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
};

#define NEW_EXPR(body)                       \
	pt_expr *new_expr;                       \
	if(new_expr = malloc(sizeof(pt_expr))) { \
	    body                                 \
	}                                        \
	return new_expr;


pt_expr *pt_create_literal(const char *str, uint8_t own_characters, pt_expression_action action) {
	NEW_EXPR(
		new_expr->op = PT_LITERAL;
		new_expr->data.characters = str;
		new_expr->N = strlen(str);
		new_expr->own_characters = own_characters;
		new_expr->action = action;
	)
}

pt_expr *pt_create_set(const char *str, uint8_t own_characters, pt_expression_action action) {
	NEW_EXPR(
		new_expr->op = PT_SET;
		new_expr->data.characters = str;
		new_expr->own_characters = own_characters;
		new_expr->action = action;
	)
}

pt_expr *pt_create_range(const char *str, uint8_t own_characters, pt_expression_action action) {
	NEW_EXPR(
		new_expr->op = PT_RANGE;
		new_expr->data.characters = str;
		new_expr->own_characters = own_characters;
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
		new_expr->own_characters = own_characters;
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

pt_expr *pt_create_quantifier(pt_expr *e, int N, pt_expression_action action) {
	NEW_EXPR(
		new_expr->op = PT_QUANTIFIER;
		new_expr->N = N;
		new_expr->data.e = e;
		new_expr->action = action;
	)
}

pt_expr *pt_create_and(pt_expr *e) {
	NEW_EXPR(
		new_expr->op = PT_AND;
		new_expr->data.e = e;
		new_expr->action = NULL;
	)
}

pt_expr *pt_create_not(pt_expr *e) {
	NEW_EXPR(
		new_expr->op = PT_NOT;
		new_expr->data.e = e;
		new_expr->action = NULL;
	)
}

pt_expr *pt_create_sequence(pt_expr **es, int N, pt_expression_action action) {
	NEW_EXPR(
		new_expr->op = PT_SEQUENCE;
		new_expr->N = N;
		new_expr->data.es = es;
		new_expr->action = action;
	)
}

pt_expr *pt_create_choice(pt_expr **es, int N, pt_expression_action action) {
	NEW_EXPR(
		new_expr->op = PT_CHOICE;
		new_expr->N = N;
		new_expr->data.es = es;
		new_expr->action = action;
	)
}

pt_expr *pt_create_custom_matcher(pt_custom_matcher f, pt_expression_action action) {
	NEW_EXPR(
		new_expr->op = PT_CUSTOM_MATCHER;
		new_expr->data.matcher = f;
		new_expr->action = action;
	)
}

void pt_destroy_expr(pt_expr *e) {
	int i;
	switch(e->op) {
		case PT_NON_TERMINAL:
			if(!e->data.characters) break;
		case PT_LITERAL: case PT_SET: case PT_RANGE:
			if(e->own_characters) {
				free((void *) e->data.characters);
			}
			break;

		case PT_QUANTIFIER: case PT_AND: case PT_NOT:
			pt_destroy_expr(e->data.e);
			break;

		case PT_SEQUENCE: case PT_CHOICE:
			for(i = 0; i < e->N; i++) {
				pt_destroy_expr(e->data.es[i]);
			}
			free(e->data.es);
			break;

		default: break;
	}
	free(e);
}

pt_expr *pt__from_nt_array(pt_expr *(*f)(pt_expr **, int, pt_expression_action), pt_expr **nt_exprs, pt_expression_action action) {
	int N, byte_size;
	pt_expr **aux;
	for(aux = nt_exprs; *aux; aux++);
	N = aux - nt_exprs;
	byte_size = N * sizeof(pt_expr *);
	if(aux = malloc(byte_size)) {
		return f(memcpy(aux, nt_exprs, byte_size), N, action);
	}
	else {
		return NULL;
	}
}

