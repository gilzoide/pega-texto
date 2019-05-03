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

/**
 * @file list.h
 * Generic dynamic sequential list.
 */
#ifndef __PEGA_TEXTO_LIST_H__
#define __PEGA_TEXTO_LIST_H__

/**
 * Dynamic list with constant growth rate.
 */
typedef struct pt_list {
	void *arr;
	unsigned int size;
	unsigned int capacity;
} pt_list;

#define PT_LIST_GROWTH_RATE 1.5

#define pt_list_(c_type) pt_list

int pt_list_initialize(pt_list *lst, unsigned int initial_capacity, unsigned int member_size);
#define pt_list_initialize_as(lst, initial_capacity, type) \
	(pt_list_initialize(lst, initial_capacity, sizeof(type)))

void pt_list_destroy(pt_list *lst);
void pt_list_clear(pt_list *lst);

void *pt_list_push(pt_list *lst, unsigned int member_size);
#define pt_list_push_as(lst, type) \
	((type *) pt_list_push(lst, sizeof(type)))

void *pt_list_pop(pt_list *lst, unsigned int member_size);
#define pt_list_pop_as(lst, type) \
	((type *) pt_list_pop(lst, sizeof(type)))

void *pt_list_peek(const pt_list *lst, unsigned int member_size);
#define pt_list_peek_as(lst, type) \
	((type *) pt_list_peek(lst, sizeof(type)))

#define pt_list_at(lst, i, type) \
	(((type *) (lst)->arr) + i)

#endif

