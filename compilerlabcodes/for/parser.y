%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int yylex();
void yyerror(const char *s);
extern char tokens [];
%}

%union{
    char *str;
}

%token <str> ID NUM
%token FOR LT MUL PLUS ASSIGN 

%%

program:
    for_stmt{
        printf("The tokens are : %s\n",tokens);
        printf("TAC generated ");
    }

for_stmt:
    FOR '(' ID ASSIGN NUM ';' ID LT ID ';' ID ASSIGN ID PLUS NUM ')'
    {
        printf("%s = %s \n",$3,$5);
        printf("L!:\n");
        printf("IfFalse %s < %s goto L2\n",$7,$9);

    }

    '{'body '}'
    {
        printf("t5 = %s + %s\n",$13,$15);
        printf("%s = t5\n",$11);
        printf("goto L1\n");
        printf("L2:\n" );
    }

    ;


body:
    body stmt
    | stmt 
    ;

stmt:
    ID ASSIGN NUM MUL NUM ';'{
        printf("t1 = %s * %s\n",$3,$5);
        printf("%s = t1\n",$1);
    }

    | ID ASSIGN ID MUL ID PLUS ID ';'{
        printf("t2 = %s * %s \n",$3,$5);
        printf("t3 =  t2 + %s\n ",$7);
        printf("%s = t3\n",$1);
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
    fprintf(stderr,"Syntax Error %s\n",s);
}


int main()
{
    yyparse();
    return 0;
}

