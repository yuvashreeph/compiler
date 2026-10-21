#include <stdio.h>
#include <string.h>
#include "ir.h"

static const char *comparison_instruction(const char *op)
{
    if (strcmp(op, "<") == 0)  return "CMP_LT";
    if (strcmp(op, ">") == 0)  return "CMP_GT";
    if (strcmp(op, "<=") == 0) return "CMP_LE";
    if (strcmp(op, ">=") == 0) return "CMP_GE";
    if (strcmp(op, "==") == 0) return "CMP_EQ";
    if (strcmp(op, "!=") == 0) return "CMP_NE";

    return NULL;
}

void generate_target_code(void)
{
    for (Instruction *p = ir_head; p != NULL; p = p->next) {
        char lhs[128], a[128], op[16], b[128], label[128];
        char extra;

        /* Labels */
        if (strchr(p->text, ':') != NULL &&
            sscanf(p->text, "%127[^:]:", label) == 1) {
            printf("%s:\n", label);
            continue;
        }

        /* Unconditional jump */
        if (sscanf(p->text, "goto %127s", label) == 1) {
            printf("JMP %s\n", label);
            continue;
        }

        /* Conditional jump */
        if (sscanf(p->text, "ifFalse %127s goto %127s",
                   a, label) == 2) {
            printf("CMP %s, 0\n", a);
            printf("JE %s\n", label);
            continue;
        }

        if (sscanf(p->text, "if %127s %15s %127s goto %127s",
                   a, op, b, label) == 4) {
            const char *instruction = comparison_instruction(op);

            if (instruction != NULL) {
                printf("%s %s, %s\n", instruction, a, b);
                printf("JMP_IF_TRUE %s\n", label);
            } else {
                printf("; Unsupported condition: %s\n", p->text);
            }
            continue;
        }

        /* Return with a value */
        if (sscanf(p->text, "return %127s", a) == 1) {
            printf("RET %s\n", a);
            continue;
        }

        /* Return without a value */
        if (strcmp(p->text, "return") == 0) {
            printf("RET\n");
            continue;
        }

        /* Binary operations and comparisons */
        if (sscanf(p->text, "%127s = %127s %15s %127s %c",
                   lhs, a, op, b, &extra) == 4) {

            const char *instruction = NULL;

            if (strcmp(op, "+") == 0) instruction = "ADD";
            else if (strcmp(op, "-") == 0) instruction = "SUB";
            else if (strcmp(op, "*") == 0) instruction = "MUL";
            else if (strcmp(op, "/") == 0) instruction = "DIV";

            if (instruction != NULL) {
                printf("MOV R0, %s\n", a);
                printf("%s R0, %s\n", instruction, b);
                printf("MOV %s, R0\n", lhs);
                continue;
            }

            instruction = comparison_instruction(op);

            if (instruction != NULL) {
                printf("%s %s, %s\n", instruction, a, b);
                printf("MOV %s, FLAGS\n", lhs);
                continue;
            }
        }

        /* Simple assignment */
        if (sscanf(p->text, "%127s = %127s %c",
                   lhs, a, &extra) == 2) {
            printf("MOV %s, %s\n", lhs, a);
            continue;
        }

        printf("; Unhandled TAC: %s\n", p->text);
    }
}