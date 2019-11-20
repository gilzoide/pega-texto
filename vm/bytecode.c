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

#include "bytecode.h"

#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>

const char * const pt_opcode_description[] = {
	"nop",
	"succ",
	"fail",
	"flt",
	"qcz",
	"qci",
	"jr",
	"jmp",
	"jrf",
	"jmpf",
	"call",
	"ret",
	"push",
	"peek",
	"pop",
	"byte",
	"nbyte",
	"str",
	"cls",
	"set",
	"rng",
	"act",
};
#ifdef static_assert
static_assert(sizeof(pt_opcode_description) == PT_OPCODE_ENUM_COUNT * sizeof(const char *),
              "Missing opcode descriptions");
#endif

void pt_init_bytecode(pt_bytecode *bytecode) {
	pt_list_initialize_as(&bytecode->chunk, PT_CHUNK_LIST_INITIAL_CAPACITY, uint8_t);
}

void pt_release_bytecode(pt_bytecode *bytecode) {
	pt_list_destroy(&bytecode->chunk);
}

void pt_clear_bytecode(pt_bytecode *bytecode) {
	pt_list_clear(&bytecode->chunk);
}

int pt_push_byte(pt_bytecode *bytecode, uint8_t b) {
	uint8_t *byte_ptr = pt_list_push_as(&bytecode->chunk, uint8_t);
	return byte_ptr && (*byte_ptr = b, 1);
}

int pt_push_bytes(pt_bytecode *bytecode, int num_bytes, ...) {
	uint8_t *byte_ptr = pt_list_push_n_as(&bytecode->chunk, num_bytes, uint8_t);
	if(byte_ptr) {
		va_list args;
		va_start(args, num_bytes);
		int i;
		for(i = 0; i < num_bytes; i++) {
			*byte_ptr = (uint8_t)va_arg(args, int);
			byte_ptr++;
		}
		return 1;
	}
	else return 0;
}

int pt_push_byte_array(pt_bytecode *bytecode, int num_bytes, const uint8_t *bs) {
	uint8_t *byte_ptr = pt_list_push_n_as(&bytecode->chunk, num_bytes, uint8_t);
	return byte_ptr && (memcpy(byte_ptr, bs, num_bytes * sizeof(uint8_t)), num_bytes);
}

int pt_reserve_bytes(pt_bytecode *bytecode, int num_bytes) {
	return pt_list_push_n_as(&bytecode->chunk, num_bytes, uint8_t) != NULL;
}

uint8_t *pt_byte_at(pt_bytecode *bytecode, int i) {
	/* if(i < 0) i = bytecode->chunk.size + i; */
	return pt_list_at(&bytecode->chunk, i, uint8_t);
}

void pt_dump_bytecode(const pt_bytecode *bytecode) {
	/* pt_bytecode_constant *constant; */
	uint8_t *pc, *bytecode_start = bytecode->chunk.arr, *bytecode_end = bytecode_start + bytecode->chunk.size;
	int b, instruction;
	printf("Size: %d\n", bytecode->chunk.size);
	for(pc = bytecode_start; pc < bytecode_end; pc++) {
		b = *pc;
		instruction = b;
		printf("%4ld | 0x%02x | %s", pc - bytecode_start, *pc, pt_opcode_description[instruction]);
		switch(instruction) {
			case FAIL_LESS_THEN: case ACTION:
				pc++;
				printf(" %d", (int)*pc);
				break;
			case JUMP_RELATIVE: case JUMP_RELATIVE_IF_FAIL:
				pc++;
				printf(" %d", *((int8_t *)pc));
				break;
			case JUMP: case JUMP_IF_FAIL: case CALL:
				pc++;
				b = (int16_t)*((int16_t *)pc);
				printf(" %d", b);
				pc++;
				break;
			case BYTE: case NOT_BYTE:
				pc++;
				printf(" '%c'", *pc);
				break;
			case CLASS:
				pc++;
				printf(" \\%c", *pc);
				break;
			case RANGE:
				pc++;
				b = *pc;
				pc++;
				printf(" [%c-%c]", b, *pc);
				break;
			case STRING: case SET:
				pc++;
				printf(" \"%s\"", pc);
				pc += strlen((const char *)pc);
				break;
			default: break;
		}
		printf("\n");
	}
}
#undef PRINT_BYTE
#undef PRINT_STR_CONSTANT

