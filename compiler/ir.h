#ifndef IR_H
#define IR_H

typedef struct Instruction {
    char *text;
    struct Instruction *next;
} Instruction;

extern Instruction *ir_head;
extern Instruction *ir_tail;

void emit(const char *text);
char *new_temp(void);
char *new_label(void);
void emit_label(const char *label);
void print_ir(void);
void clear_ir(void);
void optimize_code(void);
void generate_target_code(void);

#endif