%option noyywrap yylineno

%{
#include "parser.tab.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char *copy_string(const char *s)
{
    size_t n = strlen(s) + 1;
    char *p = malloc(n);

    if (p == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    memcpy(p, s, n);
    return p;
}

static void print_token(const char *type, const char *lexeme)
{
    printf("%-8d %-16s %s\n", yylineno, type, lexeme);
}
%}

DIGIT       [0-9]
ID          [a-zA-Z_][a-zA-Z0-9_]*
NUMBER      ({DIGIT}+(\.{DIGIT}*)?|\.{DIGIT}+)([eE][+-]?{DIGIT}+)?

%%

"while"     { print_token("KEYWORD", yytext); return WHILE; }
"for"       { print_token("KEYWORD", yytext); return FOR; }
"if"        { print_token("KEYWORD", yytext); return IF; }
"else"      { print_token("KEYWORD", yytext); return ELSE; }

"int"       { print_token("KEYWORD", yytext); return INT; }
"float"     { print_token("KEYWORD", yytext); return FLOAT; }
"double"    { print_token("KEYWORD", yytext); return DOUBLE; }
"char"      { print_token("KEYWORD", yytext); return CHAR; }
"void"      { print_token("KEYWORD", yytext); return VOID; }
"return"    { print_token("KEYWORD", yytext); return RETURN; }

"++"        { print_token("OPERATOR", yytext); return INC; }
"--"        { print_token("OPERATOR", yytext); return DEC; }

"<="        { print_token("OPERATOR", yytext); return LE; }
">="        { print_token("OPERATOR", yytext); return GE; }
"=="        { print_token("OPERATOR", yytext); return EQ; }
"!="        { print_token("OPERATOR", yytext); return NE; }

"&&"        { print_token("OPERATOR", yytext); return AND; }
"||"        { print_token("OPERATOR", yytext); return OR; }

{NUMBER}    {
                print_token("NUMBER", yytext);
                yylval.str = copy_string(yytext);
                return NUMBER;
            }

{ID}        {
                print_token("IDENTIFIER", yytext);
                yylval.str = copy_string(yytext);
                return ID;
            }

[+\-*/%=<>] {
                print_token("OPERATOR", yytext);
                return (unsigned char)yytext[0];
            }

[(){};,]    {
                print_token("SYMBOL", yytext);
                return (unsigned char)yytext[0];
            }

[ \t\r]+    { /* Ignore spaces and tabs. */ }
\n          { /* yylineno is updated automatically. */ }

"//".*      { /* Ignore single-line comments. */ }

"/*"([^*]|\*+[^*/])*\*+"/" {
                /* Ignore block comments. */
            }

.           {
                fprintf(stderr,
                        "LEXICAL ERROR at line %d: unexpected character '%s'\n",
                        yylineno, yytext);
                return INVALID;
            }

<<EOF>>     {
                printf("%-8d %-16s EOF\n", yylineno, "EOF");
                return 0;
            }

%%
