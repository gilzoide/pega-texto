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

/** @file logging.h
 * Simple logging.
 */

#ifndef __PEGA_TEXTO_LOGGING_H__
#define __PEGA_TEXTO_LOGGING_H__

enum pt_log_level {
    PT_LOG_INVALID = -1,
    PT_LOG_NONE,
    PT_LOG_ERROR,
    PT_LOG_WARNING,
    PT_LOG_INFO,
    PT_LOG_DEBUG,
};

#define pt_log_level_at_least(level, minlevel) \
    (level <= minlevel)

#define PT_LOG_LEVEL_ENV "PT_LOG_LEVEL"

void pt_set_log_level(enum pt_log_level level);
void pt_set_log_level_from_env();
enum pt_log_level pt_parse_log_level(const char *str);
void pt_log(enum pt_log_level level, const char *fmt, ...);

#endif
