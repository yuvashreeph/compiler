%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int yylex(void);
void yyerror(const char *s);
%}

%union{
    char *str;
}

%token <str> ID NUM
%token WHILE LT ASSIGN PLUS MUL

%%
program:
    while_stmt  { printf("TAC GEnerated\n"); }
    ;

while_stmt:
    WHILE '(' ID LT ID MUL NUM ')'
    {
        printf("L1: \n");
        printf("t1=%s*%s\n",$5,$7);
        printf("t2=%s<t1\n",$3);
        printf("IfFalse t2 goto L2\n");

    }
    '{' body '}'{
        printf("goto L1m,\n");
        printf("L2:");
    }


body:
    body stmt
    | stmt
    ;

stmt:
    ID ASSIGN NUM MUL  NUM ';' {
        printf("t3= %s*%s\n",$3,$5);
        printf("%s = t3\n",$1);
    }

    | ID ASSIGN ID MUL ID PLUS ID ';' {
    printf("t4 = %s * %s\n", $3, $5);
    printf("t5 = t4 + %s\n", $7);
    printf("%s = t5\n", $1);
}

    | ID ASSIGN ID PLUS ID ';'{
        printf("t6 = %s + %s \n",$3,$5);
        printf("%s = t6\n",$1);
    }
    ;



%%

void yyerror(const char *s)
{
    fprintf(stderr,"Syntas Error : %s",s);
}

int main()
{
    yyparse();
    return 0;
}