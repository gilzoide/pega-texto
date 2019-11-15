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

#include "list.h"

#include <stdlib.h>
#include <assert.h>

#ifdef static_assert
static_assert(PT_LIST_GROWTH_RATE > 1.0, "Cannot grow list with a grouth rate that is less than 1.0");
#endif

int pt_list_initialize(pt_list *lst, unsigned int initial_capacity, unsigned int member_size) {
	lst->size = 0;
	lst->arr = malloc(initial_capacity * member_size);
	int malloc_success = lst->arr != NULL;
	lst->capacity = malloc_success * initial_capacity;
	return malloc_success;
}

void pt_list_destroy(pt_list *lst) {
	free(lst->arr);
}

void pt_list_clear(pt_list *lst) {
	lst->size = 0;
}

void *pt_list_push(pt_list *lst, unsigned int n_members, unsigned int member_size) {
	if(pt_list_ensure_extra_capacity(lst, n_members, member_size)) {
		void *slot_ptr = lst->arr + (member_size * lst->size);
		lst->size += n_members;
		return slot_ptr;
	}
	else return NULL;
}

void *pt_list_pop(pt_list *lst, unsigned int member_size) {
	if(lst->size > 0) {
		lst->size--;
		return lst->arr + (member_size * lst->size);
	}
	else return NULL;
}

void *pt_list_peek(const pt_list *lst, unsigned int member_size) {
	return lst->size > 0 ? lst->arr + (member_size * (lst->size - 1)) : NULL;
}

int pt_list_ensure_capacity(pt_list *lst, int capacity, unsigned int member_size) {
	int lst_capacity = lst->capacity;
	if(lst_capacity < capacity) {
		do {
			lst_capacity *= PT_LIST_GROWTH_RATE;
		} while(lst_capacity > 0 && lst_capacity < capacity);
		void *arr;
		if(arr = realloc(lst->arr, lst_capacity * member_size)) {
			lst->capacity = lst_capacity;
			lst->arr = arr;
		}
		else return 0;
	}
	return 1;
}

int pt_list_ensure_extra_capacity(pt_list *lst, int extra_capacity, unsigned int member_size) {
	return pt_list_ensure_capacity(lst, lst->size + extra_capacity, member_size);
}

int pt_list_empty(const pt_list *lst) {
	return lst->size <= 0;
}

