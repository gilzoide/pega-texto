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

#include "disassembler.h"

#include "logging.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>

int main(int argc, const char **argv) {
    pt_set_log_level_from_env();
    if(argc < 2) {
        pt_log(PT_LOG_ERROR, "Usage: pega-texto-disassembler (-- | FILE)");
        return -1;
    }
    const char *filename = argv[1];
    FILE *file = strcmp(filename, "--") == 0 ? stdin : fopen(filename, "r");
    if(file == NULL) {
        pt_log(PT_LOG_ERROR, "Error reading file '%s': %s", filename, strerror(errno));
        return errno;
    }

    int ret = pt_dump_bytecode_from_file(file);

    fclose(file);
    return ret;
}
