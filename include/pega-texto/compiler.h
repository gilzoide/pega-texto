/*
 * Copyright 2017, 2018 Gil Barbosa Reis <gilzoide@gmail.com>
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

#include "pega-texto/grammar.h"

/**
 * Possible status codes for Grammar compilation.
 */
enum pt_compile_status {
	PT_COMPILE_SUCCESS = 0,  ///< No errors on compilation
	// Grammar validation errors
	PT_COMPILE_NULL_GRAMMAR,  ///< Grammar is a NULL pointer
	PT_COMPILE_EMPTY_GRAMMAR,  ///< Grammar doesn't present any Rules
	PT_COMPILE_NULL_POINTER,  ///< NULL pointer as Expression data
	PT_COMPILE_RANGE_BUFFER,  ///< Range buffer must have at least 2 characters
	PT_COMPILE_INVALID_RANGE,  ///< Range characters must be numerically ordered
	PT_COMPILE_OUT_OF_BOUNDS,  ///< Non-terminal index is out of Grammar bounds
	PT_COMPILE_UNDEFINED_RULE,  ///< Rule undefined in given Grammar
	PT_COMPILE_LOOP_EMPTY_STRING,  ///< Loop body may accept empty string
	// Other compiler errors
	PT_COMPILE_MEMORY_ERROR,  ///< Malloc error
	PT_COMPILE_CONSTANTS_LIMIT,

	PT_COMPILE_STATUS_ENUM_COUNT,
};

/**
 * String description of the compilation status codes
 *
 * @see pt_compile_status
 */
extern const char * const pt_compile_status_description[];

enum pt_opcode {
	// TODO
	PT_OP_FAIL,
	PT_OP_RETURN,
	PT_OP_LITERAL, // 1 -> constant
};

typedef union pt_bytecode_constant {
	const char *characters;
	int offset;
	void *ptr;
} pt_bytecode_constant;
#define PT_MAX_CONSTANTS 255

/**
 * Compiled grammar.
 */
typedef struct pt_bytecode {
	uint8_t *chunk;
	int chunk_size;
	int chunk_capacity;

	int constants_size;
	pt_bytecode_constant constants[PT_MAX_CONSTANTS];
} pt_bytecode;

/**
 * Initialize a Bytecode struct with the expected values.
 */
void pt_init_bytecode(pt_bytecode *bytecode);

/**
 * Release the memory associated with Bytecode and reinitialize it with zeros.
 *
 * @note It is safe to pass a NULL pointer here.
 *
 * @warning Never call this in an unitialized Bytecode.
 */
void pt_release_bytecode(pt_bytecode *bytecode);

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
enum pt_compile_status pt_compile_grammar(pt_bytecode *bytecode, pt_grammar *g);


#ifdef __cplusplus
}
#endif

#endif

