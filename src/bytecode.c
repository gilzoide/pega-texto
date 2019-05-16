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

#include <pega-texto/bytecode.h>

#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <assert.h>

const char * const pt_opcode_description[] = {
	"PT_OP_FAIL",
	"PT_OP_SUCCESS",
	"PT_OP_RETURN",
	"PT_OP_BYTE",
	"PT_OP_NOT_BYTE",
	"PT_OP_STRING",
	"PT_OP_SET",
};
#ifdef static_assert
static_assert(sizeof(pt_opcode_description) == PT_OPCODE_ENUM_COUNT * sizeof(const char *),
              "Missing opcode descriptions");
#endif

void pt_init_bytecode(pt_bytecode *bytecode) {
	pt_list_initialize_as(&bytecode->chunk, PT_CHUNK_LIST_INITIAL_CAPACITY, uint8_t);
	pt_list_initialize_as(&bytecode->constants, PT_CONSTANT_LIST_INITIAL_CAPACITY, pt_bytecode_constant);
}

void pt_release_bytecode(pt_bytecode *bytecode) {
	pt_list_destroy(&bytecode->chunk);
	pt_list_destroy(&bytecode->constants);
}

void pt_clear_bytecode(pt_bytecode *bytecode) {
	pt_list_clear(&bytecode->chunk);
	pt_list_clear(&bytecode->constants);
}

int pt_push_byte(pt_bytecode *bytecode, uint8_t b) {
	uint8_t *byte_ptr = pt_list_push_as(&bytecode->chunk, uint8_t);
	return byte_ptr && (*byte_ptr = b, 1);
}

int pt_push_bytes(pt_bytecode *bytecode, int num_bytes, const uint8_t *bs) {
	uint8_t *byte_ptr = pt_list_push_n_as(&bytecode->chunk, num_bytes, uint8_t);
	return byte_ptr && (memcpy(byte_ptr, bs, num_bytes * sizeof(uint8_t)), 1);
}

int pt_push_constant(pt_bytecode *bytecode, pt_bytecode_constant c) {
	pt_bytecode_constant *const_ptr = pt_list_push_as(&bytecode->constants, pt_bytecode_constant);
	if(const_ptr) {
		*const_ptr = c;
		return pt_push_byte(bytecode, bytecode->constants.size - 1);
	}
	else return 0;
}

uint8_t * const pt_byte_at(const pt_bytecode *bytecode, int i) {
	return pt_list_at(&bytecode->chunk, i, uint8_t);
}

pt_bytecode_constant * const pt_constant_at(const pt_bytecode *bytecode, int i) {
	return pt_list_at(&bytecode->constants, i, pt_bytecode_constant);
}

void pt_dump_bytecode(const pt_bytecode *bytecode) {
	pt_bytecode_constant *constant;
	uint8_t *pc, *bytecode_start = bytecode->chunk.arr, *bytecode_end = bytecode_start + bytecode->chunk.size;
	printf("Size: %d\n", bytecode->chunk.size);
#define PRINT_BYTE(fmt, ...) \
	printf("%4ld | 0x%02x  | " fmt "\n", pc - bytecode_start, *pc, ##__VA_ARGS__)
#define PRINT_STR(s) \
	printf("%4ld | char* | \"%s\" \n", pc - bytecode_start, s)
#define PRINT_STR_CONSTANT(c) \
	PRINT_BYTE("const.as_str '%s'", c->as_str)
	for(pc = bytecode_start; pc < bytecode_end; pc++) {
		uint8_t b = *pc;
		PRINT_BYTE("%s", pt_opcode_description[b]);
		switch(b) {
			case PT_OP_BYTE:
			case PT_OP_NOT_BYTE:
				pc++;
				PRINT_BYTE("'%c'", *pc);
				break;
			case PT_OP_STRING:
			case PT_OP_SET:
				pc++;
				PRINT_STR(pc);
				pc += strlen((const char *)pc);
				break;
			default: break;
		}
	}
}
#undef PRINT_BYTE
#undef PRINT_STR_CONSTANT

