%{
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

extern int yylex();
extern int yyparse();
extern FILE *yyin;
 
void yyerror(const char *s);
%}

%union {
	int ival;
	float fval;
	char *sval;
}

%token <ival> INT
%token <fval> FLOAT
%token <sval> STRING

%%

snazzle:
	snazzle INT      { printf("bison found an int: %d\n", $2); }
	| snazzle FLOAT  { printf("bison found a float: %f\n", $2); }
	| snazzle STRING { printf("bison found a string: %s\n", $2); free($2); }
	| INT            { printf("bison found an int: %d\n", $1); }
	| FLOAT          { printf("bison found a float: %f\n", $1); }
	| STRING         { printf("bison found a string: %s\n", $1); free($1); }
	;

%%

#include "benchmark.h"
int main(int argc, char **argv) {
	FILE *input = fopen(argc > 1 ? argv[1] : "grammar.txt", "r");
	if(!input) {
		fprintf(stderr, "Input read error: %s", strerror(errno));
		return -1;
	}

	yyin = input;
	BENCHMARK(while(yyparse()))
	/* fclose(input); */
	return 0;
}

void yyerror(const char *s) {
	fprintf(stderr, "Parse error: %s", s);
	exit(-1);
}

