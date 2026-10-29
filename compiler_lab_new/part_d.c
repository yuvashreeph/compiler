#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

typedef struct {
    char op[15], arg1[20], arg2[20], res[20], raw[100];
    int folded;
    int eliminated;
    int hoisted; // Stores the index of the loop label it should be hoisted above (-1 if not hoisted)
} Quad;

Quad q[300];
int n = 0;

int is_number(const char *s) {
    if (!s || *s == '\0') return 0;
    if (*s == '-' || *s == '+') s++;
    if (*s == '\0') return 0;
    while (*s) {
        if (!isdigit((unsigned char)*s)) return 0;
        s++;
    }
    return 1;
}

int is_temp(const char *s) {
    return (s && s[0] == 't' && isdigit((unsigned char)s[1]));
}

void parse_line(char *line) {
    char a[20], b[20], c[20], d[20], e[20];
    int cnt = sscanf(line, "%19s %19s %19s %19s %19s", a, b, c, d, e);
    if (cnt <= 0 || line[0] == '-') return;

    strcpy(q[n].raw, line);
    q[n].folded = 0;
    q[n].eliminated = 0;
    q[n].hoisted = -1;
    q[n].op[0] = q[n].arg1[0] = q[n].arg2[0] = q[n].res[0] = '\0';

    if (a[strlen(a)-1] == ':') {
        strcpy(q[n].op, "label"); a[strlen(a)-1] = '\0'; strcpy(q[n].res, a);
    } else if (strcmp(a, "goto") == 0) {
        strcpy(q[n].op, "goto"); strcpy(q[n].res, b);
    } else if (strcmp(a, "ifFalse") == 0) {
        strcpy(q[n].op, "ifFalse"); strcpy(q[n].arg1, b); strcpy(q[n].res, d);
    } else if (strcmp(a, "return") == 0) {
        strcpy(q[n].op, "return"); strcpy(q[n].arg1, b);
    } else if (cnt == 3 && strcmp(b, "=") == 0) {
        strcpy(q[n].op, "="); strcpy(q[n].res, a); strcpy(q[n].arg1, c);
    } else if (cnt == 4 && strcmp(b, "=") == 0) {
        strcpy(q[n].op, c); strcpy(q[n].res, a); strcpy(q[n].arg1, d);
    } else if (cnt == 5 && strcmp(b, "=") == 0) {
        strcpy(q[n].op, d); strcpy(q[n].res, a); strcpy(q[n].arg1, c); strcpy(q[n].arg2, e);
    }
    n++;
}

// Safely gets constant value ONLY if the variable isn't modified multiple times (e.g. loop counters)
char* get_constant_value(char *var, int current_idx) {
    if (is_number(var)) return var;
    if (var[0] == '\0') return NULL;

    // Count how many times 'var' is assigned a value in the entire program
    int assign_count = 0;
    for (int i = 0; i < n; i++) {
        if (!q[i].eliminated && strcmp(q[i].res, var) == 0 &&
            strcmp(q[i].op, "label") != 0 && strcmp(q[i].op, "goto") != 0 && strcmp(q[i].op, "ifFalse") != 0) {
            assign_count++;
        }
    }
    
    // If modified more than once (like 'z' in a loop), it is NOT a constant!
    if (assign_count > 1) return NULL;

    for (int i = current_idx - 1; i >= 0; i--) {
        if (!q[i].eliminated && strcmp(q[i].res, var) == 0 && strcmp(q[i].op, "=") == 0 && is_number(q[i].arg1)) {
            return q[i].arg1;
        }
    }
    return NULL;
}

// 1. Constant Folding & Propagation Pass
void optimize_constant_folding() {
    for (int i = 0; i < n; i++) {
        if (q[i].eliminated) continue;

        // Binary arithmetic operations (e.g., t2 = 5 * 10)
        if (strcmp(q[i].op, "+") == 0 || strcmp(q[i].op, "-") == 0 ||
            strcmp(q[i].op, "*") == 0 || strcmp(q[i].op, "/") == 0 || strcmp(q[i].op, "%") == 0) {
            
            char *val1 = get_constant_value(q[i].arg1, i);
            char *val2 = get_constant_value(q[i].arg2, i);

            if (val1 && val2) {
                int v1 = atoi(val1);
                int v2 = atoi(val2);
                int res_val = 0, computed = 1;

                if (strcmp(q[i].op, "+") == 0) res_val = v1 + v2;
                else if (strcmp(q[i].op, "-") == 0) res_val = v1 - v2;
                else if (strcmp(q[i].op, "*") == 0) res_val = v1 * v2;
                else if (strcmp(q[i].op, "/") == 0 && v2 != 0) res_val = v1 / v2;
                else if (strcmp(q[i].op, "%") == 0 && v2 != 0) res_val = v1 % v2;
                else computed = 0;

                if (computed) {
                    snprintf(q[i].raw, sizeof(q[i].raw), "%s = %d", q[i].res, res_val);
                    strcpy(q[i].op, "=");
                    snprintf(q[i].arg1, sizeof(q[i].arg1), "%d", res_val);
                    q[i].arg2[0] = '\0';
                    q[i].folded = 1;
                }
            }
        } 
        // Propagate folded temporary constants into direct assignments (e.g., invariant_val = t2 -> invariant_val = 50)
        else if (strcmp(q[i].op, "=") == 0 && is_temp(q[i].arg1)) {
            char *val = get_constant_value(q[i].arg1, i);
            if (val) {
                strcpy(q[i].arg1, val);
                snprintf(q[i].raw, sizeof(q[i].raw), "%s = %s", q[i].res, val);
                q[i].folded = 1;
            }
        }
    }
}

