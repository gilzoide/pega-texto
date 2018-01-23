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

#include <pega-texto/validate.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Auxiliary structure for keeping validations per Rule, to avoid Cycles between non-terminals.
typedef struct {
	uint8_t was_visited : 1;
	uint8_t is_nullable_visited : 1;
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
static int pt_validate_expr_in_grammar(pt_grammar *g, pt_expr *e, uint16_t *rule, pt_validation_per_rule *visited_rules, int should_skip);

/// Is an Expression Nullable (does it accept the empty string)?
static int pt_is_nullable(pt_grammar *g, pt_expr *e, uint16_t *rule, pt_validation_per_rule *visited_rules) {
	int i, res;
	uint16_t cur_rule = *rule;
	visited_rules[cur_rule].is_nullable_visited = 1;

	switch(e->op) {
		case PT_AND: case PT_NOT:
			return PT_VALIDATE_LOOP_EMPTY_STRING;

		case PT_QUANTIFIER:
			return e->N <= 0 ? PT_VALIDATE_LOOP_EMPTY_STRING : PT_VALIDATE_SUCCESS;

		case PT_NON_TERMINAL:
			// Maybe non-terminal wasn't seen yet: validate it first (to correct this)
			if(e->N < 0) {
				if((res = pt_validate_expr_in_grammar(g, e, rule, visited_rules, 1)) != PT_VALIDATE_SUCCESS) {
					return res;
				}
			}
			// Only visit non-terminal if it hasn't yet been visited
			if(visited_rules[e->N].is_nullable_visited == 0) {
				*rule = e->N;
				if((res = pt_is_nullable(g, g->es[e->N], rule, visited_rules)) != PT_VALIDATE_SUCCESS) {
					return res;
				}
				*rule = cur_rule;
			}
			break;

		case PT_SEQUENCE:
			for(i = 0; i < e->N; i++) {
				res = pt_is_nullable(g, e->data.es[i], rule, visited_rules);
				if(res == PT_VALIDATE_SUCCESS || res != PT_VALIDATE_LOOP_EMPTY_STRING) {
					return res;
				}
			}
			return PT_VALIDATE_LOOP_EMPTY_STRING;
		
		case PT_CHOICE:
			for(i = 0; i < e->N; i++) {
				if((res = pt_is_nullable(g, e->data.es[i], rule, visited_rules)) != PT_VALIDATE_SUCCESS) {
					return res;
				}
			}
			break;

		default: break;
	}
	return PT_VALIDATE_SUCCESS;
}

/// Validates a single expression in the Grammar, to be called recursively.
static int pt_validate_expr_in_grammar(pt_grammar *g, pt_expr *e, uint16_t *rule, pt_validation_per_rule *visited_rules, int should_skip) {
	if(e == NULL) {
		return PT_VALIDATE_NULL_POINTER;
	}

	int i, res;
	uint16_t cur_rule = *rule;
	visited_rules[cur_rule].was_visited = 1;

	switch(e->op) {
		case PT_RANGE:
			if(e->data.characters == NULL) {
				return PT_VALIDATE_NULL_POINTER;
			}
			else if(!should_skip) {
				if(strlen(e->data.characters) < 2) {
					return PT_VALIDATE_RANGE_BUFFER;
				}
				else if(e->data.characters[0] > e->data.characters[1]) {
					return PT_VALIDATE_INVALID_RANGE;
				}
			}
			break;

		case PT_NON_TERMINAL:
			if(e->N < 0) {
				if(e->data.characters == NULL) {
					return PT_VALIDATE_NULL_POINTER;
				}
				else if((e->N = pt_safely_find_non_terminal_index(e->data.characters, g->names, g->N)) == -1) {
					return PT_VALIDATE_UNDEFINED_RULE;
				}
			}
			else if(e->N >= g->N) {
				return PT_VALIDATE_OUT_OF_BOUNDS;
			}
			// Only visit non-terminal if it hasn't yet been visited
			if(visited_rules[e->N].was_visited == 0) {
				*rule = e->N;
				if((res = pt_validate_expr_in_grammar(g, g->es[e->N], rule, visited_rules, should_skip)) != PT_VALIDATE_SUCCESS) {
					return res;
				}
				*rule = cur_rule;
			}
			break;

		case PT_QUANTIFIER:
			if(e->data.e == NULL) {
				return PT_VALIDATE_NULL_POINTER;
			}
			else if((res = pt_validate_expr_in_grammar(g, e->data.e, rule, visited_rules, should_skip)) != PT_VALIDATE_SUCCESS) {
				return res;
			}
			else if(!should_skip && e->N == 0 && (res = pt_is_nullable(g, e->data.e, rule, visited_rules))) {
				return res;
			}
			break;

		case PT_AND: case PT_NOT:
			if(e->data.e == NULL) {
				return PT_VALIDATE_NULL_POINTER;
			}
			else {
				return pt_validate_expr_in_grammar(g, e->data.e, rule, visited_rules, should_skip);
			}

		case PT_SEQUENCE: case PT_CHOICE:
            // NULL pointer is valid if there are zero Expressions
			if(e->data.es == NULL && e->N != 0) {
				return PT_VALIDATE_NULL_POINTER;
			}
			for(i = 0; i < e->N; i++) {
				if(e->data.es[i] == NULL) {
					return PT_VALIDATE_NULL_POINTER;
				}
				else if((res = pt_validate_expr_in_grammar(g, e->data.es[i], rule, visited_rules, should_skip)) != PT_VALIDATE_SUCCESS) {
					return res;
				}
			}
			break;

		case PT_ERROR:
			if(e->data.e != NULL) {
				if((res = pt_validate_expr_in_grammar(g, e->data.e, rule, visited_rules, should_skip)) != PT_VALIDATE_SUCCESS) {
					return res;
				}
				else if(!should_skip && (res = pt_is_nullable(g, e->data.e, rule, visited_rules))) {
					return res;
				}
			}
			break;
	}
	return PT_VALIDATE_SUCCESS;
}

pt_validate_result pt_validate_grammar(pt_grammar *g, pt_validate_behaviour bhv) {
	pt_validate_result res = {};
	if(g == NULL) {
		res.status = PT_VALIDATE_NULL_GRAMMAR;
	}
	else if(g->N == 0) {
		res.status = PT_VALIDATE_EMPTY_GRAMMAR;
	}
	else {
		pt_validation_per_rule visited_rules[g->N];
		memset(visited_rules, 0, g->N * sizeof(pt_validation_per_rule));
		res.status = pt_validate_expr_in_grammar(g, g->es[0], &res.rule, visited_rules, bhv & PT_VALIDATE_SKIP);
	}
	if(res.status != PT_VALIDATE_SUCCESS && bhv & PT_VALIDATE_PRINT_ERROR) {
		fprintf(stderr, "[pt_grammar_validate] Error");
		if(g != NULL) {
			fprintf(stderr, " on rule \"%s\"", g->names[res.rule]);
		}
		fprintf(stderr, ": %s\n", pt_validate_codes_description[res.status]);
		if(bhv == PT_VALIDATE_ABORT) {
			exit(res.status);
		}
	}
	return res;
}

const char * const pt_validate_codes_description[] = {
	"No errors on Grammar",
	"Grammar is a NULL pointer",
	"Grammar doesn't present any Rules",
	"NULL pointer as Expression data",
	"Range buffer must have at least 2 characters",
	"Range characters must be numerically ordered",
	"Non-terminal index is out of Grammar bounds",
	"Rule undefined in given Grammar",
	"Loop body may accept empty string",
};
