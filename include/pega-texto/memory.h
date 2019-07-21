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

#ifndef __PEGA_TEXTO_MEMORY_H__
#define __PEGA_TEXTO_MEMORY_H__

#define PT_MEMORY_INITIAL_SIZE 4096

/// Marker for memory allocations.
typedef int pt_memory_marker;

/**
 * Custom memory manager: a double stack allocator.
 *
 * The bottom marker is increased, so pushed memory blocks have increasing addresses.
 * The top marker is decreased, so pushed memory blocks have decreasing addresses.
 *
 */
typedef struct pt_memory {
	void *buffer;  ///< Memory buffer used
	int capacity;  ///< Capacity of memory buffer
	pt_memory_marker top;  ///< Top mark, moved when allocating from the top
	pt_memory_marker bottom;  ///< Bottom mark, moved when allocating from the bottom
} pt_memory;

int pt_memory_init(pt_memory *memory);
int pt_memory_init_with_size(pt_memory *memory, int size);
void pt_memory_release(pt_memory *memory);

void *pt_memory_alloc_top(pt_memory *memory, int size);
void *pt_memory_alloc_bottom(pt_memory *memory, int size);

int pt_memory_available(pt_memory *memory);
int pt_memory_used(pt_memory *memory);

pt_memory_marker pt_memory_get_top_marker(pt_memory *memory);
pt_memory_marker pt_memory_get_bottom_marker(pt_memory *memory);

void pt_memory_free_top(pt_memory *memory);
void pt_memory_free_bottom(pt_memory *memory);

void pt_memory_free_top_marker(pt_memory *memory, pt_memory_marker marker);
void pt_memory_free_bottom_marker(pt_memory *memory, pt_memory_marker marker);

#endif

