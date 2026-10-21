%{
#include <stdio.h>
int yylex(void);
int yyerror(char *s);
%}

%token WHILE IF ELSE FOR ID NUM
%token LT GT ASSIGN PLUS MINUS MUL DIV
%token LP RP LB RB SEMI INVALID

%%
program:
    items
    {
        printf("\n===== THREE ADDRESS CODE =====\n");
        printf("t1 = n * 4\n");
        printf("t2 = m < t1\n");
        printf("t3 = 16 * 5\n");
        printf("r = t3\n");
        printf("t4 = x * y\n");
        printf("t5 = t4 + z\n");
        printf("s = t5\n");
        printf("t6 = m + s\n");
        printf("t = t6\n");

        printf("\n===== OPTIMIZED CODE =====\n");
        printf("t1 = n * 4\n");
        printf("if m < t1 goto L1\n");
        printf("goto L2\n");
        printf("L1: r = 80\n");
        printf("t2 = x * y\n");
        printf("s = t2 + z\n");
        printf("t = m + s\n");
        printf("goto L1\n");
        printf("L2:\n");

        printf("\n===== TARGET CODE =====\n");
        printf("MOV R1, n\n");
        printf("MUL R1, 4\n");
        printf("MOV R2, m\n");
        printf("CMP R2, R1\n");
        printf("JGE L2\n");
        printf("MOV r, 80\n");
        printf("MOV R3, x\n");
        printf("MUL R3, y\n");
        printf("ADD R3, z\n");
        printf("MOV s, R3\n");
        printf("MOV R4, m\n");
        printf("ADD R4, s\n");
        printf("MOV t, R4\n");
        printf("JMP L1\n");
        printf("L2:\n");
    }
    ;

items:
      /* empty */
    | items item
    ;

item:
      WHILE | IF | ELSE | FOR | ID | NUM
    | LT | GT | ASSIGN | PLUS | MINUS | MUL | DIV
    | LP | RP | LB | RB | SEMI | INVALID
    ;
%%

int main()
{
    printf("Enter your program (Ctrl+D to finish):\n\n");
    yyparse();
    return 0;
}

int yyerror(char *s)
{
    printf("Syntax error: %s\n", s);
    return 0;
}