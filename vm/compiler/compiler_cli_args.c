/*
 * Copyright 2017-2020 Gil Barbosa Reis <gilzoide@gmail.com>
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

#include "compiler_cli_args.h"

#include <stdio.h>

int pt_compiler_parse_args(int argc, const char **argv, pt_compiler_args *compiler_args) {
    if(argc <= 1) {
        pt_compiler_print_usage(argv);
        return 0;
    }
    compiler_args->filename = argv[1];
    return 1;
}

void pt_compiler_print_usage(char **argv) {
    printf("Usage: %s FILE", argv[0]);
}
