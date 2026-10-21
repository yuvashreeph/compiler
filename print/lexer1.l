%{
#include "parser1.tab.h"
#include <stdio.h>
#include <string.h>

char tokens[10000] = "";
%}

%%
"while"                     { strcat(tokens, "WHILE: while\n"); return WHILE; }
"if"                        { strcat(tokens, "IF: if\n"); return IF; }
"else"                      { strcat(tokens, "ELSE: else\n"); return ELSE; }
"for"                       { strcat(tokens, "FOR: for\n"); return FOR; }

[a-zA-Z_][a-zA-Z0-9_]*      { strcat(tokens, "ID: "); strcat(tokens, yytext); strcat(tokens, "\n"); return ID; }
[0-9]+                      { strcat(tokens, "NUM: "); strcat(tokens, yytext); strcat(tokens, "\n"); return NUM; }

"<"                         { strcat(tokens, "LT: <\n"); return LT; }
">"                         { strcat(tokens, "GT: >\n"); return GT; }
"="                         { strcat(tokens, "ASSIGN: =\n"); return ASSIGN; }
"+"                         { strcat(tokens, "PLUS: +\n"); return PLUS; }
"-"                         { strcat(tokens, "MINUS: -\n"); return MINUS; }
"*"                         { strcat(tokens, "MUL: *\n"); return MUL; }
"/"                         { strcat(tokens, "DIV: /\n"); return DIV; }

"("                         { strcat(tokens, "LP: (\n"); return LP; }
")"                         { strcat(tokens, "RP: )\n"); return RP; }
"{"                         { strcat(tokens, "LB: {\n"); return LB; }
"}"                         { strcat(tokens, "RB: }\n"); return RB; }
";"                         { strcat(tokens, "SEMI: ;\n"); return SEMI; }

[ \t\r\n]+                  ;
.                           { strcat(tokens, "INVALID: "); strcat(tokens, yytext); strcat(tokens, "\n"); }
%%

int yywrap()
{
    return 1;
}