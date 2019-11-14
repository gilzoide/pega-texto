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

#include "vm.h"
#include "bytecode.h"
#include "pega-texto/match.h"

#include <stdio.h>
#include <string.h>

int pt_init_vm(pt_vm *vm) {
	vm->bytecode = NULL;
	return dsa_init_with_size(&vm->memory, 4096);
}

void pt_release_vm(pt_vm *vm) {
	dsa_release(&vm->memory);
}

void pt_vm_load_bytecode(pt_vm *vm, pt_bytecode *bytecode) {
	vm->bytecode = bytecode;
}

pt_bytecode *pt_vm_unload_bytecode(pt_vm *vm) {
	pt_bytecode *bytecode = vm->bytecode;
	vm->bytecode = NULL;
	return bytecode;
}

void pt_vm_unload_and_release_bytecode(pt_vm *vm) {
	pt_release_bytecode(vm->bytecode);
	vm->bytecode = NULL;
}

typedef struct pt_vm_match_state {
	const char *sp; // string pointer
	int qc; // quantifier counter
} pt_vm_match_state;

// Match flags
enum pt_vm_match_flag {
	FAIL_HANDLER_FLAG  = 0b0001,
	SYNTAX_ERROR_FLAG  = 0b0010,
};

#define NEXT_BYTE() \
	*(++ip)
#define NEXT_2_BYTES() \
	*((uint16_t *)(++ip)); ip++
#define ADVANCE_UNTIL_NULL() \
	while(NEXT_BYTE())
#define PEEK_NEXT_BYTE() \
	((int8_t *)ip)[1]

int pt_vm_match(pt_vm *vm, const char *str, void *userdata) {
	if(str == NULL) return PT_NULL_INPUT;
	pt_bytecode *bytecode = vm->bytecode;
	if(bytecode == NULL) return PT_VM_NULL_BYTECODE;

	uint8_t *chunk_ptr = pt_byte_at(bytecode, 0);
	/* uint_fast16_t chunk_fail = bytecode->chunk.size - 1; */

	/* pt_bytecode_constant *rc; // constant register */
	const char *sp = str;    // string pointer
	uint8_t *ip = chunk_ptr; // instruction pointer
	int rf = 1;              // result (0 if failing)
	int qc = 0;              // quantifier counter
	enum pt_opcode instruction, opcode;

	pt_vm_match_state *state;
	pt_list_(pt_vm_match_state) state_stack;
	pt_list_initialize_as(&state_stack, 8, pt_vm_match_state);

	int matched;
	
	uint8_t *chunk_end = bytecode->chunk.arr + bytecode->chunk.size;
	while(ip < chunk_end) {
		instruction = *ip;
		opcode = instruction;
#if 1
		int i;
		/* printf("{ qc = %d }\n", qc); */
		/* for(i = state_stack.size - 1; i >= 0; i--) { */
			/* pt_vm_match_state *aux = pt_list_at(&state_stack, i, pt_vm_match_state); */
			/* printf("{ qc = %d }\n", aux->qc); */
		/* } */
		printf("\nip = %ld, qc = %d - '%.*s'", ip - (uint8_t *)bytecode->chunk.arr, qc, 40, sp);
		/* int fodas; */
		/* scanf("%d", &fodas); */
		/* printf("\n\n"); */
#endif

		switch(opcode) {
			case NOP:
				break;
			case SUCCEED:
				rf = 1;
				break;
			case FAIL:
				rf = 0;
				break;
			case FAIL_LESS_THEN:
				rf = qc >= NEXT_BYTE();
				break;
			case QC_ZERO:
				qc = 0;
				break;
			case QC_INC:
				qc++;
				break;

			case JUMP: {
				int address = NEXT_2_BYTES();
				ip = chunk_ptr + address;
				continue;
			}
			case JUMP_RELATIVE: {
				int offset = PEEK_NEXT_BYTE();
				ip += offset;
				continue;
			}
			case JUMP_RELATIVE_IF_FAIL: {
				int offset = PEEK_NEXT_BYTE();
				if(!rf) {
					ip += offset;
					continue;
				}
				else {
					ip++;
				}
				break;
			}
			case JUMP_IF_FAIL: {
				int address = NEXT_2_BYTES();
				if(!rf) {
					ip = chunk_ptr + address;
					continue;
				}
				break;
			}
			case CALL: //TODO
			case RET: //TODO
			case PUSH:
				if(state = pt_list_push_as(&state_stack, pt_vm_match_state)) { \
					*state = (pt_vm_match_state){ .sp = sp, .qc = qc };
				}
				else {
					matched = PT_NO_STACK_MEM;
					goto match_end;
				}
				break;
			case PEEK:
				state = pt_list_peek_as(&state_stack, pt_vm_match_state);
				sp = state->sp;
				qc = state->qc;
				break;
			case POP:
				pt_list_pop_as(&state_stack, pt_vm_match_state);
				break;

			case BYTE:
				rf = *sp == NEXT_BYTE();
				sp += rf;
				break;
			case NOT_BYTE:
				rf = *sp != NEXT_BYTE();
				sp += rf;
				break;
			case STRING: {
				int c;
				const char *str_aux = sp;
				do {
					c = NEXT_BYTE();
				} while(c && c == *(str_aux++));
				if(c != '\0') { // didn't reach end, failed!
					ADVANCE_UNTIL_NULL();
					rf = 0;
				}
				else {
					sp = str_aux;
					rf = 1;
				}
				break;
			}
			case CLASS: {
				int (*f)(int) = pt_function_for_character_class(NEXT_BYTE());
				rf = f(*sp) != 0;
				sp += rf;
				break;
			}
			case SET: {
				rf = 0;
				int base = *sp, c;
				while(c = NEXT_BYTE()) {
					if(c == base) {
						rf = 1;
						sp++;
						break;
					}
				}
				ADVANCE_UNTIL_NULL();
				break;
			}
			case RANGE: {
				int b = *sp;
				int rangemin = NEXT_BYTE();
				int rangemax = NEXT_BYTE();
				rf = b >= rangemin && b <= rangemax;
				sp += rf;
				break;
			}

			default: // unknown opcode
				matched = PT_VM_INVALID_INSTRUCTION;
				printf("!!! Unknow opcode: 0x%02x; ip = %ld, sp = '%s'\n", opcode, ip - (uint8_t *)bytecode->chunk.arr, sp);
				goto match_end;
		}
		ip++;
	}
	matched = rf ? sp - str : PT_NO_MATCH;
match_end:
	pt_list_destroy(&state_stack);
	return matched;
	/* return (pt_match_result){matched, result_data}; */
}
#undef ADVANCE_BYTE
#undef NEXT_BYTE
#undef ADVANCE_UNTIL_NULL
#undef NEXT_CONSTANT
#undef SET_FLAG
#undef UNSET_FLAG
#undef TOGGLE_FLAG

