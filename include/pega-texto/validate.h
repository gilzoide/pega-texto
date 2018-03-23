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

/** @file validate.h
 * Validation of Grammars.
 */

#ifndef __PEGA_TEXTO_VALIDATE_H__
#define __PEGA_TEXTO_VALIDATE_H__

#include "pega-texto/grammar.h"

/**
 * Possible status codes for Grammar validation.
 */
typedef enum {
	PT_VALIDATE_SUCCESS = 0,  ///< No errors on validation
	PT_VALIDATE_NULL_GRAMMAR,  ///< Grammar is a NULL pointer
	PT_VALIDATE_EMPTY_GRAMMAR,  ///< Grammar doesn't present any Rules
	PT_VALIDATE_NULL_POINTER,  ///< NULL pointer as Expression data
	PT_VALIDATE_RANGE_BUFFER,  ///< Range buffer must have at least 2 characters
	PT_VALIDATE_INVALID_RANGE,  ///< Range characters must be numerically ordered
	PT_VALIDATE_OUT_OF_BOUNDS,  ///< Non-terminal index is out of Grammar bounds
	PT_VALIDATE_UNDEFINED_RULE,  ///< Rule undefined in given Grammar
	PT_VALIDATE_LOOP_EMPTY_STRING,  ///< Loop body may accept empty string
} pt_validate_codes;

/**
 * String description of the validate status codes
 *
 * @see pt_validate_codes
 */
extern const char * const pt_validate_codes_description[];

/**
 * Behaviours for Grammar validation.
 */
typedef enum {
	/**
	 * Default behaviour: validate when asked, without printing errors nor aborting.
	 */
	PT_VALIDATE_DEFAULT = 0,
	/**
	 * Skip well-formedness validation.
	 *
	 * This still looks for `NULL` pointers, as they may be introduced by
	 * `malloc` errors.
	 *
	 * This is useful for release builds when using non-changing Grammars
	 * you know are valid.
	 */
	PT_VALIDATE_SKIP = 0b001,
	/**
	 * Print an error message on `stderr` if Grammar is invalid.
	 */
	PT_VALIDATE_PRINT_ERROR = 0b010,
	/**
	 * Abort program if Grammar is invalid, printing an error message on `stderr`.
	 *
	 * May be useful for Debug builds.
	 */
	PT_VALIDATE_ABORT = 0b110,
} pt_validate_behaviour;

/**
 * Validation result: a {status code, invalid rule index (if there is one)} pair.
 *
 * If status code is different from PT_VALIDATE_SUCCESS, `rule` will contain the
 * rule index in which the problem was found.
 *
 * @see pt_validate_codes
 */
typedef struct {
	uint8_t status;  ///< Status code.
	uint16_t rule;  ///< Rule index.
} pt_validate_result;

/**
 * Validate a Grammar.
 *
 * This checks if a Grammar is well-formed, as described by
 * [Ford (2014)](https://pdos.csail.mit.edu/~baford/packrat/popl04/peg-popl04.pdf),
 * and if Non-terminal indexes (either by name or numerical index) exist and
 * are inbounds.
 *
 * @param g   Grammar to be validated.
 * @param bhv Validation behaviour
 * @return Validate result: a {status code, invalid rule index (if there is one)} pair.
 */
pt_validate_result pt_validate_grammar(pt_grammar *g, pt_validate_behaviour bhv);


#endif

