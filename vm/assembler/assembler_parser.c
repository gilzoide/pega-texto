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

#include "assembler_parser.h"

COMPILE_CONSTANT_BYTECODE(assembly_bytecode, 
// Space = ('#' [^\n]*) / \s
	BYTE, '#',
	JUMP_RELATIVE_IF_FAIL, 11,
	// loop_not_eol:
	NOT_BYTE, '\n',
	JUMP_RELATIVE_IF_FAIL, 4,
	JUMP_RELATIVE, -4,
	// loop_not_eol_end:
	SUCCEED,
	JUMP_RELATIVE, 5,
	// not_comment:
	CLASS, 's',
	// success:
	RET,
	ACTION, 1,
);

pt_bytecode *pt_assembly_bytecode = &assembly_bytecode;

