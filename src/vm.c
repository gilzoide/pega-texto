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

#include <pega-texto/vm.h>
#include <pega-texto/bytecode.h>
#include <pega-texto/compiler.h>

#include <stdio.h>
#include <string.h>

int pt_init_vm(pt_vm *vm) {
	vm->bytecode = NULL;
	return pt_memory_init(&vm->memory);
}

void pt_release_vm(pt_vm *vm) {
	pt_memory_release(&vm->memory);
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
	uint16_t addr_fail; // addres to jump to on fail
	uint16_t addr_accept; // address to jump to on success
	int qc; // quantifier counter
} pt_vm_match_state;

// Match flags
enum pt_vm_match_flag {
	FAIL_HANDLER_FLAG  = 0b0001,
	SYNTAX_ERROR_FLAG  = 0b0010,
};

#define ADVANCE_BYTE() \
	++ip
#define NEXT_BYTE() \
	*(++ip)
#define NEXT_2_BYTES() \
	*((uint16_t *)(++ip)); ip++
#define ADVANCE_UNTIL_NULL() \
	while(NEXT_BYTE())
#define NEXT_CONSTANT() \
	pt_constant_at(bytecode, NEXT_BYTE())
#define GET_FLAG(flag) \
	fr & flag
#define SET_FLAG(flag) \
	fr |= flag
#define UNSET_FLAG(flag) \
	fr &= ~flag
#define TOGGLE_FLAG(flag) \
	fr ^= flag

#define PUSH_STATE() \
	if(state = pt_list_push_as(&state_stack, pt_vm_match_state)) { \
		*state = (pt_vm_match_state){ .sp = sp, .addr_fail = addr_fail, .addr_accept = addr_accept, .qc = qc }; \
	} \
	else { \
		matched = PT_NO_STACK_MEM; \
		goto match_end; \
	}

