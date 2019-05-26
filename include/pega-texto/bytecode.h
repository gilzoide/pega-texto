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

typedef union pt_bytecode_constant {
	const char *as_str;
	void *as_ptr;
	int as_offset;
} pt_bytecode_constant;

/// Create a constant from a string
#define CONST_STRING(str) \
	((pt_bytecode_constant){ .as_str = (str) })

/// Create a constant from a pointer
#define CONST_PTR(ptr) \
	((pt_bytecode_constant){ .as_ptr = (ptr) })

/// Create an offset constant from int value
#define CONST_OFFSET(offset) \
	((pt_bytecode_constant){ .as_offset = (offset) })

enum pt_opcode { 
	PT_OP_FAIL,
	PT_OP_POP_AND_FAIL, // discard top and fail
	PT_OP_RETURN, // pop ip
	PT_OP_BYTE, // +1 -> byte to be matched
	PT_OP_STRING, // + NULL terminated string for literal matching
	PT_OP_SET, // + NULL terminated string for character set
	PT_OP_CHAR_CLASS, // +[wWaAcCdDgGlLpPsSuUxX] -> character class tested
	PT_OP_RANGE, // +2 -> byte range
	PT_OP_CALL, // +1 -> rule index to jump
	PT_OP_PUSH_ADDRESS, // +1 -> address to push as ip
	PT_OP_RETURN_ON_SUCCESS, // if `!fail register` PT_OP_RETURN

	PT_OPCODE_ENUM_COUNT,

	PT_OP_MASK = 0b00001111, // bit mask of the opcode
	PT_OP_NOT  = 0b00010000, // bit that negates the operation to be performed
	PT_OP_AND  = 0b00100000, // bit that makes input to not be consumed on success
};
extern const char * const pt_opcode_description[];

/**
 * Compiled grammar.
 */
typedef struct pt_bytecode {
	pt_list_(uint8_t) chunk;
	pt_list_(uint16_t) rule_addresses;
	pt_list_(pt_bytecode_constant) constants;
} pt_bytecode;
#define PT_CHUNK_LIST_INITIAL_CAPACITY 256
#define PT_RULE_ADDRESSES_LIST_INITIAL_CAPACITY 256
#define PT_CONSTANTS_LIST_INITIAL_CAPACITY 64

/**
 * Initialize a Bytecode struct with the expected values.
 */
void pt_init_bytecode(pt_bytecode *bytecode);

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
int pt_push_byte(pt_bytecode *bytecode, uint8_t b);
/**
 * Push several bytes into bytecode chunk.
 *
 * @return 0 on memory allocation error.
 * @return 1 otherwise.
 */
int pt_push_bytes(pt_bytecode *bytecode, int num_bytes, const uint8_t *bs);
/**
 * Push bytes into bytecode chunk without initialization.
 */
int pt_reserve_bytes(pt_bytecode *bytecode, int num_bytes);

/**
 * Push a constant into bytecode.
 *
 * @return 0 on memory allocation error.
 * @return 1 otherwise.
 */
int pt_push_constant(pt_bytecode *bytecode, pt_bytecode_constant c);

uint8_t *pt_byte_at(pt_bytecode *bytecode, int i);
pt_bytecode_constant * const pt_constant_at(const pt_bytecode *bytecode, int i);

/**
 * Utility to dump a bytecode textual representation into stdout, for debugging
 * purposes.
 */
void pt_dump_bytecode(const pt_bytecode *bytecode);

#ifdef __cplusplus
}
#endif

#endif


