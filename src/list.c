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

#include <pega-texto/list.h>

#include <stdlib.h>

int pt_list_initialize(pt_list *lst, unsigned int initial_capacity, unsigned int member_size) {
	lst->size = 0;
	if(lst->arr = malloc(initial_capacity * member_size)) {
		lst->capacity = initial_capacity;
	}
	return lst->arr != NULL;
}

void pt_list_destroy(pt_list *lst) {
	free(lst->arr);
}

void pt_list_clear(pt_list *lst) {
	lst->size = 0;
}

void *pt_list_push(pt_list *lst, unsigned int member_size) {
	if(lst->size == lst->capacity) {
		unsigned int new_capacity = lst->capacity * PT_LIST_GROWTH_RATE;
		void *arr;
		if(arr = realloc(lst->arr, new_capacity * member_size)) {
			lst->capacity = new_capacity;
			lst->arr = arr;
		}
		else {
			return NULL;
		}
	}
	return lst->arr + (member_size * lst->size++);
}

void *pt_list_pop(pt_list *lst, unsigned int member_size) {
	if(lst->size > 0) {
		lst->size--;
		return lst->arr + (member_size * lst->size);
	}
	else {
		return NULL;
	}
}

void *pt_list_peek(const pt_list *lst, unsigned int member_size) {
	return lst->size > 0 ? lst->arr + (member_size * (lst->size - 1)) : NULL;
}

int pt_list_empty(const pt_list *lst) {
	return lst->size <= 0;
}

