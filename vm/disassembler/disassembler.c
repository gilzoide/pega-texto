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

#include "bytecode.h"
#include "logging.h"

const char *_pt_escape_character(char c, int midstring) {
    static char str[2] = { 0, 0 };
    switch (c) {
        case '\a': return "\\a";
        case '\b': return "\\b";
        case '\f': return "\\f";
        case '\n': return "\\n";
        case '\r': return "\\r";
        case '\t': return "\\t";
        case '\v': return "\\v";
        case '\'': return midstring ? "'" : "\\'";
        case '\"': return midstring ? "\\\"" : "\"";
        case '\\': return "\\\\";
        default:
            str[0] = c;
            return str;
    }
}

#define NEXT_BYTE() \
    b = fgetc(file); count++; if(ferror(file)) return -1;

#define NEXT_SHORT() \
    ((uint8_t *)&b)[0] = fgetc(file); count++; if(ferror(file)) return -1; \
    ((uint8_t *)&b)[1] = fgetc(file); count++; if(ferror(file)) return -2;

#define UNGET_BYTE() \
    ungetc(b, file); count--;

int pt_dump_bytecode_from_file(FILE *file) {
    int version = pt_read_bytecode_version(file);
    if(version == EOF) {
        pt_log(PT_LOG_ERROR, "File doesn't follow pega-texto bytecode format\n");
        return -1;
    }
    else if(version > PT_BYTECODE_VERSION) {
        pt_log(PT_LOG_ERROR, "Bytecode version %d is greater than the supported version %d\n", version, PT_BYTECODE_VERSION);
        return -2;
    }
    printf("ptbc version %d\n", version);
    int instruction, b, count = -1;
    while(!(feof(file) || ferror(file))) {
        NEXT_BYTE();
        if(b == EOF) break;
        instruction = b;
        printf("%4d | 0x%02x | %s", count, instruction, pt_opcode_description[instruction]);
        switch(instruction) {
			case FAIL_LESS_THEN: case ACTION:
				NEXT_BYTE();
				printf(" %d", b);
				break;
			case JUMP_RELATIVE: case JUMP_RELATIVE_IF_FAIL:
				NEXT_BYTE();
				printf(" %d", b);
				break;
			case JUMP: case JUMP_IF_FAIL: case CALL:
				NEXT_SHORT();
				printf(" %d", b);
				break;
			case BYTE: case NOT_BYTE:
				NEXT_BYTE();
				printf(" '%s'", _pt_escape_character(b, 0));
				break;
			case CLASS:
				NEXT_BYTE();
				printf(" \\%c", b);
				break;
			case RANGE:
				NEXT_BYTE();
                printf(" [%c", b);
				NEXT_BYTE();
				printf("-%c]", b);
				break;
			case STRING: case SET:
                printf(" \"");
                while(1) {
                    NEXT_BYTE();
                    if(b == '\0') break;
                    printf("%s", _pt_escape_character(b, 1));
                }
                printf("\"");
				break;
            case RET:
                NEXT_BYTE();
                if(b != PT_BYTECODE_END_AFTER_RET) UNGET_BYTE();
                break;
			default: break;
		}
		printf("\n");
    }
    return 0;
}
