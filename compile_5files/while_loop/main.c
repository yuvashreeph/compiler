#include <stdio.h>
#include <stdlib.h>
#include "optimizer.h"
#include "codegen.h"

extern int yyparse();
extern FILE* yyin;
extern TAC tac_list[];
extern int tac_count;

int main(int argc, char** argv) {
    if (argc > 1) {
        FILE *file = fopen(argv[1], "r");
        if (!file) {
            perror("Error opening input file");
            return 1;
        }
        yyin = file;
    } else {
        printf("Reading input from standard input...\n");
    }

    printf("=========================================\n");
    printf("1. PARSING & INTERMEDIATE CODE GENERATION\n");
    printf("=========================================\n");
    
    if (yyparse() == 0) {
        printf("[+] Syntax Analysis Successful.\n\n");
    } else {
        printf("[-] Syntax Analysis Failed.\n");
        return 1;
    }

    printf("--- Generated Three-Address Code (TAC) ---\n");
    print_tac(tac_list, tac_count);

    printf("\n=========================================\n");
    printf("2. CODE OPTIMIZATION (CODE MOTION & FOLDING)\n");
    printf("=========================================\n");
    
    TAC optimized_tac[100];
    int opt_count = 0;

    optimize_code(tac_list, tac_count, optimized_tac, &opt_count);
    print_tac(optimized_tac, opt_count);

    printf("\n=========================================\n");
    printf("3. TARGET CODE GENERATION                \n");
    generate_target_code(optimized_tac, opt_count);

    return 0;
}