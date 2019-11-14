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

#include "pega-texto/memory.h"

#include <stdlib.h>
#include <string.h>

int pt_memory_init(pt_memory *memory) {
	return pt_memory_init_with_size(memory, PT_MEMORY_INITIAL_SIZE);
}

int pt_memory_init_with_size(pt_memory *memory, int size) {
	memory->buffer = malloc(size);
	int malloc_success = memory->buffer != NULL;
	memory->capacity = malloc_success * size;
	memory->top = (malloc_success * size) - 1;
	memory->bottom = 0;
	return malloc_success;
}

void pt_memory_release(pt_memory *memory) {
	free(memory->buffer);
	// zero out buffer, capacity and bottom, set top to -1
	*memory = (pt_memory){
		.top = -1,
	};
}

void *pt_memory_alloc_top(pt_memory *memory, int size) {
	if(memory->top - size < memory->bottom) return NULL;
	memory->top -= size;
	void *ptr = memory->buffer + memory->top + 1;
	return ptr;
}

void *pt_memory_alloc_bottom(pt_memory *memory, int size) {
	if(memory->bottom + size > memory->top) return NULL;
	void *ptr = memory->buffer + memory->bottom;
	memory->bottom += size;
	return ptr;
}

int pt_memory_available(pt_memory *memory) {
	return memory->top + 1 - memory->bottom;
}

int pt_memory_used(pt_memory *memory) {
	return memory->bottom + (memory->capacity - (memory->top + 1));
}

pt_memory_marker pt_memory_get_top_marker(pt_memory *memory) {
	return memory->top;
}
pt_memory_marker pt_memory_get_bottom_marker(pt_memory *memory) {
	return memory->bottom;
}

void pt_memory_free_top(pt_memory *memory) {
	memory->top = memory->capacity - 1;
}

void pt_memory_free_bottom(pt_memory *memory) {
	memory->bottom = 0;
}

void pt_memory_free_top_marker(pt_memory *memory, pt_memory_marker marker) {
	if(marker > memory->top && marker < memory->capacity) {
		memory->top = marker;
	}
}

void pt_memory_free_bottom_marker(pt_memory *memory, pt_memory_marker marker) {
	if(marker < memory->bottom && marker >= 0) {
		memory->bottom = marker;
	}
}

