%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "ir.h"

int yylex(void);
void yyerror(const char *s);
extern FILE *yyin;

static void emitf(const char *format, ...)
{
    char buffer[512];
    va_list args;

    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    emit(buffer);
}

static char *duplicate_string(const char *s)
{
    size_t length = strlen(s) + 1;
    char *copy = malloc(length);

    if (copy == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    memcpy(copy, s, length);
    return copy;
}

static char *make_binary(const char *left,
                         const char *op,
                         const char *right)
{
    char *temp = new_temp();
    emitf("%s = %s %s %s", temp, left, op, right);
    return temp;
}
%}

%union {
    char *str;
}

%token <str> ID NUMBER

%token WHILE FOR IF ELSE
%token INT FLOAT DOUBLE CHAR VOID RETURN
%token INC DEC LE GE EQ NE AND OR INVALID

%type <str> type declaration declarator_list
%type <str> expression condition assignment
%type <str> for_init for_update
%type <str> while_start while_test
%type <str> for_start for_test
%type <str> if_prefix

%left OR
%left AND
%left EQ NE
%left '<' '>' LE GE
%left '+' '-'
%left '*' '/'
%right UMINUS

%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE

%%

/* ================= PROGRAM ================= */

program
    : external_list
    ;

external_list
    : external_list external
    | /* empty */
    ;

external
    : function_definition
    | statement
    ;

function_definition
    : type ID '(' ')' compound_statement
    | type ID '(' parameter_list ')' compound_statement
    ;

parameter_list
    : type ID
    | parameter_list ',' type ID
    ;

/* ================= DATA TYPES ================= */

type
    : INT       { $$ = "int"; }
    | FLOAT     { $$ = "float"; }
    | DOUBLE    { $$ = "double"; }
    | CHAR      { $$ = "char"; }
    | VOID      { $$ = "void"; }
    ;

/* ================= COMPOUND STATEMENTS ================= */

compound_statement
    : '{' statement_list '}'
    ;

statement_list
    : statement_list statement
    | /* empty */
    ;

statement
    : declaration ';'
    | assignment ';'
    | expression ';'
    | compound_statement
    | while_statement
    | for_statement
    | if_statement
    | RETURN expression ';'
        {
            emitf("return %s", $2);
        }
    | RETURN ';'
        {
            emit("return");
        }
    | ';'
    ;

/* ================= DECLARATIONS ================= */

declaration
    : type declarator_list
        {
            $$ = "";
        }
    ;

declarator_list
    : ID
        {
            $$ = "";
        }
    | declarator_list ',' ID
        {
            $$ = "";
        }
    | ID '=' expression
        {
            emitf("%s = %s", $1, $3);
            $$ = "";
        }
    | declarator_list ',' ID '=' expression
        {
            emitf("%s = %s", $3, $5);
            $$ = "";
        }
    ;

/* ================= ASSIGNMENT ================= */

assignment
    : ID '=' expression
        {
            emitf("%s = %s", $1, $3);
            $$ = $1;
        }
    ;

/* ================= WHILE LOOP ================= */

while_statement
    : WHILE '(' while_start while_test ')' statement
        {
            emitf("goto %s", $3);
            emit_label($4);
        }
    ;

while_start
    : /* empty */
        {
            $$ = new_label();
            emit_label($$);
        }
    ;

while_test
    : condition
        {
            $$ = new_label();
            emitf("ifFalse %s goto %s", $1, $$);
        }
    ;

/* ================= FOR LOOP ================= */

for_statement
    : FOR '(' for_init ';' for_start for_test ';'
      for_update ')' statement
        {
            if ($8 != NULL && strcmp($8, "") != 0) {
                size_t len = strlen($8);

                if (len >= 3 &&
                    strcmp($8 + len - 2, "++") == 0) {
                    char *name = duplicate_string($8);
                    name[len - 2] = '\0';

                    emitf("%s = %s + 1", name, name);
                    free(name);
                }
                else if (len >= 3 &&
                         strcmp($8 + len - 2, "--") == 0) {
                    char *name = duplicate_string($8);
                    name[len - 2] = '\0';

                    emitf("%s = %s - 1", name, name);
                    free(name);
                }
                else {
                    emit($8);
                }
            }

            emitf("goto %s", $5);
            emit_label($6);
        }
    ;

for_start
    : /* empty */
        {
            $$ = new_label();
            emit_label($$);
        }
    ;

for_test
    : condition
        {
            $$ = new_label();
            emitf("ifFalse %s goto %s", $1, $$);
        }
    ;

for_init
    : assignment
        {
            $$ = "";
        }
    | declaration
        {
            $$ = "";
        }
    | /* empty */
        {
            $$ = "";
        }
    ;

