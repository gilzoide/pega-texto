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

/** @file vm.h
 * Virtual Machine for running compiled pega-texto grammar matches.
 */

#ifndef __PEGA_TEXTO_VM_H__
#define __PEGA_TEXTO_VM_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "match.h"
#include "memory.h"

// Forward declarations
typedef struct pt_bytecode pt_bytecode;

/**
 * Virtual Machine that runs `pt_bytecode` compiled grammars.
 */
typedef struct pt_vm {
	pt_bytecode *bytecode;
	pt_memory memory;
} pt_vm;

int pt_init_vm(pt_vm *vm);
void pt_release_vm(pt_vm *vm);

void pt_vm_load_bytecode(pt_vm *vm, pt_bytecode *bytecode);
pt_bytecode *pt_vm_unload_bytecode(pt_vm *vm);
void pt_vm_unload_and_release_bytecode(pt_vm *vm);

pt_match_result pt_vm_match(pt_vm *vm, const char *str, void *userdata);

#ifdef __cplusplus
}
#endif

#endif

