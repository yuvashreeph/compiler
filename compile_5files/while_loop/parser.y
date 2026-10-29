%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void yyerror(const char *s);
int yylex(void);

// Data structure for TAC Instructions
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
int label_count = 1;

char* new_temp() {
    char* t = (char*)malloc(16);
    sprintf(t, "t%d", temp_var_count++);
    return t;
}

char* new_label() {
    char* l = (char*)malloc(16);
    sprintf(l, "L%d", label_count++);
    return l;
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

typedef struct {
    char* label_start;
    char* label_end;
    char* label_body;
} LoopLabels;

%}

%union {
    char* str;
    struct {
        char* name;
    } expr;
    struct {
        char* label_start;
        char* label_end;
        char* label_body;
    } loop_info;
}

%token <str> ID NUMBER
%token WHILE LE GE EQ NE LT GT ASSIGN PLUS MINUS MULT DIV LPAREN RPAREN LBRACE RBRACE SEMI

%type <expr> expr rel_expr
%type <loop_info> while_header

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
    assignment_stmt
    | while_stmt
    ;

assignment_stmt:
    ID ASSIGN expr SEMI {
        emit("=", $3.name, NULL, $1);
    }
    ;

while_stmt:
    while_header LBRACE stmt_list RBRACE {
        emit("goto", NULL, NULL, $1.label_start);
        emit("label", NULL, NULL, $1.label_end);
    }
    ;

while_header:
    WHILE LPAREN rel_expr RPAREN {
        $$.label_start = new_label();
        $$.label_body = new_label();
        $$.label_end = new_label();

        emit("label", NULL, NULL, $$.label_start);
        emit("ifFalse", $3.name, "goto", $$.label_end);
        emit("label", NULL, NULL, $$.label_body);
    }
    ;

rel_expr:
    expr LT expr {
        $$.name = new_temp();
        emit("<", $1.name, $3.name, $$.name);
    }
    | expr GT expr {
        $$.name = new_temp();
        emit(">", $1.name, $3.name, $$.name);
    }
    | expr LE expr {
        $$.name = new_temp();
        emit("<=", $1.name, $3.name, $$.name);
    }
    | expr GE expr {
        $$.name = new_temp();
        emit(">=", $1.name, $3.name, $$.name);
    }
    | expr EQ expr {
        $$.name = new_temp();
        emit("==", $1.name, $3.name, $$.name);
    }
    | expr NE expr {
        $$.name = new_temp();
        emit("!=", $1.name, $3.name, $$.name);
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
    | LPAREN expr RPAREN {
        $$.name = $2.name;
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