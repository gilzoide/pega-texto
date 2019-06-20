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
#include <stdarg.h>
#include <string.h>
#include <assert.h>

const char * const pt_opcode_description[] = {
	"PT_OP_FAIL",
	"PT_OP_ACCEPT",

	"PT_OP_SET_ADDRESS_FAIL",
	"PT_OP_SET_ADDRESS_ACCEPT",
	"PT_OP_PUSH",
	"PT_OP_POP_ACCEPT",
	"PT_OP_POP_FAIL",

	"PT_OP_BYTE",
	"PT_OP_STRING",
	"PT_OP_SET",
	"PT_OP_CHAR_CLASS",
	"PT_OP_RANGE",
	"PT_OP_CALL",
	"PT_OP_JUMP_ABSOLUTE",
	"PT_OP_SAVE_SP",

	"PT_OP_RESET_QC",
	"PT_OP_INC_QC",
	"PT_OP_FAIL_QC_LESS_THAN",
	"PT_OP_JUMP_QC_GREATER_EQUAL_THAN",
};
#ifdef static_assert
static_assert(sizeof(pt_opcode_description) == PT_OPCODE_ENUM_COUNT * sizeof(const char *),
              "Missing opcode descriptions");
#endif

void pt_init_bytecode(pt_bytecode *bytecode) {
	pt_list_initialize_as(&bytecode->chunk, PT_CHUNK_LIST_INITIAL_CAPACITY, uint8_t);
	pt_list_initialize_as(&bytecode->rule_addresses, PT_RULE_ADDRESSES_LIST_INITIAL_CAPACITY, uint16_t);
	pt_list_initialize_as(&bytecode->constants, PT_CONSTANTS_LIST_INITIAL_CAPACITY, pt_bytecode_constant);
}

void pt_release_bytecode(pt_bytecode *bytecode) {
	pt_list_destroy(&bytecode->chunk);
	pt_list_destroy(&bytecode->rule_addresses);
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

int pt_push_constant(pt_bytecode *bytecode, pt_bytecode_constant c) {
	pt_bytecode_constant *const_ptr = pt_list_push_as(&bytecode->constants, pt_bytecode_constant);
	if(const_ptr) {
		*const_ptr = c;
		return pt_push_byte(bytecode, bytecode->constants.size - 1);
	}
	else return 0;
}

uint8_t *pt_byte_at(pt_bytecode *bytecode, int i) {
	/* if(i < 0) i = bytecode->chunk.size + i; */
	return pt_list_at(&bytecode->chunk, i, uint8_t);
}

pt_bytecode_constant * const pt_constant_at(const pt_bytecode *bytecode, int i) {
	return pt_list_at(&bytecode->constants, i, pt_bytecode_constant);
}

void pt_dump_bytecode(const pt_bytecode *bytecode) {
	/* pt_bytecode_constant *constant; */
	uint8_t *pc, *bytecode_start = bytecode->chunk.arr, *bytecode_end = bytecode_start + bytecode->chunk.size;
	int b, instruction, not_flag, and_flag;
	printf("Size: %d\n", bytecode->chunk.size);
#define PRINT_BYTE(fmt, ...) \
	printf("%4ld | 0x%02x  | " fmt "\n", pc - bytecode_start, *pc, ##__VA_ARGS__)
#define PRINT_SHORT(s) \
	printf("%4ld | addr  | %d\n", pc - bytecode_start, s)
#define PRINT_STR(s) \
	printf("%4ld | char* | \"%s\"\n", pc - bytecode_start, s)
#define PRINT_STR_CONSTANT(c) \
	PRINT_BYTE("const.as_str '%s'", c->as_str)
	for(pc = bytecode_start; pc < bytecode_end; pc++) {
		b = *pc;
		instruction = b & PT_OP_MASK;
		not_flag = b & PT_OP_NOT;
		and_flag = b & PT_OP_AND;
		PRINT_BYTE("%s%s%s", and_flag ? "&" : "", not_flag ? "!" : "", pt_opcode_description[instruction] + 3);
		switch(instruction) {
			case PT_OP_BYTE:
			case PT_OP_CHAR_CLASS:
				pc++;
				PRINT_BYTE("'%c'", *pc);
				break;
			case PT_OP_CALL:
			case PT_OP_FAIL_QC_LESS_THAN:
				pc++;
				PRINT_BYTE("%d", (int)*pc);
				break;
			case PT_OP_RANGE:
				pc++;
				PRINT_BYTE("'%c'", *pc);
				pc++;
				PRINT_BYTE("'%c'", *pc);
				break;
			case PT_OP_STRING:
			case PT_OP_SET:
				pc++;
				PRINT_STR(pc);
				pc += strlen((const char *)pc);
				break;
			case PT_OP_JUMP_ABSOLUTE:
			case PT_OP_SET_ADDRESS_FAIL:
			case PT_OP_SET_ADDRESS_ACCEPT:
				pc++;
				b = (int16_t)*((int16_t *)pc);
				PRINT_SHORT(b);
				pc++;
				break;
			default: break;
		}
	}

	uint16_t *addr, *addr_start = bytecode->rule_addresses.arr, *addr_end = addr_start + bytecode->rule_addresses.size;
	printf("Rules: %d\n", bytecode->rule_addresses.size);
	for(addr = addr_start; addr < addr_end; addr++) {
		printf("%3ld | %d\n", addr - addr_start, *addr);
	}
}
#undef PRINT_BYTE
#undef PRINT_STR_CONSTANT

