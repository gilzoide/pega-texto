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

/** @file bytecode.h
 * Bytecode for compiled pega-texto Grammars.
 */

#ifndef __PEGA_TEXTO_BYTECODE_H__
#define __PEGA_TEXTO_BYTECODE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "list.h"

#include <stdint.h>
#include <stdio.h>

enum pt_opcode { 
	NOP,
	SUCCEED,
	FAIL,
	FAIL_LESS_THEN,
	QC_ZERO,
	QC_INC,
	JUMP_RELATIVE,
	JUMP,
	JUMP_RELATIVE_IF_FAIL,
	JUMP_IF_FAIL,
	CALL,
	RET,
	PUSH,
	PEEK,
	POP,
	BYTE,
	NOT_BYTE,
	STRING,
	CLASS,
	SET,
	RANGE,
	ACTION,

	PT_OPCODE_ENUM_COUNT,
};
extern const char * const pt_opcode_description[];

/**
 * Compiled grammar.
 */
typedef struct pt_bytecode {
	pt_list_(uint8_t) chunk;
} pt_bytecode;
#define PT_CHUNK_LIST_INITIAL_CAPACITY 2048

typedef uint16_t pt_bytecode_address;

const pt_bytecode pt_constant_bytecode(int size, uint8_t *arr);

/**
 * Initialize a Bytecode struct with the expected values.
 */
int pt_init_bytecode(pt_bytecode *bytecode);

/**
 * Release the memory associated with Bytecode and reinitialize it with zeros.
 *
 * @note It is safe to pass a NULL pointer here.
 *
 * @warning Never call this in an unitialized Bytecode.
 */
void pt_release_bytecode(pt_bytecode *bytecode);

/**
 * Clear the bytecode, without freeing the used memory.
 */
void pt_clear_bytecode(pt_bytecode *bytecode);

/**
 * Push a byte into bytecode chunk.
 *
 * @return 0 on memory allocation error.
 * @return 1 otherwise.
 */
uint8_t *pt_push_byte(pt_bytecode *bytecode, uint8_t b);
/**
 * Push several bytes into bytecode chunk.
 *
 * @return 0 on memory allocation error.
 * @return 1 otherwise.
 */
uint8_t *pt_push_bytes(pt_bytecode *bytecode, int num_bytes, ...);
/**
 * Push several bytes into bytecode chunk.
 *
 * @return 0 on memory allocation error.
 * @return 1 otherwise.
 */
uint8_t *pt_push_byte_array(pt_bytecode *bytecode, int num_bytes, const uint8_t *bs);
/**
 * Push bytes into bytecode chunk without initialization.
 */
uint8_t *pt_reserve_bytes(pt_bytecode *bytecode, int num_bytes);

uint8_t *pt_push_address(pt_bytecode *bytecode, pt_bytecode_address address);

uint8_t *pt_byte_at(const pt_bytecode *bytecode, int i);

pt_bytecode_address pt_current_address(const pt_bytecode *bytecode);
void pt_patch_address(pt_bytecode_address *pointer_in_bytecode, pt_bytecode_address address);

size_t pt_bytecode_write_to_file(const pt_bytecode *bytecode, FILE *file);

/**
 * Utility to dump a bytecode textual representation into stdout, for debugging
 * purposes.
 */
void pt_dump_bytecode(const pt_bytecode *bytecode);

#define COMPILE_CONSTANT_BYTECODE(var, ...) \
	const uint8_t __##var[] = { __VA_ARGS__ }; \
	pt_bytecode var = { .chunk = { .arr = (void *)__##var, .size = sizeof(__##var), .capacity = sizeof(__##var), } };

#ifdef __cplusplus
}
#endif

#endif


