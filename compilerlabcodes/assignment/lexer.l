
%{
#include "y.tab.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char tokens[1000] = "";
%}

%%
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

"="  { return ASSIGN; }
"+"  { return PLUS; }
"-"  { return MINUS; }
"*"  { return MUL; }
"/"  { return DIV; }

"("  { return '('; }
")"  { return ')'; }
";"  { return ';'; }

[ \t\r\n]+ ;

. {
    printf("Syntax Error: %s\n", yytext);
    return yytext[0];
}
%%

int yywrap(void)
{
    return 1;
}
