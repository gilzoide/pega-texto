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

int pt_dump_bytecode_from_file(FILE *file) {
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
			default: break;
		}
		printf("\n");
    }
    return 0;
}
