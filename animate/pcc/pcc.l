%{
// p-code compile
#include <stdio.h>
#include <stdlib.h>
#include "pcc.h"
#include "y.tab.h"

%}

%%

"declear" { return DECLEAR; }
"native"  { return NATIVE; }
"method"  { return METHOD; }
"end"	  { return END; }
"ins"     { return INS; }
"var"     { return VAR; }
[_A-Za-z][_A-Za-z0-9]*  { yylval.sym = strdup(yytext);  return ID; }
[0-9]+[DdOoBbHh]?  { yylval.number = trans_num(yytext, yyleng); return NUM; }
;[^\n]*\n	;
^\n 	|
^[\t ][\t ]*\n 	; 
[:,]     { return yytext[0]; }
\n		  { yylineno ++; return yytext[0]; }

%%

int yywrap(void)
{
	return 1;
}

