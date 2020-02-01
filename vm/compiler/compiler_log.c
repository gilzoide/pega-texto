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

#include "compiler_log.h"

#include <stdio.h>
#include <stdarg.h>

static enum pt_compiler_log_level global_log_level = LOG_DEBUG;

void pt_compiler_set_log_level(enum pt_compiler_log_level level) {
    global_log_level = level;
}

void pt_compiler_log(enum pt_compiler_log_level level, const char *fmt, ...) {
    if(pt_log_level_at_least(level, global_log_level)) {
        va_list args;
        va_start(args, fmt);
        FILE *output = pt_log_level_at_least(level, LOG_ERROR) ? stderr : stdout;
        vfprintf(output, fmt, args);
        fputc('\n', output);
        va_end(args);
    }
}
