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

/** @file data.h
 * Custom data returned by Expression Actions/Match functions.
 */

#ifndef __PEGA_TEXTO_DATA_H__
#define __PEGA_TEXTO_DATA_H__

/**
 * Collection of possible types for Expression Actions to return.
 *
 * @note This is not a Tagged Union, so you (developer) are responsible for
 * knowing which type each datum is. This can and should be avoided when
 * structuring the Grammar.
 */
typedef union {
	void *p;
	char c;
	unsigned char uc;
	short s;
	unsigned short us;
	int i;
	unsigned int ui;
	long l;
	unsigned long ul;
	long long ll;
	unsigned long long ull;
	float f;
	double d;
} pt_data;

/**
 * Facility to return a null #pt_data
 */
#define PT_NULL_DATA ((pt_data){NULL})

#endif

