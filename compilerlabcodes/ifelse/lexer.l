%{
#include "y.tab.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char tokens[1000]=" ";
%}

%%
"if" {return IF; }
"else" {return ELSE;}

[a-zA-Z_][a-zA-Z0-9_]* {yylval.str=strdup(yytext); return ID;}

[0-9]+  {yylval.str=strdup(yytext); return NUM;}

"<"   {return LT;}
"="   {return ASSIGN;}
"+"   {return PLUS;}
"*"    {return MUL;}

"("     {return '(';}
")"     {return ')';}
"{"     {return '{';}
"}"     {return '}';}
";"     {return ';';}

[ \t\r\n]+  ;
.   {printf("Syntax Error : %s\n",yytext); return yytext[0];}


%%

int yywrap(void)
{
    return 1;
}