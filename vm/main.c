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

#include "pega-texto/list.h"
#include "bytecode.h"
#include "vm.h"

#include <stdio.h>

int main(int argc, char **argv) {
	uint8_t code[] = {
		QC_ZERO,
		CLASS, 'w',
		JUMP_RELATIVE_IF_FAIL, 5,
		QC_INC,
		JUMP_RELATIVE, -5,
		// label Fail
		FAIL_LESS_THEN, 1,
	};
	pt_bytecode bytecode = {
		.chunk = { .arr = code, .size = sizeof(code), .capacity = sizeof(code) }
	};
	pt_dump_bytecode(&bytecode);

	pt_vm vm;
	pt_init_vm(&vm);
	pt_vm_load_bytecode(&vm, &bytecode);

	int matched = pt_vm_match(&vm, "olars", NULL);
	printf("Matched %d\n", matched);

	pt_release_vm(&vm);
	return 0;
}

