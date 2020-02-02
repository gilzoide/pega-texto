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

#include "logging.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

static enum pt_log_level global_log_level = PT_LOG_INFO;

void pt_set_log_level(enum pt_log_level level) {
    global_log_level = level;
}

void pt_set_log_level_from_env() {
    const char *env = getenv(PT_LOG_LEVEL_ENV);
    if(env == NULL) return;
    enum pt_log_level log_level = pt_parse_log_level(env);
    if(log_level != PT_LOG_INVALID) {
        global_log_level = log_level;
    }
}

enum pt_log_level pt_parse_log_level(const char *str) {
    if(strcasecmp(str, "none") == 0) {
        return PT_LOG_NONE;
    }
    else if(strcasecmp(str, "error") == 0) {
        return PT_LOG_ERROR;
    }
    else if(strcasecmp(str, "warning") == 0) {
        return PT_LOG_WARNING;
    }
    else if(strcasecmp(str, "info") == 0) {
        return PT_LOG_INFO;
    }
    else if(strcasecmp(str, "debug") == 0) {
        return PT_LOG_DEBUG;
    }
    else {
        return PT_LOG_INVALID;
    }
}

void pt_log(enum pt_log_level level, const char *fmt, ...) {
    if(pt_log_level_at_least(level, global_log_level)) {
        va_list args;
        va_start(args, fmt);
        vfprintf(stderr, fmt, args);
        fputc('\n', stderr);
        va_end(args);
    }
}
