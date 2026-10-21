%{
#include "parser.tab.h"
#include <stdio.h>
%}

%%
"while"                     { printf("WHILE: %s\n", yytext); }
"if"                        { printf("IF: %s\n", yytext); }
"else"                      { printf("ELSE: %s\n", yytext); }
"for"                       { printf("FOR: %s\n", yytext); }

[a-zA-Z_][a-zA-Z0-9_]*      { printf("ID: %s\n", yytext); }
[0-9]+                      { printf("NUM: %s\n", yytext); }

"<"                         { printf("LT: %s\n", yytext); }
">"                         { printf("GT: %s\n", yytext); }
"="                         { printf("ASSIGN: %s\n", yytext); }
"+"                         { printf("PLUS: %s\n", yytext); }
"-"                         { printf("MINUS: %s\n", yytext); }
"*"                         { printf("MUL: %s\n", yytext); }
"/"                         { printf("DIV: %s\n", yytext); }

"("                         { printf("LP: %s\n", yytext); }
")"                         { printf("RP: %s\n", yytext); }
"{"                         { printf("LB: %s\n", yytext); }
"}"                         { printf("RB: %s\n", yytext); }
";"                         { printf("SEMI: %s\n", yytext); }

[ \t\r\n]+                  ;
.                           { printf("INVALID: %s\n", yytext); }
%%

int yywrap()
{
    return 1;
}