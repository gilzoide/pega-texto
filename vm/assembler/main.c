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

#include "list.h"
#include "assembler_parser.h"
#include "bytecode.h"
#include "vm.h"

#include <stdio.h>

pt_data _test_action(const char *str, int size, int id, int argc, pt_data *argv, void *userdata) {
	printf("Action: %d '%.*s'\n", size, size, str);
	return PT_NULL_DATA;
}

COMPILE_CONSTANT_BYTECODE(teste,
	NOT_BYTE, 0,
	NOT_BYTE, 0,
	NOT_BYTE, 0,
	NOT_BYTE, 0,
	ACTION, 1,
);

int main(int argc, char **argv) {
	pt_bytecode *bytecode = pt_assembly_bytecode;
	pt_dump_bytecode(bytecode);

	pt_vm vm;
	pt_init_vm(&vm);
	pt_vm_load_bytecode(&vm, bytecode);

	int matched = pt_vm_match(&vm, "# olars\nde novo", &_test_action, NULL);
	printf("Matched %d\n", matched);

	pt_release_vm(&vm);
	return 0;
}
