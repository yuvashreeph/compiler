%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void yyerror(const char *s);
int yylex(void);

typedef struct {
    char op[10];
    char arg1[32];
    char arg2[32];
    char res[32];
} TAC;

#define MAX_TAC 100
TAC tac_list[MAX_TAC];
int tac_count = 0;
int temp_var_count = 1;

char* new_temp() {
    char* t = (char*)malloc(16);
    sprintf(t, "t%d", temp_var_count++);
    return t;
}

void emit(const char* op, const char* arg1, const char* arg2, const char* res) {
    if (tac_count < MAX_TAC) {
        strcpy(tac_list[tac_count].op, op);
        strcpy(tac_list[tac_count].arg1, arg1 ? arg1 : "");
        strcpy(tac_list[tac_count].arg2, arg2 ? arg2 : "");
        strcpy(tac_list[tac_count].res, res ? res : "");
        tac_count++;
    }
}
%}

%union {
    char* str;
    struct {
        char* name;
    } expr;
}

%token <str> ID NUMBER
%token ASSIGN PLUS MINUS MULT DIV SEMI

%type <expr> expr

%left PLUS MINUS
%left MULT DIV

%%

program:
    stmt_list
    ;

stmt_list:
    stmt_list stmt
    | stmt
    ;

stmt:
    ID ASSIGN expr SEMI {
        emit("=", $3.name, NULL, $1);
    }
    ;

expr:
    expr PLUS expr {
        $$.name = new_temp();
        emit("+", $1.name, $3.name, $$.name);
    }
    | expr MINUS expr {
        $$.name = new_temp();
        emit("-", $1.name, $3.name, $$.name);
    }
    | expr MULT expr {
        $$.name = new_temp();
        emit("*", $1.name, $3.name, $$.name);
    }
    | expr DIV expr {
        $$.name = new_temp();
        emit("/", $1.name, $3.name, $$.name);
    }
    | ID {
        $$.name = $1;
    }
    | NUMBER {
        $$.name = $1;
    }
    ;

%%

void yyerror(const char *s) {
    fprintf(stderr, "Syntax Error: %s\n", s);
}