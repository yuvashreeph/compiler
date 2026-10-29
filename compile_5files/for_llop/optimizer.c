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
    TAC temp_tac[100];
    int temp_count = 0;

    // 1. Constant Folding & Strength Reduction Pass
    for (int i = 0; i < in_count; i++) {
        TAC current = input[i];

        if (strcmp(current.op, "label") == 0 || strcmp(current.op, "goto") == 0 || 
            strcmp(current.op, "ifFalse") == 0 || strcmp(current.op, "<") == 0 || 
            strcmp(current.op, ">") == 0 || strcmp(current.op, "<=") == 0 || 
            strcmp(current.op, ">=") == 0 || strcmp(current.op, "==") == 0 || 
            strcmp(current.op, "!=") == 0) {
            temp_tac[temp_count++] = current;
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

        temp_tac[temp_count++] = current;
    }

    // 2. Loop-Invariant Code Motion (LICM)
    // Identifies invariant statements inside loops and hoists them before the loop start.
    int loop_start_idx = -1;
    for (int i = 0; i < temp_count; i++) {
        if (strcmp(temp_tac[i].op, "label") == 0) {
            loop_start_idx = i;
            break;
        }
    }

    if (loop_start_idx != -1) {
        // Find modified variables inside the loop body
        char modified_vars[50][32];
        int mod_count = 0;

        for (int i = loop_start_idx; i < temp_count; i++) {
            if (strlen(temp_tac[i].res) > 0 && strcmp(temp_tac[i].op, "label") != 0 && strcmp(temp_tac[i].op, "goto") != 0 && strcmp(temp_tac[i].op, "ifFalse") != 0) {
                // Check if already recorded
                int exists = 0;
                for (int m = 0; m < mod_count; m++) {
                    if (strcmp(modified_vars[m], temp_tac[i].res) == 0) exists = 1;
                }
                if (!exists) {
                    strcpy(modified_vars[mod_count++], temp_tac[i].res);
                }
            }
        }

        // Identify invariant statements (operands not modified in loop)
        TAC invariants[20];
        int inv_count = 0;
        int keep_indices[100];
        int keep_count = 0;

        for (int i = 0; i < temp_count; i++) {
            TAC t = temp_tac[i];
            int is_inside_loop = (i >= loop_start_idx);
            int is_assign = (strcmp(t.op, "=") == 0 || strcmp(t.op, "+") == 0 || strcmp(t.op, "-") == 0 || strcmp(t.op, "*") == 0 || strcmp(t.op, "<<") == 0);

            if (is_inside_loop && is_assign) {
                int arg1_mod = 0, arg2_mod = 0;
                if (strlen(t.arg1) > 0 && !is_number(t.arg1)) {
                    for (int m = 0; m < mod_count; m++) {
                        if (strcmp(modified_vars[m], t.arg1) == 0) arg1_mod = 1;
                    }
                }
                if (strlen(t.arg2) > 0 && !is_number(t.arg2)) {
                    for (int m = 0; m < mod_count; m++) {
                        if (strcmp(modified_vars[m], t.arg2) == 0) arg2_mod = 1;
                    }
                }

                // If neither operand is modified in the loop, it's invariant!
                if (!arg1_mod && !arg2_mod) {
                    invariants[inv_count++] = t;
                    continue; // Skip keeping it inside the loop
                }
            }
            keep_indices[keep_count++] = i;
        }

        // Reconstruct optimized TAC with hoisted invariants before loop start
        *out_count = 0;
        for (int i = 0; i < temp_count; i++) {
            if (i == loop_start_idx) {
                // Insert invariants right before loop start
                for (int inv = 0; inv < inv_count; inv++) {
                    output[(*out_count)++] = invariants[inv];
                }
            }
            // Copy kept instruction
            for (int k = 0; k < keep_count; k++) {
                if (keep_indices[k] == i) {
                    output[(*out_count)++] = temp_tac[i];
                    break;
                }
            }
        }
    } else {
        // No loop found, copy directly
        *out_count = temp_count;
        for (int i = 0; i < temp_count; i++) output[i] = temp_tac[i];
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