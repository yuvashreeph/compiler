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
int label_count = 1;

char update_res[32];
char update_arg1[32];
char update_op[10];
char update_arg2[32];

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

char* label_stack[20];
int top = -1;
%}

%union {
    char* str;
    struct {
        char* name;
    } expr;
}

%token <str> ID NUMBER
%token FOR LE GE EQ NE LT GT ASSIGN PLUS MINUS MULT DIV LPAREN RPAREN LBRACE RBRACE SEMI

%type <expr> expr rel_expr

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
    | for_stmt
    ;

assignment_stmt:
    ID ASSIGN expr SEMI {
        emit("=", $3.name, NULL, $1);
    }
    ;
update_stmt:
    ID ASSIGN expr {
        strcpy(update_res, $1);
        strcpy(update_op, "=");
        strcpy(update_arg1, $3.name);
        strcpy(update_arg2, "");
    }
    ;

for_stmt:
    FOR LPAREN assignment_stmt {
        char* l_start = new_label();
        emit("label", NULL, NULL, l_start);
        label_stack[++top] = l_start;
    } rel_expr SEMI {
        char* l_end = new_label();
        emit("ifFalse", $5.name, "goto", l_end);
        label_stack[++top] = l_end;
    } update_stmt RPAREN LBRACE stmt_list RBRACE {
        char* l_end = label_stack[top--];
        char* l_start = label_stack[top--];
        
        // Emit update statement at the end of the loop body
        emit(update_op, update_arg1, update_arg2, update_res);
        // Jump back to loop start condition check
        emit("goto", NULL, NULL, l_start);
        // Emit loop exit label
        emit("label", NULL, NULL, l_end);
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