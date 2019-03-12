#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "benchmark.h"

extern int yyparse();

int main(int argc, const char **argv) {
	BENCHMARK(while(yyparse()));
	return 0;
}
