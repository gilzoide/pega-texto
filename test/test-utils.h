#ifndef __PEGA_TEXTO_TEST_UTILS_H__
#define __PEGA_TEXTO_TEST_UTILS_H__

#include <pega-texto.h>
#include <pega-texto/macro-on.h>
#include <stdlib.h>
#include <stdio.h>

// Raw
int _Yes(pt_expr **es, const char **names, const char *s) {
	if(pt_match(es, names, s, NULL).matched >= 0) return 1;
	printf("Error: no match for \"%s\"\n", s);
	return 0;
}
int _No(pt_expr **es, const char **names, const char *s) {
	if(pt_match(es, names, s, NULL).matched < 0) return 1;
	printf("Error: there was match for \"%s\"\n", s);
	return 0;
}
int _Res(pt_expr **es, const char **names, const char *s, int expected_output) {
	int res = pt_match(es, names, s, NULL).matched;
	if(res == expected_output) return 1;
	printf("Results don't match for \"%s\": expected %d, found %d\n", s, expected_output, res);
	return 0;
}

// Expressions
int Yes(pt_expr *e, const char *s) {
	if(pt_match_expr(e, s, NULL).matched >= 0) return 1;
	printf("Error: no match for \"%s\"\n", s);
	return 0;
}
int No(pt_expr *e, const char *s) {
	if(pt_match_expr(e, s, NULL).matched < 0) return 1;
	printf("Error: there was match for \"%s\"\n", s);
	return 0;
}
int Res(pt_expr *e, const char *s, int expected_output) {
	int res = pt_match_expr(e, s, NULL).matched;
	if(res == expected_output) return 1;
	printf("Results don't match for \"%s\": expected %d, found %d\n", s, expected_output, res);
	return 0;
}

// Grammar
int gYes(pt_grammar *g, const char *s) {
	if(pt_match_grammar(g, s, NULL).matched >= 0) return 1;
	printf("Error: no match for \"%s\"\n", s);
	return 0;
}
int gNo(pt_grammar *g, const char *s) {
	if(pt_match_grammar(g, s, NULL).matched < 0) return 1;
	printf("Error: there was match for \"%s\"\n", s);
	return 0;
}
int gRes(pt_grammar *g, const char *s, int expected_output) {
	int res = pt_match_grammar(g, s, NULL).matched;
	if(res == expected_output) return 1;
	printf("Results don't match for \"%s\": expected %d, found %d\n", s, expected_output, res);
	return 0;
}

#endif