// 2. Loop-Invariant Code Motion (LICM) Pass
void optimize_code_motion() {
    for (int i = 0; i < n; i++) {
        if (strcmp(q[i].op, "label") == 0) {
            int loop_start = i;
            int loop_end = -1;
            
            // Find the backward 'goto' that jumps to this label
            for (int j = i + 1; j < n; j++) {
                if (strcmp(q[j].op, "goto") == 0 && strcmp(q[j].res, q[i].res) == 0) {
                    loop_end = j;
                    break;
                }
            }
            
            if (loop_end != -1) {
                // Find where the loop condition ends ('ifFalse ... goto ...') so we NEVER hoist the loop condition itself
                int body_start = loop_start + 1;
                for (int k = loop_start + 1; k < loop_end; k++) {
                    if (strcmp(q[k].op, "ifFalse") == 0) {
                        body_start = k + 1;
                        break;
                    }
                }

                // Scan loop body for invariant assignments/expressions
                int changed = 1;
                while (changed) {
                    changed = 0;
                    for (int k = body_start; k < loop_end; k++) {
                        if (q[k].eliminated || q[k].hoisted != -1) continue;
                        if (strcmp(q[k].op, "label") == 0 || strcmp(q[k].op, "goto") == 0 || 
                            strcmp(q[k].op, "ifFalse") == 0 || strcmp(q[k].op, "return") == 0) continue;

                        int invariant = 1;

                        // 1. Check if the target variable itself is modified elsewhere in the loop (e.g. z = z + 1)
                        for (int m = loop_start + 1; m < loop_end; m++) {
                            if (m != k && !q[m].eliminated && strcmp(q[m].res, q[k].res) == 0) {
                                invariant = 0;
                            }
                        }

                        // 2. Check if arg1 is modified inside the loop (unless that modification was already hoisted)
                        if (q[k].arg1[0] != '\0' && !is_number(q[k].arg1)) {
                            for (int m = loop_start + 1; m < loop_end; m++) {
                                if (!q[m].eliminated && q[m].hoisted == -1 && strcmp(q[m].res, q[k].arg1) == 0) {
                                    invariant = 0;
                                }
                            }
                        }

                        // 3. Check if arg2 is modified inside the loop
                        if (q[k].arg2[0] != '\0' && !is_number(q[k].arg2)) {
                            for (int m = loop_start + 1; m < loop_end; m++) {
                                if (!q[m].eliminated && q[m].hoisted == -1 && strcmp(q[m].res, q[k].arg2) == 0) {
                                    invariant = 0;
                                }
                            }
                        }

                        if (invariant && q[k].res[0] != '\0') {
                            q[k].hoisted = loop_start; // Hoist right above this loop's label
                            changed = 1;
                        }
                    }
                }
            }
        }
    }
}

// 3. Dead Code Elimination (DCE) Pass
void optimize_dead_code() {
    int changed = 1;
    while (changed) {
        changed = 0;
        for (int i = 0; i < n; i++) {
            if (q[i].eliminated) continue;
            if (strcmp(q[i].op, "label") == 0 || strcmp(q[i].op, "goto") == 0 ||
                strcmp(q[i].op, "ifFalse") == 0 || strcmp(q[i].op, "return") == 0) continue;

            // Keep hoisted user variables (like 'invariant_val = 50') so LICM is visible in output!
            if (q[i].hoisted != -1 && !is_temp(q[i].res)) continue;

            char *target = q[i].res;
            if (target[0] == '\0') continue;

            // Check across the ENTIRE program (0 to n) to account for backward loop jumps
            int referenced = 0;
            for (int j = 0; j < n; j++) {
                if (i != j && !q[j].eliminated) {
                    if (strcmp(q[j].arg1, target) == 0 || strcmp(q[j].arg2, target) == 0) {
                        referenced = 1;
                        break;
                    }
                }
            }

            if (!referenced) {
                q[i].eliminated = 1;
                changed = 1;
            }
        }
    }
}

int main() {
    char line[100];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\r\n")] = 0;
        parse_line(line);
    }

    // Run the 3 optimizations in order
    optimize_constant_folding();
    optimize_code_motion();
    optimize_dead_code();

    printf("--- PART D: OPTIMIZED TAC (CONSTANT FOLDING, LICM & DCE) ---\n");
    for (int i = 0; i < n; i++) {
        // If any instructions inside a loop were hoisted above this label, print them right here!
        for (int j = 0; j < n; j++) {
            if (q[j].hoisted == i && !q[j].eliminated) {
                printf("%s\n", q[j].raw);
            }
        }
        // Print normal instruction if it wasn't eliminated or hoisted elsewhere
        if (!q[i].eliminated && q[i].hoisted == -1) {
            printf("%s\n", q[i].raw);
        }
    }
    return 0;
}