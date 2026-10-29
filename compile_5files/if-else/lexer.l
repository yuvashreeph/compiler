%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "y.tab.h"
%}

%option noyywrap

%%

"if"                   { printf("TOKEN: IF        | Lexeme: %s\n", yytext); return IF; }
"else"                 { printf("TOKEN: ELSE      | Lexeme: %s\n", yytext); return ELSE; }
[0-9]+                 { yylval.str = strdup(yytext); printf("TOKEN: NUMBER    | Value:  %s\n", yytext); return NUMBER; }
[a-zA-Z_][a-zA-Z0-9_]* { yylval.str = strdup(yytext); printf("TOKEN: ID        | Value:  %s\n", yytext); return ID; }
"<="                   { printf("TOKEN: LE        | Lexeme: %s\n", yytext); return LE; }
">="                   { printf("TOKEN: GE        | Lexeme: %s\n", yytext); return GE; }
"=="                   { printf("TOKEN: EQ        | Lexeme: %s\n", yytext); return EQ; }
"!="                   { printf("TOKEN: NE        | Lexeme: %s\n", yytext); return NE; }
"<"                    { printf("TOKEN: LT        | Lexeme: %s\n", yytext); return LT; }
">"                    { printf("TOKEN: GT        | Lexeme: %s\n", yytext); return GT; }
"="                    { printf("TOKEN: ASSIGN    | Lexeme: %s\n", yytext); return ASSIGN; }
"+"                    { printf("TOKEN: PLUS      | Lexeme: %s\n", yytext); return PLUS; }
"-"                    { printf("TOKEN: MINUS     | Lexeme: %s\n", yytext); return MINUS; }
"*"                    { printf("TOKEN: MULT      | Lexeme: %s\n", yytext); return MULT; }
"/"                    { printf("TOKEN: DIV       | Lexeme: %s\n", yytext); return DIV; }
"("                    { printf("TOKEN: LPAREN    | Lexeme: %s\n", yytext); return LPAREN; }
")"                    { printf("TOKEN: RPAREN    | Lexeme: %s\n", yytext); return RPAREN; }
"{"                    { printf("TOKEN: LBRACE    | Lexeme: %s\n", yytext); return LBRACE; }
"}"                    { printf("TOKEN: RBRACE    | Lexeme: %s\n", yytext); return RBRACE; }
";"                    { printf("TOKEN: SEMI      | Lexeme: %s\n", yytext); return SEMI; }
[ \t\n\r]+             { /* Ignore whitespace */ }
.                      { printf("Lexical Error: Unknown character %s\n", yytext); }

%%