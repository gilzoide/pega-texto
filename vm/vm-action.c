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

#include "vm-action.h"

#include "double_stack_allocator.h"

#include <stdlib.h>

pt_data pt_vm_run_actions(double_stack_allocator *memory, pt_vm_capture *captures, int size, const char *str, pt_vm_action action, void *userdata) {
	// allocate the data stack
	pt_data *data_stack;
	int bottom_marker = dsa_get_bottom_marker(memory);
	if((data_stack = dsa_alloc_bottom(memory, size * sizeof(pt_data))) == NULL) return PT_NULL_DATA;

	// index to current Data on the stack
	int data_index = 0;

	// Fold It, 'til there are no Actions left.
	// Note that this only works because of how the Actions are layed out in
	// memory
	pt_vm_capture *capture;
	for(capture = captures; capture < captures + size; capture++) {
		// "pop" arguments
		data_index -= capture->argc;
		// run action with arguments (which are still stacked in `data_stack` in the right position)
		data_stack[data_index] = action(str + capture->start, capture->end - capture->start, capture->id, capture->argc, data_stack + data_index, userdata);
		// "push" result
		data_index++;
	}
	pt_data res = data_stack[0];
	dsa_free_bottom_marker(memory, bottom_marker);
	return res;
}
