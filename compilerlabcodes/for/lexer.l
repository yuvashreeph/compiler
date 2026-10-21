
%{
#include "parser.tab.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char tokens[1000] = "";

%}

%%

"for" {
    strcat(tokens, "FOR : for\n");
    return FOR;
}

[a-zA-Z_][a-zA-Z0-9_]* {
    strcat(tokens, "ID : ");
    strcat(tokens, yytext);
    strcat(tokens, "\n");

    yylval.str = strdup(yytext);
    return ID;
}

[0-9]+ {
    strcat(tokens, "NUM : ");
    strcat(tokens, yytext);
    strcat(tokens, "\n");

    yylval.str = strdup(yytext);
    return NUM;
}

"=" {
    strcat(tokens, "ASSIGN : =\n");
    return ASSIGN;
}

"+" {
    strcat(tokens, "PLUS : +\n");
    return PLUS;
}

"*" {
    strcat(tokens, "MUL : *\n");
    return MUL;
}

"<" {
    strcat(tokens, "LT : <\n");
    return LT;
}

"(" {
    strcat(tokens, "LPAREN : (\n");
    return '(';
}

")" {
    strcat(tokens, "RPAREN : )\n");
    return ')';
}

"{" {
    strcat(tokens, "LBRACE : {\n");
    return '{';
}

"}" {
    strcat(tokens, "RBRACE : }\n");
    return '}';
}

";" {
    strcat(tokens, "SEMICOLON : ;\n");
    return ';';
}

[ \t\r\n]+ ;

. {
    printf("Invalid Syntax: %s\n", yytext);
    return yytext[0];
}

%%

int yywrap(void)
{
    return 1;
}
