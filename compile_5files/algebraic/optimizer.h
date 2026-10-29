#ifndef OPTIMIZER_H
#define OPTIMIZER_H

typedef struct {
    char op[10];
    char arg1[32];
    char arg2[32];
    char res[32];
} TAC;

void optimize_code(TAC input_tac[], int in_count, TAC output_tac[], int *out_count);
void print_tac(TAC tac_list[], int count);

#endif