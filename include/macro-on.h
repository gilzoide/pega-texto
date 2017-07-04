/*
 * Copyright 2017 Gil Barbosa Reis <gilzoide@gmail.com>
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

/* Turn on Parsing Expression constructor macros */

#ifndef NULL
#include <stdlib.h>
#endif

#define L(s)     (pt_create_literal(s, 0))
#define L_O(s)   (pt_create_literal(s, 1))
#define S(s)     (pt_create_set(s, 0))
#define S_O(s)   (pt_create_set(s, 1))
#define R(s)     (pt_create_range(s, 0))
#define R_O(s)   (pt_create_range(s, 1))
#define ANY      (pt_create_any())
#define V(r)     (pt_create_non_terminal(r, 0))
#define V_O(r)   (pt_create_non_terminal(r, 1))
#define VI(i)    (pt_create_non_terminal_idx(i))
#define Q(e, N)  (pt_create_quantifier(e, N))
#define AND(e)   (pt_create_and(e))
#define NOT(e)   (pt_create_not(e))
#define SEQ(...) (pt__from_nt_array(&pt_create_sequence, ((pt_expr*[]){__VA_ARGS__, NULL})))
#define OR(...)  (pt__from_nt_array(&pt_create_choice,   ((pt_expr*[]){__VA_ARGS__, NULL})))
#define F(f)     (pt_create_custom_matcher(f))

