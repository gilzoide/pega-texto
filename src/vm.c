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

#include <string.h>

void pt_init_vm(pt_vm *vm) {
	memset(vm, 0, sizeof(pt_vm));
}

void pt_release_vm(pt_vm *vm) {
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
	const char *sp;
	uint8_t *ip;
} pt_vm_match_state;

// Match flags
enum pt_vm_match_flag {
	FAILED_FLAG        = 0b0001,
	SYNTAX_ERROR_FLAG  = 0b0010,
};

#define ADVANCE_BYTE() \
	++ip
#define NEXT_BYTE() \
	*(++ip)
#define ADVANCE_UNTIL_NULL() \
	while(NEXT_BYTE())
#define NEXT_CONSTANT() \
	pt_constant_at(bytecode, NEXT_BYTE())
#define SET_FLAG(flag) \
	fr |= flag
#define UNSET_FLAG(flag) \
	fr &= ~flag
#define TOGGLE_FLAG(flag) \
	fr ^= flag

pt_match_result pt_vm_match(pt_vm *vm, const char *str, void *userdata) {
	if(str == NULL) return (pt_match_result){ PT_NULL_INPUT, PT_NULL_DATA };
	pt_bytecode *bytecode = vm->bytecode;
	if(bytecode == NULL) return (pt_match_result){ PT_VM_NULL_BYTECODE, PT_NULL_DATA };

	pt_bytecode_constant *rc; // constant register
	const char *sp = str; // string pointer
	uint8_t *ip = pt_byte_at(bytecode, 0); // instruction pointer
	enum pt_opcode instruction, not_flag, and_flag;
	enum pt_vm_match_flag fr = 0; // flag register
	int b; // auxiliary byte holder
	int sp_inc = 0;

	pt_vm_match_state state = {
		.sp = sp,
		.ip = ip,
	};
	pt_list_(pt_vm_match_state) state_stack;
	pt_list_initialize_as(&state_stack, 8, pt_vm_match_state);

	pt_data result_data = PT_NULL_DATA;
	int matched;

	while(1) {
		b = *ip;
		instruction = b & PT_OP_MASK;
		not_flag = b & PT_OP_NOT;
		and_flag = b & PT_OP_AND;
		switch(instruction) {
			case PT_OP_SUCCESS:
				matched = sp - str;
				// TODO: actions
				goto match_end;
			case PT_OP_BYTE:
				if(*sp == NEXT_BYTE()) {
					sp_inc = !and_flag;
					break;
				}
				else goto match_fail;
			case PT_OP_STRING:
				{
					int c;
					const char * str_aux = sp;
					do {
						c = NEXT_BYTE();
					} while(c && c == *(str_aux++));
					if(c != '\0') { // didn't reach end, failed!
						ADVANCE_UNTIL_NULL();
						goto match_fail;
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
					else goto match_fail;
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
					if(c != base) goto match_fail;
				}
				break;
			case PT_OP_RANGE:
				{
					b = *sp;
					int rangemin = NEXT_BYTE();
					int rangemax = NEXT_BYTE();
					if(b >= rangemin && b <= rangemax) {
						sp_inc = !and_flag;
					}
					else goto match_fail;
				}
				break;
match_fail:
			case PT_OP_FAIL:
				if(not_flag) {
					sp_inc = !and_flag;
				}
				else if(!pt_list_empty(&state_stack)) {
					// TODO
				}
				else {
					matched = PT_NO_MATCH;
					goto match_end;
				}
				break;

			default: // unknown opcode
				matched = PT_VM_INVALID_INSTRUCTION;
				goto match_end;
		}
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