pt_match_result pt_vm_match(pt_vm *vm, const char *str, void *userdata) {
	if(str == NULL) return (pt_match_result){ PT_NULL_INPUT, PT_NULL_DATA };
	pt_bytecode *bytecode = vm->bytecode;
	if(bytecode == NULL) return (pt_match_result){ PT_VM_NULL_BYTECODE, PT_NULL_DATA };

	pt_data result_data = PT_NULL_DATA;

	uint8_t *chunk_ptr = pt_byte_at(bytecode, 0);
	uint_fast16_t chunk_fail = bytecode->chunk.size - 1;

	/* pt_bytecode_constant *rc; // constant register */
	const char *sp = str; // string pointer
	uint8_t *ip = chunk_ptr; // instruction pointer
	uint16_t addr_fail = chunk_fail, addr_accept = chunk_fail - 1;
	int qc = 0; // quantifier counter
	enum pt_opcode instruction, opcode, and_flag;
	int sp_inc = 0;

	pt_vm_match_state *state;
	pt_list_(pt_vm_match_state) state_stack;
	pt_list_initialize_as(&state_stack, 8, pt_vm_match_state);

	int matched;

	while(1) {
		instruction = *ip;
		opcode = instruction & PT_OP_MASK;
		and_flag = instruction & PT_OP_AND;
#if 0
		int i;
		printf("{ accept = %d, fail = %d, qc = %d }\n", addr_accept, addr_fail, qc);
		for(i = state_stack.size - 1; i >= 0; i--) {
			pt_vm_match_state *aux = pt_list_at(&state_stack, i, pt_vm_match_state);
			printf("{ accept = %d, fail = %d, qc = %d }\n", aux->addr_accept, aux->addr_fail, aux->qc);
		}
		printf("\nip = %ld \n'%.*s...'", ip - (uint8_t *)bytecode->chunk.arr, 40, sp);
		int fodas;
		scanf("%d", &fodas);
		printf("\n\n");
#endif
		switch(opcode) {
			case PT_OP_POP_ACCEPT:
				state = pt_list_pop_as(&state_stack, pt_vm_match_state);
				if(state) {
					ip = chunk_ptr + addr_accept;
					addr_fail = state->addr_fail;
					addr_accept = state->addr_accept;
					qc = state->qc;
					continue;
				}
				else {
					matched = sp - str;
					// TODO: actions
					goto match_end;
				}
			case PT_OP_SET_ADDRESS_FAIL:
				addr_fail = NEXT_2_BYTES();
				break;
			case PT_OP_SET_ADDRESS_ACCEPT:
				addr_accept = NEXT_2_BYTES();
				break;
			case PT_OP_PUSH:
				PUSH_STATE();
				break;
			case PT_OP_BYTE:
				if(*sp == NEXT_BYTE()) {
					sp_inc = !and_flag;
					break;
				}
				else goto failed_match;
			case PT_OP_STRING:
				{
					int c;
					const char * str_aux = sp;
					do {
						c = NEXT_BYTE();
					} while(c && c == *(str_aux++));
					if(c != '\0') { // didn't reach end, failed!
						ADVANCE_UNTIL_NULL();
						goto failed_match;
					}
					else {
						sp_inc = !and_flag * (str_aux - sp);
					}
				}
				break;
			case PT_OP_CHAR_CLASS:
				{
					int (*f)(int) = pt_function_for_character_class(NEXT_BYTE());
					if(f(*sp)) {
						sp_inc = !and_flag;
					}
					else goto failed_match;
				}
				break;
			case PT_OP_SET:
				{
					int base = *sp, c;
					while(c = NEXT_BYTE()) {
						if(c == base) {
							sp_inc = !and_flag;
							break;
						}
					}
					ADVANCE_UNTIL_NULL();
					if(c != base) goto failed_match;
				}
				break;
			case PT_OP_RANGE:
				{
					int b = *sp;
					int rangemin = NEXT_BYTE();
					int rangemax = NEXT_BYTE();
					if(b >= rangemin && b <= rangemax) {
						sp_inc = !and_flag;
					}
					else goto failed_match;
				}
				break;
			case PT_OP_CALL:
				{
					int address = *(pt_list_at(&bytecode->rule_addresses, NEXT_BYTE(), uint16_t));
					PUSH_STATE();
					addr_accept = ip - chunk_ptr + 1;
					addr_fail = chunk_fail;
					ip = chunk_ptr + address;
					continue;
				}
				break;
			case PT_OP_JUMP_ABSOLUTE:
				{
					int address = NEXT_2_BYTES();
					ip = chunk_ptr + address;
					continue;
				}
				break;
			case PT_OP_SAVE_SP:
				/* state->sp = sp; */
				break;
			case PT_OP_RESET_QC:
				qc = 0;
				break;
			case PT_OP_INC_QC:
				qc++;
				break;
			case PT_OP_FAIL_QC_LESS_THAN:
				if(qc < NEXT_BYTE()) {
					goto failed_match;
				}
				break;
			case PT_OP_POP_FAIL:
				state = pt_list_pop_as(&state_stack, pt_vm_match_state);
				if(state) {
					sp = state->sp;
					addr_fail = state->addr_fail;
					addr_accept = state->addr_accept;
					qc = state->qc;
				}
				else {
					matched = PT_NO_MATCH;
					goto match_end;
				}
				// fallthrough
			case PT_OP_FAIL:
failed_match:
				if(instruction & PT_OP_NOT) {
					sp_inc = !and_flag;
					goto succeeded_not_match;
				}
failed_not_match:
				ip = chunk_ptr + addr_fail;
				continue;
				

			default: // unknown opcode
				matched = PT_VM_INVALID_INSTRUCTION;
				/* printf("!!! Unknow opcode: 0x%02x; ip = %ld, sp = '%s'\n", opcode, ip - (uint8_t *)bytecode->chunk.arr, sp); */
				goto match_end;
		}
		if(instruction & PT_OP_NOT) {
			goto failed_not_match;
		}
succeeded_not_match:
		ip++;
		if(sp_inc > 0) {
			sp += sp_inc;
			sp_inc = 0;
		}
	}
match_end:
	pt_list_destroy(&state_stack);
	return (pt_match_result){matched, result_data};
}
#undef ADVANCE_BYTE
#undef NEXT_BYTE
#undef ADVANCE_UNTIL_NULL
#undef NEXT_CONSTANT
#undef SET_FLAG
#undef UNSET_FLAG
#undef TOGGLE_FLAG

