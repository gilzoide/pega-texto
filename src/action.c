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

#include <pega-texto/action.h>
#include "_action.h"

#include <string.h>

#if __STDC_VERSION__ >= 201112L && !(__STDC_NO_THREADS__) && NDEBUG
# include <threads.h>
# define PT_PRIVATE thread_local static
#else
# define PT_PRIVATE static
#endif

/// The Action Stack we'll be dealing with.
static pt_match_action_stack *stack;
/// The subject string.
PT_PRIVATE const char *str;
/// The userdata passed.
PT_PRIVATE void *data;
/// The Action sequence, a tree containing which action should run when (by index).
PT_PRIVATE int *action_sequence;
/// The number of arguments for each Action.
PT_PRIVATE int *num_args;
/// Mark which Actions were already visited.
PT_PRIVATE char *was_visited;

/**
 * Calculates which Action should be performed berofe which, and how many
 * arguments are needed.
 */
static void pt_calculate_action_sequence(int action_index, int *visited) {
	if(was_visited[action_index]) return;
	was_visited[action_index] = 1;

	int i;
	pt_match_action *action = stack->actions + action_index;
	for(i = action_index + 1; i < stack->size; i++) {
		// `action` is `i`s child, which needs one more argument
		if(stack->actions[i].start <= action->start && stack->actions[i].end >= action->end) {
			num_args[i]++;
			pt_calculate_action_sequence(i, visited);
			break;
		}
	}
	action_sequence[(*visited)++] = action_index;
}

/**
 * Structure that keeps track of how many Actions were visited/run and the
 * fold's result.
 */
typedef struct {
	unsigned int visited;
	pt_data data;
} pt_fold_result;

/// Run the Actions, folding them into a single #pt_data.
static pt_fold_result pt_fold_actions(int index) {
	int i, action_index = action_sequence[index];
	int N = num_args[action_index];
	unsigned int visited;
	pt_match_action *action = stack->actions + action_index;
	pt_fold_result res;

	// Process arguments
	pt_data arguments[N];
	for(visited = i = 0; i < N; i++) {
		res = pt_fold_actions(index + 1 + visited);
		arguments[i] = res.data;
		visited += res.visited;
	}
	return (pt_fold_result) {
		.visited = visited + 1,
		.data = action->f(str, action->start, action->end, N, arguments, data),
	};
}

pt_data pt_run_actions(pt_match_action_stack *a, const char *s, void *userdata) {
	// store stuff in static variables, so we don't clutter the memory
	// stack with duplicated values
	stack = a;
	data = userdata;
	str = s;
	int _action_sequence[a->size];
	int _num_args[a->size];
	char _was_visited[a->size];
	action_sequence = _action_sequence;
	num_args = memset(_num_args, 0, a->size * sizeof(int));
	was_visited = memset(_was_visited, 0, a->size * sizeof(int));

	// find the Action sequence
	int i, visited = 0;
	for(i = 0; i < a->size; i++) pt_calculate_action_sequence(i, &visited);
	// and Fold It, 'til there is no Actions left
	pt_fold_result res;
	for(i = 0; i < a->size; i += res.visited) {
		res = pt_fold_actions(i);
	}
	return res.data;
}

