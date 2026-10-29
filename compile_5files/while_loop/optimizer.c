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

void optimize_code(TAC input[], int in_count, TAC output[], int *out_count) {
    TAC temp_tac[100];
    int temp_count = 0;

    // Pass 1: Constant Folding
    for (int i = 0; i < in_count; i++) {
        TAC current = input[i];
        if (strcmp(current.op, "*") == 0 && is_number(current.arg1) && is_number(current.arg2)) {
            int val = atoi(current.arg1) * atoi(current.arg2);
            strcpy(current.op, "=");
            sprintf(current.arg1, "%d", val);
            strcpy(current.arg2, "");
        }
        else if (strcmp(current.op, "+") == 0 && is_number(current.arg1) && is_number(current.arg2)) {
            int val = atoi(current.arg1) + atoi(current.arg2);
            strcpy(current.op, "=");
            sprintf(current.arg1, "%d", val);
            strcpy(current.arg2, "");
        }
        temp_tac[temp_count++] = current;
    }

    // Pass 2: Loop Invariant Code Motion (Hoisting statements out of loop)
    // Identify loop region between first label and jump
    int loop_start_idx = -1;
    int loop_end_idx = -1;

    for (int i = 0; i < temp_count; i++) {
        if (strcmp(temp_tac[i].op, "label") == 0 && loop_start_idx == -1) {
            loop_start_idx = i;
        }
    }

    *out_count = 0;

    // Collect invariant instructions
    TAC invariants[20];
    int inv_count = 0;

    TAC loop_body[100];
    int body_count = 0;

    // Identifiers modified inside the loop
    char modified_vars[20][32];
    int mod_count = 0;

    // First pass over loop to find modified variables
    for (int i = loop_start_idx; i < temp_count; i++) {
        if (strcmp(temp_tac[i].op, "=") == 0) {
            strcpy(modified_vars[mod_count++], temp_tac[i].res);
        }
    }

    // Determine loop invariant instructions
    for (int i = 0; i < temp_count; i++) {
        if (i < loop_start_idx) {
            output[(*out_count)++] = temp_tac[i];
        } else {
            // Check if instruction inside loop is invariant
            int is_invariant = 0;

            // Check constant expressions inside loop
            if (strcmp(temp_tac[i].op, "=") == 0 && is_number(temp_tac[i].arg1)) {
                is_invariant = 1;
            } else if (strcmp(temp_tac[i].op, "*") == 0) {
                // If operands are not modified inside loop
                int arg1_mod = 0, arg2_mod = 0;
                for (int m = 0; m < mod_count; m++) {
                    if (strcmp(temp_tac[i].arg1, modified_vars[m]) == 0) arg1_mod = 1;
                    if (strcmp(temp_tac[i].arg2, modified_vars[m]) == 0) arg2_mod = 1;
                }
                if (!arg1_mod && !arg2_mod) is_invariant = 1;
            }

            if (is_invariant) {
                invariants[inv_count++] = temp_tac[i];
            } else {
                loop_body[body_count++] = temp_tac[i];
            }
        }
    }

    // Place loop invariant instructions before loop label
    for (int i = 0; i < inv_count; i++) {
        output[(*out_count)++] = invariants[i];
    }

    // Append remaining loop instructions
    for (int i = 0; i < body_count; i++) {
        output[(*out_count)++] = loop_body[i];
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