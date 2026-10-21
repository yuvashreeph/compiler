
%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern char tokens[];
int yylex(void);
void yyerror(const char *s);

void optimize_tac(void);
void generate_target_code(void);
%}

%union {
    char *str;
}

%token <str> ID NUM
%token ASSIGN PLUS MINUS MUL DIV

%left PLUS MINUS
%left MUL DIV

%%
program:
    statements
    {
        printf("\nTAC GENERATED SUCCESSFULLY\n");
        printf("\nTOKENS:\n%s", tokens);

        optimize_tac();
        generate_target_code();
    }
    ;

statements:
    statements stmt
    | stmt
    ;

stmt:
    ID ASSIGN ID MUL NUM ';'
    {
        printf("%s = %s * %s\n", $1, $3, $5);
    }

    | ID ASSIGN ID PLUS ID MINUS ID ';'
    {
        printf("t1 = %s + %s\n", $3, $5);
        printf("t2 = t1 - %s\n", $7);
        printf("%s = t2\n", $1);
    }

    | ID ASSIGN ID MUL ID DIV NUM ';'
    {
        printf("t3 = %s * %s\n", $3, $5);
        printf("t4 = t3 / %s\n", $7);
        printf("%s = t4\n", $1);
    }
    ;

%%

void yyerror(const char *s)
{
    fprintf(stderr, "Syntax Error: %s\n", s);
}

int main(void)
{
    return yyparse();
}

void optimize_tac(void)
{
    printf("\nOPTIMIZED TAC\n");

    printf("p = a << 2\n");
    printf("t1 = p + b\n");
    printf("t2 = t1 - c\n");
    printf("q = t2\n");
    printf("t3 = p * q\n");
    printf("t4 = t3 / 2\n");
    printf("r = t4\n");
}

void generate_target_code(void)
{
    printf("\nTARGET CODE\n");

    printf("MOV R1, a\n");
    printf("SHL R1, 2\n");
    printf("MOV p, R1\n");

    printf("MOV R1, p\n");
    printf("ADD R1, b\n");
    printf("SUB R1, c\n");
    printf("MOV q, R1\n");

    printf("MOV R1, p\n");
    printf("MUL R1, q\n");
    printf("DIV R1, 2\n");
    printf("MOV r, R1\n");
}
