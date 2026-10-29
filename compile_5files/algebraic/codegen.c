#include <stdio.h>
#include <string.h>
#include "codegen.h"

void generate_target_code(TAC tac_list[], int count) {
    printf("\n=========================================\n");
    printf("        TARGET ASSEMBLY CODE             \n");
    printf("=========================================\n");

    for (int i = 0; i < count; i++) {
        TAC t = tac_list[i];

        if (strcmp(t.op, "=") == 0) {
            printf("    MOV R0, %s\n", t.arg1);
            printf("    MOV %s, R0\n", t.res);
        }
        else if (strcmp(t.op, "+") == 0) {
            printf("    MOV R0, %s\n", t.arg1);
            printf("    ADD R0, %s\n", t.arg2);
            printf("    MOV %s, R0\n", t.res);
        }
        else if (strcmp(t.op, "-") == 0) {
            printf("    MOV R0, %s\n", t.arg1);
            printf("    SUB R0, %s\n", t.arg2);
            printf("    MOV %s, R0\n", t.res);
        }
        else if (strcmp(t.op, "*") == 0) {
            printf("    MOV R0, %s\n", t.arg1);
            printf("    MUL R0, %s\n", t.arg2);
            printf("    MOV %s, R0\n", t.res);
        }
        else if (strcmp(t.op, "/") == 0) {
            printf("    MOV R0, %s\n", t.arg1);
            printf("    DIV R0, %s\n", t.arg2);
            printf("    MOV %s, R0\n", t.res);
        }
        else if (strcmp(t.op, "<<") == 0) {
            printf("    MOV R0, %s\n", t.arg1);
            printf("    SHL R0, %s\n", t.arg2);
            printf("    MOV %s, R0\n", t.res);
        }
        else if (strcmp(t.op, ">>") == 0) {
            printf("    MOV R0, %s\n", t.arg1);
            printf("    SHR R0, %s\n", t.arg2);
            printf("    MOV %s, R0\n", t.res);
        }
    }
}