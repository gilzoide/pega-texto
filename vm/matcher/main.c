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

#include "bytecode.h"
#include "vm.h"

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static int try_match(const pt_bytecode *bytecode, const char *text) {
    pt_vm vm;
    if(!pt_init_vm(&vm)) return -1;
    pt_vm_load_bytecode(&vm, bytecode);

    int ret = pt_vm_match(&vm, text, NULL, NULL);
    printf("Matched: %d\n", ret);

    pt_vm_unload_bytecode(&vm);
    return ret < 0;
}

static char *readfile(const char *filename, int *out_size) {
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
    if(out_size) *out_size = size;
    return buffer;
}

int main(int argc, const char **argv) {
    if(argc < 3) {
        fprintf(stderr, "Usage: pega-texto-matcher BYTECODE FILE");
        return -1;
    }
    int size;
    char *bytecode_txt = readfile(argv[1], &size);
    char *input = readfile(argv[2], &size);
    if(bytecode_txt == NULL) {
        fprintf(stderr, "Error reading file '%s': %s", bytecode_txt, strerror(errno));
        return errno;
    }
    else if(input == NULL) {
        fprintf(stderr, "Error reading file '%s': %s", input, strerror(errno));
        return errno;
    }

    const pt_bytecode bytecode = pt_constant_bytecode(size, bytecode_txt);
    int ret = try_match(&bytecode, input);

    free(bytecode_txt);
    free(input);
    return ret;
}
