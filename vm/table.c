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

#include "table.h"

#include <stdlib.h>
#include <string.h>

#define HASH_SEED 2166136261u
#define HASH_MULTIPLIER 16777619
#define TABLE_MAX_LOAD 0.75
#define TOMBSTONE_VALUE (uintptr_t)-1
#define GROW_CAPACITY(capacity) \
    ((capacity) < 8 ? 8 : (capacity) * 2)

static uint32_t hash_string(const char *str, int length) {
    uint32_t hash = HASH_SEED;

    for(int i = 0; i < length; i++) {
        hash ^= str[i];
        hash *= HASH_MULTIPLIER;
    }

    return hash;
}

static pt_table_entry *find_entry(pt_table_entry *entries, int capacity, const char *key, int length) {
    uint32_t hash = hash_string(key, length);
    uint32_t index = hash % capacity;    
    pt_table_entry *tombstone = NULL;         
    while(1) {
        pt_table_entry *entry = &entries[index];

        if (entry->key == NULL) {
            if(entry->value != TOMBSTONE_VALUE) {
                return tombstone ?: entry;
            }
            else {
                if(tombstone == NULL) tombstone = entry;
            }
        }
        else if(entry->length == length && strncmp(entry->key, key, length) == 0) {
            return entry;
        }

        index = (index + 1) % capacity;
    }
}

static int adjust_capacity(pt_table *table, int capacity) {
    pt_table_entry *old_entries = table->entries;

    int size = capacity * sizeof(pt_table_entry);
    pt_table_entry *new_entries = malloc(size);
    if(new_entries == NULL) return 0;
    memset(new_entries, 0, size);
    
    int count = 0;
    if(table->count > 0) {
        for(int i = 0; i < capacity; i++) {
            pt_table_entry entry = old_entries[i];
            if(entry.key == NULL) continue;

            pt_table_entry *dest = find_entry(new_entries, capacity, entry.key, entry.length);
            *dest = entry;
            count++;
        }
    }
    free(old_entries);

    table->entries = new_entries;
    table->capacity = capacity;
    table->count = count;
    return 1;
}

int pt_table_init(pt_table *table, pt_table_value_destructor value_destructor) {
    table->entries = NULL;
    table->capacity = 0;
    table->count = 0;
    table->value_destructor = value_destructor;
    return 1;
}

void pt_table_destroy(pt_table *table) {
    pt_table_value_destructor destructor = table->value_destructor;
    if(destructor) {
        for(pt_table_entry *it = table->entries; it < table->entries + table->capacity; it++) {
            if(it->key != NULL) {
                destructor(it->value);
            }
        }
    }
    free(table->entries);
    pt_table_init(table, table->value_destructor);
}

int pt_table_get(pt_table *table, const char *key, int length, uintptr_t *value) {      
    if (table->count == 0) return 0;

    pt_table_entry *entry = find_entry(table->entries, table->capacity, key, length);
    if (entry->key == NULL) return 0;

    *value = entry->value;
    return 1;
}

int pt_table_set(pt_table *table, const char *key, int length, uintptr_t value) {
    if (table->count + 1 > table->capacity * TABLE_MAX_LOAD) {
        int capacity = GROW_CAPACITY(table->capacity);
        if(!adjust_capacity(table, capacity)) return 0;
    }
    
    pt_table_entry *entry = find_entry(table->entries, table->capacity, key, length);

    int isNewKey = entry->key == NULL;
    if (isNewKey && entry->value != TOMBSTONE_VALUE) {
        table->count++;
    }

    entry->key = key;
    entry->length = length;
    entry->value = value;
    return 1;
}

int pt_table_delete(pt_table *table, const char *key, int length) {
    if(table->count == 0) return 0;

    pt_table_entry *entry = find_entry(table->entries, table->capacity, key, length);
    if(entry->key == NULL) return 0;

    if(table->value_destructor) table->value_destructor(entry->value);
    entry->key = NULL;
    entry->length = 0;
    entry->value = TOMBSTONE_VALUE;
    return 1;
}
