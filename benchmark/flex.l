%{
#include <stdio.h>
#include "parser.h"
int linenum;
%}
%option noyywrap

%%

[ \t]           ;
[0-9]+\.[0-9]+  { yylval.fval = atof(yytext); return FLOAT; }
[0-9]+          { yylval.ival = atoi(yytext); return INT; }
[a-zA-Z0-9]+    { yylval.sval = strdup(yytext); return STRING; }
\n              { linenum++; }

%%

