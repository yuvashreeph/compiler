#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ir.h"

Instruction *ir_head = NULL;
Instruction *ir_tail = NULL;

static int temp_count = 0;
static int label_count = 0;

void emit(const char *text)
{
    Instruction *node = malloc(sizeof(*node));
    if (!node) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    node->text = malloc(strlen(text) + 1);
    if (!node->text) {
        perror("malloc");
        free(node);
        exit(EXIT_FAILURE);
    }

    strcpy(node->text, text);
    node->next = NULL;

    if (!ir_head)
        ir_head = node;
    else
        ir_tail->next = node;

    ir_tail = node;
}

char *new_temp(void)
{
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "t%d", ++temp_count);

    char *result = malloc(strlen(buffer) + 1);
    if (!result) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    strcpy(result, buffer);
    return result;
}

char *new_label(void)
{
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "L%d", ++label_count);

    char *result = malloc(strlen(buffer) + 1);
    if (!result) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    strcpy(result, buffer);
    return result;
}

void emit_label(const char *label)
{
    char buffer[128];
    snprintf(buffer, sizeof(buffer), "%s:", label);
    emit(buffer);
}

void print_ir(void)
{
    for (Instruction *p = ir_head; p; p = p->next)
        puts(p->text);
}

void clear_ir(void)
{
    Instruction *p = ir_head;

    while (p) {
        Instruction *next = p->next;
        free(p->text);
        free(p);
        p = next;
    }

    ir_head = ir_tail = NULL;
}