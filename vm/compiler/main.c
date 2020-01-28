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
#include "compiler.h"

#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static char *readfile(const char *filename) {
    FILE *fp = fopen(filename, "r");
    if(fp == NULL) return NULL;
    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    char *buffer = malloc((size + 1) * sizeof(char));
    if(buffer == NULL) {
        fclose(fp);
        return NULL;
    }
    fread(buffer, sizeof(char), size, fp);
    buffer[size] = '\0';
    fclose(fp);
    return buffer;
}

int main(int argc, const char **argv) {
    pt_compiler_args compiler_args;
    if(!pt_compiler_parse_args(argc, argv, &compiler_args)) {
        return -1;
    }

    char *contents = readfile(compiler_args.filename);
    if(contents == NULL) {
        printf("Error reading file '%s': %s", compiler_args.filename, strerror(errno));
        return errno;
    }
    
    pt_compiler compiler;
    if(!pt_init_compiler(&compiler)) {
        printf("Error initializing compiler");
        free(contents);
        return ENOMEM;
    }


    pt_release_compiler(&compiler);
    free(contents);
    return 0;
}
