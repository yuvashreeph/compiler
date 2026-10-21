#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ir.h"

#define MAX_CONSTANTS 256
#define MAX_NAME 128
#define MAX_VALUE 128

typedef struct {
    char name[MAX_NAME];
    char value[MAX_VALUE];
    int valid;
} Constant;

static Constant constants[MAX_CONSTANTS];
static int constant_count = 0;

static int is_number(const char *s, long *value)
{
    char *end;
    long n = strtol(s, &end, 10);

    if (s == end || *end != '\0')
        return 0;

    *value = n;
    return 1;
}

static const char *get_constant(const char *name)
{
    for (int i = 0; i < constant_count; i++) {
        if (constants[i].valid &&
            strcmp(constants[i].name, name) == 0) {
            return constants[i].value;
        }
    }

    return NULL;
}

static void invalidate_constant(const char *name)
{
    for (int i = 0; i < constant_count; i++) {
        if (strcmp(constants[i].name, name) == 0) {
            constants[i].valid = 0;
        }
    }
}

static void set_constant(const char *name, const char *value)
{
    for (int i = 0; i < constant_count; i++) {
        if (strcmp(constants[i].name, name) == 0) {
            snprintf(constants[i].value,
                     sizeof(constants[i].value), "%s", value);
            constants[i].valid = 1;
            return;
        }
    }

    if (constant_count >= MAX_CONSTANTS)
        return;

    snprintf(constants[constant_count].name,
             sizeof(constants[constant_count].name), "%s", name);

    snprintf(constants[constant_count].value,
             sizeof(constants[constant_count].value), "%s", value);

    constants[constant_count].valid = 1;
    constant_count++;
}

static void replace_instruction(Instruction *p, const char *text)
{
    char *copy = malloc(strlen(text) + 1);

    if (copy == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    strcpy(copy, text);
    free(p->text);
    p->text = copy;
}

static int is_arithmetic_operator(const char *op)
{
    return strcmp(op, "+") == 0 ||
           strcmp(op, "-") == 0 ||
           strcmp(op, "*") == 0 ||
           strcmp(op, "/") == 0;
}

void optimize_code(void)
{
    constant_count = 0;

    for (Instruction *p = ir_head; p != NULL; p = p->next) {
        char lhs[MAX_NAME], a[MAX_VALUE], op[16], b[MAX_VALUE];
        char extra;
        long x, y, result;
        char output[512];

        /* Do not propagate constants across control-flow boundaries. */
        if (strchr(p->text, ':') != NULL ||
            strncmp(p->text, "goto ", 5) == 0 ||
            strncmp(p->text, "ifFalse ", 8) == 0 ||
            strncmp(p->text, "if ", 3) == 0 ||
            strncmp(p->text, "return", 6) == 0) {
            constant_count = 0;
            continue;
        }

        /* Handle binary instructions: lhs = a op b */
        if (sscanf(p->text, "%127s = %127s %15s %127s %c",
                   lhs, a, op, b, &extra) == 4) {

            const char *ca = get_constant(a);
            const char *cb = get_constant(b);

            if (ca != NULL)
                snprintf(a, sizeof(a), "%s", ca);

            if (cb != NULL)
                snprintf(b, sizeof(b), "%s", cb);

            if (is_arithmetic_operator(op) &&
                is_number(a, &x) && is_number(b, &y)) {

                int valid = 1;

                if (strcmp(op, "+") == 0)
                    result = x + y;
                else if (strcmp(op, "-") == 0)
                    result = x - y;
                else if (strcmp(op, "*") == 0)
                    result = x * y;
                else if (strcmp(op, "/") == 0 && y != 0)
                    result = x / y;
                else {
                    result = 0;
                    valid = 0;
                }

                if (valid) {
                    snprintf(output, sizeof(output),
                             "%s = %ld", lhs, result);

                    replace_instruction(p, output);

                    char value[MAX_VALUE];
                    snprintf(value, sizeof(value), "%ld", result);
                    set_constant(lhs, value);
                    continue;
                }
            }

            snprintf(output, sizeof(output), "%s = %s %s %s",
                     lhs, a, op, b);
            replace_instruction(p, output);
            invalidate_constant(lhs);
            continue;
        }

        /* Handle simple assignments: lhs = rhs */
        if (sscanf(p->text, "%127s = %127s %c",
                   lhs, a, &extra) == 2) {

            const char *value = get_constant(a);

            if (value != NULL)
                snprintf(a, sizeof(a), "%s", value);

            snprintf(output, sizeof(output), "%s = %s", lhs, a);
            replace_instruction(p, output);

            long n;
            if (is_number(a, &n))
                set_constant(lhs, a);
            else
                invalidate_constant(lhs);
        }
    }
}
