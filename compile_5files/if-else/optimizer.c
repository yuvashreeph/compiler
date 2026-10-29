#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "optimizer.h"

int is_number(const char* str) {
    if (!str || *str == '\0') return 0;
    for (int i = 0; str[i] != '\0'; i++) {
        if (!isdigit(str[i])) return 0;
    }
    return 1;
}

int get_power_of_two(int n) {
    if (n <= 0) return -1;
    int p = 0;
    while (n > 1) {
        if (n % 2 != 0) return -1;
        n /= 2;
        p++;
    }
    return p;
}

void optimize_code(TAC input[], int in_count, TAC output[], int *out_count) {
    *out_count = 0;
    for (int i = 0; i < in_count; i++) {
        TAC current = input[i];

        if (strcmp(current.op, "label") == 0 || strcmp(current.op, "goto") == 0 || 
            strcmp(current.op, "ifFalse") == 0 || strcmp(current.op, "<") == 0 || 
            strcmp(current.op, ">") == 0 || strcmp(current.op, "<=") == 0 || 
            strcmp(current.op, ">=") == 0 || strcmp(current.op, "==") == 0 || 
            strcmp(current.op, "!=") == 0) {
            output[(*out_count)++] = current;
            continue;
        }

        // Constant Folding
        if (is_number(current.arg1) && is_number(current.arg2)) {
            int val1 = atoi(current.arg1);
            int val2 = atoi(current.arg2);
            int res_val = 0;
            int folded = 1;

            if (strcmp(current.op, "+") == 0) res_val = val1 + val2;
            else if (strcmp(current.op, "-") == 0) res_val = val1 - val2;
            else if (strcmp(current.op, "*") == 0) res_val = val1 * val2;
            else if (strcmp(current.op, "/") == 0 && val2 != 0) res_val = val1 / val2;
            else folded = 0;

            if (folded) {
                strcpy(current.op, "=");
                sprintf(current.arg1, "%d", res_val);
                strcpy(current.arg2, "");
            }
        }
        // Strength Reduction
        else if (strcmp(current.op, "*") == 0 && is_number(current.arg2)) {
            int val = atoi(current.arg2);
            int p = get_power_of_two(val);
            if (p != -1) {
                strcpy(current.op, "<<");
                sprintf(current.arg2, "%d", p);
            }
        }
        else if (strcmp(current.op, "/") == 0 && is_number(current.arg2)) {
            int val = atoi(current.arg2);
            int p = get_power_of_two(val);
            if (p != -1) {
                strcpy(current.op, ">>");
                sprintf(current.arg2, "%d", p);
            }
        }

        output[(*out_count)++] = current;
    }
}

void print_tac(TAC tac_list[], int count) {
    for (int i = 0; i < count; i++) {
        if (strcmp(tac_list[i].op, "label") == 0) {
            printf("%s:\n", tac_list[i].res);
        } else if (strcmp(tac_list[i].op, "goto") == 0) {
            printf("    goto %s\n", tac_list[i].res);
        } else if (strcmp(tac_list[i].op, "ifFalse") == 0) {
            printf("    ifFalse %s goto %s\n", tac_list[i].arg1, tac_list[i].res);
        } else if (strcmp(tac_list[i].op, "=") == 0) {
            printf("    %s = %s\n", tac_list[i].res, tac_list[i].arg1);
        } else {
            printf("    %s = %s %s %s\n", tac_list[i].res, tac_list[i].arg1, tac_list[i].op, tac_list[i].arg2);
        }
    }
}