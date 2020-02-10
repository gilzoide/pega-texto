/*
 * Copyright 2017-2020 Gil Barbosa Reis <gilzoide@gmail.com>
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

#ifndef __PEGA_TEXTO_TABLE_H__
#define __PEGA_TEXTO_TABLE_H__

#include "list.h"

#include <stdint.h>

typedef struct pt_table_entry {
    const char *key;
    uintptr_t value;
} pt_table_entry;

typedef void(*pt_table_entry_destructor)(uintptr_t);

typedef struct pt_table {
    pt_table_entry *entries;
    int capacity;
    int count;
    pt_table_entry_destructor entry_destructor;
} pt_table;

int pt_table_init(pt_table *table, pt_table_entry_destructor entry_destructor);
void pt_table_destroy(pt_table *table);

int pt_table_get(pt_table *table, const char *key, uintptr_t *value);
int pt_table_set(pt_table *table, const char *key, uintptr_t value);
int pt_table_delete(pt_table *table, const char *key);

#define pt_table_iterate(table, it_var) \
    for(pt_table_entry *__it = table->entries; __it < table->entries + table->capacity; __it++) if(__it->key != NULL && (it_var = __it->value, 1))

#endif
