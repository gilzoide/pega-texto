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

pt_match_result pt_vm_match(pt_vm *vm, const char *str, void *userdata) {
	int matched = PT_NO_MATCH;
	int pc = 0, sp = 0;
	uint8_t *ip = pt_byte_at(vm->bytecode, 0);
	uint8_t instruction;
	pt_bytecode_constant rc; // constant register
	pt_data result_data = {};

	while(1) {
		instruction = *ip;
		switch(instruction) {
			case PT_OP_FAIL:
			case PT_OP_RETURN:
				break;
			case PT_OP_LITERAL:
				break;
		}
	}

	return (pt_match_result){matched, result_data};
}

