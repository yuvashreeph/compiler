#include <stdio.h>
#include <string.h>
#include "codegen.h"

void generate_target_code(TAC tac_list[], int count) {
    printf("\n=========================================\n");
    printf("        TARGET ASSEMBLY CODE             \n");
    printf("=========================================\n");

    for (int i = 0; i < count; i++) {
        TAC t = tac_list[i];

        if (strcmp(t.op, "label") == 0) {
            printf("%s:\n", t.res);
        }
        else if (strcmp(t.op, "=") == 0) {
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
        else if (strcmp(t.op, "<") == 0) {
            printf("    MOV R0, %s\n", t.arg1);
            printf("    CMP R0, %s\n", t.arg2);
            printf("    SETL %s\n", t.res);
        }
        else if (strcmp(t.op, "ifFalse") == 0) {
            printf("    CMP %s, 0\n", t.arg1);
            printf("    JZ %s\n", t.res);
        }
        else if (strcmp(t.op, "goto") == 0) {
            printf("    JMP %s\n", t.res);
        }
    }
}