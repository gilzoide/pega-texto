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

/** @file compiler_log.h
 * Compiler logger.
 */

#ifndef __PEGA_TEXTO_COMPILER_LOG_H__
#define __PEGA_TEXTO_COMPILER_LOG_H__

enum pt_compiler_log_level {
    LOG_NONE,
    LOG_ERROR,
    LOG_WARNING,
    LOG_DEBUG,
};

#define pt_log_level_at_least(level, minlevel) \
    (level <= minlevel)

void pt_compiler_set_log_level(enum pt_compiler_log_level level);
void pt_compiler_log(enum pt_compiler_log_level level, const char *fmt, ...);

#endif
