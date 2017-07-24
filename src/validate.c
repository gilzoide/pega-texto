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

/// Look for a name in the array.
static inline int pt_find_non_terminal_index(const char *name, const char **names, int N) {
	int i;
	for(i = 0; i < N; i++) {
		if(strcmp(name, names[i]) == 0) return i;
	}
	return -1;
}

/// Is an Expression Nullable (does it accept the empty string)?
static inline int pt_is_nullable(const pt_grammar *g, const pt_expr *e) {
	int i, res;
	switch(e->op) {
		case PT_AND: case PT_NOT:
			return 1;

		case PT_QUANTIFIER:
			return e->N <= 0;

		case PT_NON_TERMINAL:
			return pt_is_nullable(g, g->es[e->N]);

		case PT_SEQUENCE:
			for(res = i = 0; i < e->N; i++) {
				res |= pt_is_nullable(g, e->data.es[i]);
			}
			return res;

		case PT_CHOICE:
			for(i = 0; i < e->N; i++) {
				if(res = pt_is_nullable(g, e->data.es[i])) {
					return 1;
				}
			}
			break;
	}
	return 0;
}

/// Validates a single expression in the Grammar, to be called recursively.
static int pt_validate_expr_in_grammar(pt_grammar *g, pt_expr *e, uint16_t *rule, uint8_t *visited_rules) {
	int i, res;
	uint16_t cur_rule = *rule;
	if(visited_rules[cur_rule] == 0) {
		visited_rules[cur_rule] = 1;
		switch(e->op) {
			case PT_RANGE:
				if(strlen(e->data.characters) < 2) return PT_VALIDATE_RANGE_BUFFER;
				else if(e->data.characters[0] > e->data.characters[1]) return PT_VALIDATE_INVALID_RANGE;
				break;

			case PT_NON_TERMINAL:
				if(e->N < 0) { 
					if((e->N = pt_find_non_terminal_index(e->data.characters, g->names, g->N)) == -1) {
						return PT_VALIDATE_UNDEFINED_RULE;
					}
				}
				else {
					if(e->N >= g->N) return PT_VALIDATE_OUT_OF_BOUNDS;
				}
				*rule = e->N;
				if((res = pt_validate_expr_in_grammar(g, g->es[e->N], rule, visited_rules)) != PT_VALIDATE_SUCCESS) {
					return res;
				}
				*rule = cur_rule;
				break;

			case PT_QUANTIFIER:
				if((res = pt_validate_expr_in_grammar(g, e->data.e, rule, visited_rules)) != PT_VALIDATE_SUCCESS) {
					return res;
				}
				else if(e->N == 0 && pt_is_nullable(g, e->data.e)) return PT_VALIDATE_LOOP_EMPTY_STRING;
				break;

			case PT_AND: case PT_NOT:
				return pt_validate_expr_in_grammar(g, e->data.e, rule, visited_rules);

			case PT_SEQUENCE: case PT_CHOICE:
				for(i = 0; i < e->N; i++) {
					if((res = pt_validate_expr_in_grammar(g, e->data.es[i], rule, visited_rules)) != PT_VALIDATE_SUCCESS) {
						return res;
					}
				}
				break;
		}
	}
	return PT_VALIDATE_SUCCESS;
}

pt_validate_result pt_grammar_validate(pt_grammar *g, pt_validate_behaviour bhv) {
	pt_validate_result res = {};
	if(bhv != PT_VALIDATE_SKIP) {
		uint8_t visited_rules[g->N];
		memset(visited_rules, 0, g->N * sizeof(uint8_t));
		res.status = pt_validate_expr_in_grammar(g, g->es[0], &res.rule, visited_rules);
		if(res.status != PT_VALIDATE_SUCCESS && bhv & PT_VALIDATE_PRINT_ERROR) {
			fprintf(stderr, "[pt_grammar_validate] Error on rule \"%s\": %s\n",
					g->names[res.rule], pt_validate_codes_description[res.status]);
			if(bhv == PT_VALIDATE_ABORT) exit(res.status);
		}
	}
	return res;
}

const char * const pt_validate_codes_description[] = {
	"No errors on grammar",
	"Range buffer must have at least 2 characters",
	"Range characters must be numerically ordered",
	"Non-terminal index is out of Grammar bounds",
	"Rule undefined in given Grammar",
	"Loop body may accept empty string",
};
