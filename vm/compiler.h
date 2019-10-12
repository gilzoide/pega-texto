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

/** @file compiler.h
 * Compiler for pega-texto grammars.
 */

#ifndef __PEGA_TEXTO_COMPILER_H__
#define __PEGA_TEXTO_COMPILER_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct pt_bytecode pt_bytecode;
typedef struct pt_grammar pt_grammar;
typedef struct pt_memory pt_memory;

/**
 * Possible status codes for Grammar compilation.
 */
enum pt_compile_status {
	PT_COMPILE_SUCCESS = 0,  ///< No errors on compilation
	// Grammar validation errors
	PT_COMPILE_NULL_GRAMMAR = -1,  ///< Grammar is a NULL pointer
	PT_COMPILE_EMPTY_GRAMMAR = -2,  ///< Grammar doesn't present any Rules
	PT_COMPILE_NULL_POINTER = -3,  ///< NULL pointer as Expression data
	PT_COMPILE_INVALID_RANGE = -4,  ///< Range characters must be numerically ordered
	PT_COMPILE_OUT_OF_BOUNDS = -5,  ///< Non-terminal index is out of Grammar bounds
	PT_COMPILE_UNDEFINED_RULE = -6,  ///< Rule undefined in given Grammar
	PT_COMPILE_LOOP_EMPTY_STRING = -7,  ///< Loop body may accept empty string
	// Other compiler errors
	PT_COMPILE_MEMORY_ERROR = -8,  ///< Malloc error
	PT_COMPILE_INVALID_EXPR = -9,  ///< Encountered an invalid or not yet supported Expression

	PT_COMPILE_STATUS_ENUM_COUNT = 10,
};

/**
 * String description of the compilation status codes
 *
 * @see pt_compile_status
 */
const char *pt_get_compile_status_description(int compile_status);

/**
 * Compile a Grammar into a bytecode to be run by the VM.
 *
 * @note This checks if a Grammar is well-formed, as described by
 *  [Ford (2014)](https://pdos.csail.mit.edu/~baford/packrat/popl04/peg-popl04.pdf),
 *  and if Non-terminal indexes (either by name or numerical index) exist and
 *  are inbounds.
 *
 * @param[out] bytecode Bytecode to be filled with instructions for matching the given Grammar.
 * @param g             Grammar to be compiled.
 * @return Compilation result.
 */
enum pt_compile_status pt_compile_grammar(pt_bytecode *bytecode, pt_grammar *g, pt_memory *memory);

#ifdef __cplusplus
}
#endif

#endif

