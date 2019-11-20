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

/** @file action.h
 * Actions to be performed on certain points of the matching algorithm.
 */

#ifndef __PEGA_TEXTO_VM_ACTION_H__
#define __PEGA_TEXTO_VM_ACTION_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "pega-texto/data.h"

typedef struct double_stack_allocator double_stack_allocator;

/**
 * Action to be called on an Expression, after the whole match succeeds.
 *
 * This is the action to be set to an Expression individually, and will be
 * called only if the whole match succeeds, in the order the Expressions were
 * matched.
 *
 * Expression Actions reduce inner Actions' result into a single value.
 *
 * Parameters:
 * - Pointer to the start of the match/capture
 * - Number of bytes contained in the match/capture
 * - Identifier of the action called
 * - Number of #pt_data arguments 
 * - #pt_data arguments, processed on inner Actions. Currently, this array is
 *   reused, so you should not rely on it after Action has returned
 * - User custom data from match options
 *
 * Return:
 *   Anything you want.
 *   This result will be used as argument for other actions below in the stack.
 */
typedef pt_data(*pt_vm_action)(const char *,
                               int,
							   int,
                               int,
                               pt_data *,
                               void *);

/**
 * Queried actions, to be executed on match success.
 */
typedef struct pt_vm_capture {
	int start;  ///< Start point of the match.
	int end;  ///< End point of the match.
	int argc;  ///< Number of arguments that will be passed when Action is executed.
	int id;  ///< Number representing the capture identifier
} pt_vm_capture;

pt_data pt_vm_run_actions(double_stack_allocator *memory, pt_vm_capture *captures, int size, const char *str, pt_vm_action action, void *userdata);

#ifdef __cplusplus
}
#endif

#endif