for_update
    : ID INC
        {
            size_t len = strlen($1) + 3;
            $$ = malloc(len);

            if ($$ == NULL) {
                perror("malloc");
                exit(EXIT_FAILURE);
            }

            snprintf($$, len, "%s++", $1);
        }
    | ID DEC
        {
            size_t len = strlen($1) + 3;
            $$ = malloc(len);

            if ($$ == NULL) {
                perror("malloc");
                exit(EXIT_FAILURE);
            }

            snprintf($$, len, "%s--", $1);
        }
    | ID '=' expression
        {
            size_t len = strlen($1) + strlen($3) + 4;
            $$ = malloc(len);

            if ($$ == NULL) {
                perror("malloc");
                exit(EXIT_FAILURE);
            }

            snprintf($$, len, "%s = %s", $1, $3);
        }
    | /* empty */
        {
            $$ = "";
        }
    ;

/* ================= IF AND IF-ELSE ================= */

if_prefix
    : IF '(' condition ')'
        {
            $$ = new_label();
            emitf("ifFalse %s goto %s", $3, $$);
        }
    ;

if_statement
    : if_prefix statement %prec LOWER_THAN_ELSE
        {
            emit_label($1);
        }
    | if_prefix statement ELSE
        {
            $<str>$ = new_label();
            emitf("goto %s", $<str>$);
            emit_label($1);
        }
      statement
        {
            emit_label($<str>4);
        }
    ;

/* ================= CONDITIONS ================= */

condition
    : expression '<' expression
        {
            $$ = make_binary($1, "<", $3);
        }
    | expression '>' expression
        {
            $$ = make_binary($1, ">", $3);
        }
    | expression LE expression
        {
            $$ = make_binary($1, "<=", $3);
        }
    | expression GE expression
        {
            $$ = make_binary($1, ">=", $3);
        }
    | expression EQ expression
        {
            $$ = make_binary($1, "==", $3);
        }
    | expression NE expression
        {
            $$ = make_binary($1, "!=", $3);
        }
    | condition AND condition
        {
            $$ = make_binary($1, "&&", $3);
        }
    | condition OR condition
        {
            $$ = make_binary($1, "||", $3);
        }
    | expression
        {
            $$ = $1;
        }
    ;

/* ================= ARITHMETIC EXPRESSIONS ================= */

expression
    : expression '+' expression
        {
            $$ = make_binary($1, "+", $3);
        }
    | expression '-' expression
        {
            $$ = make_binary($1, "-", $3);
        }
    | expression '*' expression
        {
            $$ = make_binary($1, "*", $3);
        }
    | expression '/' expression
        {
            $$ = make_binary($1, "/", $3);
        }
    | '-' expression %prec UMINUS
        {
            $$ = make_binary("0", "-", $2);
        }
    | '(' expression ')'
        {
            $$ = $2;
        }
    | ID
        {
            $$ = $1;
        }
    | NUMBER
        {
            $$ = $1;
        }
    ;

%%

/* ================= ERROR HANDLING ================= */

void yyerror(const char *s)
{
    fprintf(stderr, "SYNTAX ERROR: %s\n", s);
}

/* ================= MAIN FUNCTION ================= */

int main(int argc, char *argv[])
{
    FILE *input_file = NULL;

    if (argc > 1) {
        input_file = fopen(argv[1], "r");

        if (input_file == NULL) {
            perror("Unable to open input file");
            return EXIT_FAILURE;
        }

        yyin = input_file;
    }

    printf("\n=== PHASE 1: LEXICAL ANALYSIS ===\n");
    printf("Reading source program...\n\n");
    printf("%-8s %-16s %s\n", "LINE", "TOKEN", "LEXEME");

    if (yyparse() != 0) {
        printf("\nSyntax analysis failed.\n");

        if (input_file != NULL) {
            fclose(input_file);
        }

        return EXIT_FAILURE;
    }

    if (input_file != NULL) {
        fclose(input_file);
    }

    printf("\n=== PHASE 2: SYNTAX ANALYSIS ===\n");
    printf("Syntax analysis: SUCCESS\n");

    printf("\n=== PHASE 3: THREE-ADDRESS CODE ===\n");
    print_ir();

    printf("\n=== PHASE 4: OPTIMIZATION ===\n");
    optimize_code();
    print_ir();

    printf("\n=== PHASE 5: TARGET CODE (PSEUDO ASSEMBLY) ===\n");
    generate_target_code();

    printf("\n=== PHASE 6: INTEGRATION ===\n");
    printf("Lexer: completed\n");
    printf("Parser: passed\n");
    printf("TAC: generated\n");
    printf("Basic optimization: completed\n");
    printf("Pseudo target code: generated\n");

    return EXIT_SUCCESS;
}