%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "y.tab.h"
%}

%option noyywrap

%%

[0-9]+                 { yylval.str = strdup(yytext); printf("TOKEN: NUMBER    | Value:  %s\n", yytext); return NUMBER; }
[a-zA-Z_][a-zA-Z0-9_]* { yylval.str = strdup(yytext); printf("TOKEN: ID        | Value:  %s\n", yytext); return ID; }
"="                    { printf("TOKEN: ASSIGN    | Lexeme: %s\n", yytext); return ASSIGN; }
"+"                    { printf("TOKEN: PLUS      | Lexeme: %s\n", yytext); return PLUS; }
"-"                    { printf("TOKEN: MINUS     | Lexeme: %s\n", yytext); return MINUS; }
"*"                    { printf("TOKEN: MULT      | Lexeme: %s\n", yytext); return MULT; }
"/"                    { printf("TOKEN: DIV       | Lexeme: %s\n", yytext); return DIV; }
";"                    { printf("TOKEN: SEMI      | Lexeme: %s\n", yytext); return SEMI; }
[ \t\n\r]+             { /* Ignore whitespace */ }
.                      { printf("Lexical Error: Unknown character %s\n", yytext); }

%%