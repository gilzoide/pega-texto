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
#include "logging.h"
#include "vm.h"

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static pt_data _action(const char *str, int size, int id, int argc, pt_data *argv, void *userdata) {
    pt_log(PT_LOG_DEBUG, "Action with '%*s' of size %d", size, str, size);
    return PT_NULL_DATA;
}

static int try_match(pt_bytecode *bytecode, const char *text) {
    pt_vm vm;
    if(!pt_init_vm(&vm)) return -1;
    pt_vm_load_bytecode(&vm, bytecode);

    int ret = pt_vm_match(&vm, text, _action, NULL);
    pt_log(PT_LOG_INFO, "Matched: %d\n", ret);

    pt_vm_unload_and_release_bytecode(&vm);
    return ret < 0;
}

static int readbytecode(const char *filename, pt_bytecode *bytecode) {
    FILE *fp = fopen(filename, "r");
    if(fp == NULL) return 0;
    pt_init_bytecode(bytecode);
    int ret = pt_read_bytecode(bytecode, fp);
    fclose(fp);
    return ret;
}

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
    pt_set_log_level_from_env();
    if(argc < 3) {
        pt_log(PT_LOG_ERROR, "Usage: pega-texto-matcher BYTECODE FILE");
        return -1;
    }

    pt_bytecode bytecode;
    if(!readbytecode(argv[1], &bytecode)) {
        pt_log(PT_LOG_ERROR, "Error reading file '%s': %s", argv[1], strerror(errno));
        return errno;
    }
    char *input = readfile(argv[2]);
    if(input == NULL) {
        pt_log(PT_LOG_ERROR, "Error reading file '%s': %s", argv[2], strerror(errno));
        return errno;
    }

    int ret = try_match(&bytecode, input);

    free(input);
    return ret;
}
