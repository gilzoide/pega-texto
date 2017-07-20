/**
 * Auxiliary library for reading and entire file!
 */
#ifndef __PT_EXAMPLES_READFILE_H__
#define __PT_EXAMPLES_READFILE_H__

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

char *readfile(const char *filename) {
	FILE *fp = fopen(filename, "r");
	assert(fp != NULL && "Error reading file");
	fseek(fp, 0, SEEK_END);
	long size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	char *buffer = malloc((size + 1) * sizeof(char));
	assert(buffer && "[readfile] Couldn't malloc buffer");
	fread(buffer, sizeof(char), size, fp);
	buffer[size] = '\0';
	fclose(fp);
	return buffer;
}

#endif
