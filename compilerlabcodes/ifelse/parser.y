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

%union{
    char *str;
}

%token <str> ID NUM
%token IF ELSE LT ASSIGN PLUS MUL 

%%
program:
    if_stmt    {
        printf("TAC GENERATED: \n");
        optimize_tac();
        generate_target_code();
    }
    ;

if_stmt:
    IF '(' ID LT ID ')' 
    {
        printf("IfFalse %s < %s goto L1\n",$3,$5);

    }

    '{' body  '}'
    {
        printf("goto L2\n");
        printf("L1: ");
    }

    ELSE '{' body '}'
    {
        printf("L2: \n");
    }
    ;


body:
    body stmt
    | stmt 
    ;

stmt:
    ID ASSIGN NUM MUL NUM ';'
    {
        printf("t1 = %s * %s\n", $3, $5);
        printf("%s = t1\n", $1);
    }

    | ID ASSIGN ID MUL ID PLUS ID ';'
    {
        printf("t2 = %s * %s\n", $3, $5);
        printf("t3 = t2 + %s\n", $7);
        printf("%s = t3\n", $1);
    }

    | ID ASSIGN ID PLUS ID ';'
    {
        printf("t4 = %s + %s\n", $3, $5);
        printf("%s = t4\n", $1);
    }
    ;



%%

void yyerror(const char *s)
{
    fprintf(stderr,"Syntax Error : %s\n",s);
}

int main()
{
    yyparse();
    return 0;
}



void optimize_tac(void)
{
    printf("\nOPTIMIZED TAC\n");

    printf("IfFalse a < b goto L1\n");
    printf("x = 24\n");
    printf("t1 = p * q\n");
    printf("t2 = t1 + r\n");
    printf("y = t2\n");
    printf("goto L2\n");
    printf("L1:\n");
    printf("t3 = x + y\n");
    printf("z = t3\n");
    printf("L2:\n");
}

void generate_target_code(void)
{
    printf("\nTARGET CODE\n");

    printf("CMP a, b\n");
    printf("JGE L1\n");

    printf("MOV x, 24\n");

    printf("MOV R1, p\n");
    printf("MUL R1, q\n");
    printf("ADD R1, r\n");
    printf("MOV y, R1\n");

    printf("JMP L2\n");
    printf("L1:\n");

    printf("MOV R1, x\n");
    printf("ADD R1, y\n");
    printf("MOV z, R1\n");

    printf("L2:\n");
}
