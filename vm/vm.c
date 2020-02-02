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
#include "logging.h"
#include "pega-texto/match.h"
#include "vm-action.h"

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
	int capture_marker; // capture top memory marker
} pt_vm_match_state;

#define NEXT_BYTE() \
	*(++ip)
#define NEXT_2_BYTES() \
	(ip += 2, (ip[-1] | (ip[0] << 8)))
#define ADVANCE_UNTIL_NULL() \
	while(NEXT_BYTE())
#define PEEK_NEXT_BYTE() \
	((int8_t *)ip)[1]

int pt_vm_match(pt_vm *vm, const char *str, pt_vm_action action, void *userdata) {
	if(str == NULL) return PT_NULL_INPUT;
	pt_bytecode *bytecode = vm->bytecode;
	if(bytecode == NULL) return PT_VM_NULL_BYTECODE;

	uint8_t *chunk_ptr = pt_byte_at(bytecode, 0);

	const char *sp = str;    // string pointer
	uint8_t *ip = chunk_ptr; // instruction pointer
	int rf = 1;              // result (0 if failing)
	int qc = 0;              // quantifier counter
	int qa = 0;              // queried actions
	enum pt_opcode instruction, opcode;
	char current;

	double_stack_allocator *allocator = &vm->memory;
	int bottom_marker = dsa_get_bottom_marker(allocator);
	int top_marker = dsa_get_top_marker(allocator);
	pt_vm_match_state *state = dsa_alloc_top(allocator, sizeof(pt_vm_match_state));
	*state = (pt_vm_match_state){ .sp = sp, .qc = qc, .capture_marker = bottom_marker };

	pt_vm_capture *capture_stack = dsa_alloc_bottom(allocator, 0);
	pt_vm_capture *capture = NULL;

	int matched;
	pt_data result_data;
	
	uint8_t *chunk_end = bytecode->chunk.arr + bytecode->chunk.size;
	while((current = *sp) && ip < chunk_end) {
		instruction = *ip;
		opcode = instruction;

        pt_log(PT_LOG_DEBUG, "ip = %ld, rf = %d, qc = %d - '%.*s'", ip - (uint8_t *)bytecode->chunk.arr, qc, rf, 40, sp);

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
            case JUMP_IF_SUCCESS: {
				int address = NEXT_2_BYTES();
				if(rf) {
					ip = chunk_ptr + address;
					continue;
				}
				break;
			}
			case CALL: 
				// TODO
				break;
			case RET:
				//TODO
				break;
			case PUSH:
				if(state = dsa_alloc_top(allocator, sizeof(pt_vm_match_state))) {
					*state = (pt_vm_match_state){ .sp = sp, .qc = qc, .capture_marker = dsa_get_bottom_marker(allocator), };
				}
				else {
					matched = PT_NO_STACK_MEM;
					goto match_end;
				}
				break;
			case PEEK:
				state = dsa_peek_top(allocator, sizeof(pt_vm_match_state));
				sp = state->sp;
				qc = state->qc;
				dsa_free_bottom_marker(allocator, state->capture_marker);
				break;
			case POP:
				dsa_free_top_marker(allocator, dsa_get_top_marker(allocator) + sizeof(pt_vm_match_state));
				break;

			case BYTE:
				rf = current == NEXT_BYTE();
				sp += rf;
				break;
			case NOT_BYTE:
				rf = current != NEXT_BYTE();
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
				rf = f(current) != 0;
				sp += rf;
				break;
			}
			case SET: {
				rf = 0;
				int base = current, c;
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
				int b = current;
				int rangemin = NEXT_BYTE();
				int rangemax = NEXT_BYTE();
				rf = b >= rangemin && b <= rangemax;
				sp += rf;
				break;
			}

			case ACTION:
				if(capture = dsa_alloc_bottom(allocator, sizeof(pt_vm_capture))) {
					*capture = (pt_vm_capture){ .start = state->sp - str, .end = sp - str, .argc = qa, .id = NEXT_BYTE(), };
				}
				else {
					matched = PT_NO_STACK_MEM;
					goto match_end;
				}
				break;

			default: // unknown opcode
				matched = PT_VM_INVALID_INSTRUCTION;
				printf("!!! Unknow opcode: 0x%02x; ip = %ld, sp = '%s'\n", opcode, ip - (uint8_t *)bytecode->chunk.arr, sp);
				goto match_end;
		}
		ip++;
	}
	if(rf) {
		matched = sp - str;
		int final_bottom_marker = dsa_get_bottom_marker(allocator);
		int num_captures = (final_bottom_marker - bottom_marker) / sizeof(pt_vm_capture);
		if(action && num_captures > 0) {
			result_data = pt_vm_run_actions(allocator, capture_stack, num_captures, str, action, userdata);
		}
	}
	else {
		matched = PT_NO_MATCH;
	}
match_end:
	dsa_free_top_marker(allocator, top_marker);
	dsa_free_bottom_marker(allocator, bottom_marker);
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

