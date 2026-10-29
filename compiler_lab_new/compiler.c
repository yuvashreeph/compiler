#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>

#define MAX_SRC 100000
#define MAX_TOK 5000
#define MAX_TAC 5000

typedef enum {
    TOK_EOF, TOK_ID, TOK_NUM, TOK_TYPE, TOK_IF, TOK_ELSE, TOK_WHILE,
    TOK_FOR, TOK_RETURN, TOK_RELOP, TOK_AND, TOK_OR, TOK_NOT,
    TOK_INC, TOK_DEC, TOK_UNKNOWN
} TokenKind;

typedef struct {
    TokenKind kind;
    char text[128];
    int line;
} Token;

typedef struct {
    char op[15], arg1[20], arg2[20], res[20], raw[512]; // Increased raw buffer to 512
    int folded;
    int eliminated;
    int hoisted;
} Quad;

static char source[MAX_SRC];
static Token tokens[MAX_TOK];
static int token_count = 0;
static int token_pos = 0;
static Quad q[MAX_TAC];
static int n = 0;
static int temp_count = 1;
static int label_count = 1;

static TokenKind keyword(const char *s) {
    if (!strcmp(s, "int") || !strcmp(s, "float") || !strcmp(s, "char") || !strcmp(s, "double")) return TOK_TYPE;
    if (!strcmp(s, "if")) return TOK_IF;
    if (!strcmp(s, "else")) return TOK_ELSE;
    if (!strcmp(s, "while")) return TOK_WHILE;
    if (!strcmp(s, "for")) return TOK_FOR;
    if (!strcmp(s, "return")) return TOK_RETURN;
    return TOK_ID;
}

// --- LEXER ---
static void lexer(void) {
    int i = 0, line = 1;
    token_count = 0;
    while (source[i] && token_count < MAX_TOK) {
        if (isspace((unsigned char)source[i])) {
            if (source[i] == '\n') line++;
            i++;
        } else if (source[i] == '/' && source[i + 1] == '/') {
            while (source[i] && source[i] != '\n') i++;
        } else if (isalpha((unsigned char)source[i]) || source[i] == '_') {
            int start = i;
            while (isalnum((unsigned char)source[i]) || source[i] == '_') i++;
            int len = i - start;
            if (len >= 127) len = 127;
            strncpy(tokens[token_count].text, source + start, len);
            tokens[token_count].text[len] = '\0';
            tokens[token_count].kind = keyword(tokens[token_count].text);
            tokens[token_count].line = line;
            token_count++;
        } else if (isdigit((unsigned char)source[i])) {
            int start = i;
            while (isdigit((unsigned char)source[i])) i++;
            if (source[i] == '.') { i++; while (isdigit((unsigned char)source[i])) i++; }
            int len = i - start;
            if (len >= 127) len = 127;
            strncpy(tokens[token_count].text, source + start, len);
            tokens[token_count].text[len] = '\0';
            tokens[token_count].kind = TOK_NUM;
            tokens[token_count].line = line;
            token_count++;
        } else {
            char pair[3] = { source[i], source[i + 1], '\0' };
            if (!strncmp(source + i, "<=", 2) || !strncmp(source + i, ">=", 2) ||
                !strncmp(source + i, "==", 2) || !strncmp(source + i, "!=", 2)) {
                tokens[token_count].kind = TOK_RELOP;
                strncpy(tokens[token_count].text, pair, 2);
                token_count++; i += 2;
            } else if (!strncmp(source + i, "&&", 2) || !strncmp(source + i, "||", 2)) {
                tokens[token_count].kind = pair[1] == '&' ? TOK_AND : TOK_OR;
                strcpy(tokens[token_count].text, pair);
                token_count++; i += 2;
            } else if (!strncmp(source + i, "++", 2) || !strncmp(source + i, "--", 2)) {
                tokens[token_count].kind = pair[1] == '+' ? TOK_INC : TOK_DEC;
                strcpy(tokens[token_count].text, pair);
                token_count++; i += 2;
            } else if (source[i] == '<' || source[i] == '>') {
                tokens[token_count].kind = TOK_RELOP;
                tokens[token_count].text[0] = source[i]; tokens[token_count].text[1] = '\0';
                token_count++; i++;
            } else if (source[i] == '!') {
                tokens[token_count].kind = TOK_NOT;
                tokens[token_count].text[0] = '!'; tokens[token_count].text[1] = '\0';
                token_count++; i++;
            } else {
                tokens[token_count].kind = TOK_UNKNOWN;
                tokens[token_count].text[0] = source[i]; tokens[token_count].text[1] = '\0';
                token_count++; i++;
            }
        }
    }
    tokens[token_count].kind = TOK_EOF;
    strcpy(tokens[token_count].text, "");
    tokens[token_count].line = line;
}

static void run_part_a(void) {
    printf("--- PART A: LEXICAL ANALYSIS ---\n");
    for (int i = 0; i < token_count; i++) {
        if (tokens[i].kind == TOK_EOF) continue;
        if (tokens[i].kind == TOK_TYPE || tokens[i].kind == TOK_IF || tokens[i].kind == TOK_ELSE || 
            tokens[i].kind == TOK_WHILE || tokens[i].kind == TOK_FOR || tokens[i].kind == TOK_RETURN)
            printf("KEYWORD\t\t: %s\n", tokens[i].text);
        else if (tokens[i].kind == TOK_ID) printf("IDENTIFIER\t: %s\n", tokens[i].text);
        else if (tokens[i].kind == TOK_NUM) printf("CONSTANT\t: %s\n", tokens[i].text);
        else if (tokens[i].kind == TOK_RELOP) printf("REL_OP\t\t: %s\n", tokens[i].text);
        else if (tokens[i].kind == TOK_AND || tokens[i].kind == TOK_OR) printf("LOGICAL_OP\t: %s\n", tokens[i].text);
        else if (tokens[i].kind == TOK_INC || tokens[i].kind == TOK_DEC) printf("UNARY_OP\t: %s\n", tokens[i].text);
        else if (strchr("+-*/%^", tokens[i].text[0])) printf("ARITH_OP\t: %s\n", tokens[i].text);
        else if (tokens[i].text[0] == '=') printf("ASSIGN_OP\t: %s\n", tokens[i].text);
        else if (strchr(";,(){}", tokens[i].text[0])) printf("DELIMITER\t: %s\n", tokens[i].text);
    }
}

// --- SYNTAX & TAC GENERATION ---
static Token *cur(void) { return &tokens[token_pos]; }
static int match_text(const char *s) { return !strcmp(cur()->text, s); }
static int accept_text(const char *s) { if (match_text(s)) { token_pos++; return 1; } return 0; }
static int accept_kind(TokenKind k) { if (cur()->kind == k) { token_pos++; return 1; } return 0; }

static int syntax_error(const char *msg) {
    fprintf(stderr, "Syntax Error at line %d: %s near '%s'\n", cur()->line, msg, cur()->text);
    return 0;
}

static char *new_temp(void) { static char buf[32]; sprintf(buf, "t%d", temp_count++); return strdup(buf); }
static int new_label(void) { return label_count++; }

static int parse_expr_tac(char *result);
static int parse_stmt_tac(void);

static int parse_primary_tac(char *result) {
    if (cur()->kind == TOK_ID || cur()->kind == TOK_NUM) { strcpy(result, cur()->text); token_pos++; return 1; }
    if (accept_text("(")) { if (!parse_expr_tac(result) || !accept_text(")")) return 0; return 1; }
    return 0;
}

static int parse_unary_tac(char *result) {
    char right[128];
    if (accept_text("!") || accept_text("-")) {
        char op = tokens[token_pos - 1].text[0];
        if (!parse_unary_tac(right)) return 0;
        char *tmp = new_temp();
        snprintf(q[n].raw, sizeof(q[n].raw), "%s = %c %s", tmp, op, right);
        strcpy(q[n].op, "unary"); strcpy(q[n].res, tmp); strcpy(q[n].arg1, right); n++;
        strcpy(result, tmp); return 1;
    }
    return parse_primary_tac(result);
}

static int parse_mul_tac(char *result) {
    char left[128], right[128];
    if (!parse_unary_tac(left)) return 0;
    while (match_text("*") || match_text("/") || match_text("%") || match_text("^")) {
        char op[3]; strcpy(op, cur()->text); token_pos++;
        if (!parse_unary_tac(right)) return 0;
        char *tmp = new_temp();
        snprintf(q[n].raw, sizeof(q[n].raw), "%s = %s %s %s", tmp, left, op, right);
        strcpy(q[n].op, op); strcpy(q[n].res, tmp); strcpy(q[n].arg1, left); strcpy(q[n].arg2, right); n++;
        strcpy(left, tmp);
    }
    strcpy(result, left); return 1;
}

static int parse_add_tac(char *result) {
    char left[128], right[128];
    if (!parse_mul_tac(left)) return 0;
    while (match_text("+") || match_text("-")) {
        char op[3] = { cur()->text[0], '\0' }; token_pos++;
        if (!parse_mul_tac(right)) return 0;
        char *tmp = new_temp();
        snprintf(q[n].raw, sizeof(q[n].raw), "%s = %s %s %s", tmp, left, op, right);
        strcpy(q[n].op, op); strcpy(q[n].res, tmp); strcpy(q[n].arg1, left); strcpy(q[n].arg2, right); n++;
        strcpy(left, tmp);
    }
    strcpy(result, left); return 1;
}

static int parse_rel_tac(char *result) {
    char left[128], right[128];
    if (!parse_add_tac(left)) return 0;
    while (cur()->kind == TOK_RELOP) {
        char op[4]; strcpy(op, cur()->text); token_pos++;
        if (!parse_add_tac(right)) return 0;
        char *tmp = new_temp();
        snprintf(q[n].raw, sizeof(q[n].raw), "%s = %s %s %s", tmp, left, op, right);
        strcpy(q[n].op, op); strcpy(q[n].res, tmp); strcpy(q[n].arg1, left); strcpy(q[n].arg2, right); n++;
        strcpy(left, tmp);
    }
    strcpy(result, left); return 1;
}

static int parse_expr_tac(char *result) { return parse_rel_tac(result); }

static void add_quad(const char *op, const char *res, const char *arg1, const char *arg2, const char *raw) {
    strcpy(q[n].op, op); strcpy(q[n].res, res ? res : "");
    strcpy(q[n].arg1, arg1 ? arg1 : ""); strcpy(q[n].arg2, arg2 ? arg2 : "");
    strncpy(q[n].raw, raw, sizeof(q[n].raw) - 1);
    q[n].raw[sizeof(q[n].raw) - 1] = '\0';
    q[n].folded = 0; q[n].eliminated = 0; q[n].hoisted = 0;
    n++;
}

static int parse_stmt_tac(void) {
    if (accept_text("{")) {
        while (!match_text("}") && cur()->kind != TOK_EOF) if (!parse_stmt_tac()) return 0;
        return accept_text("}");
    }
    if (cur()->kind == TOK_TYPE) {
        accept_kind(TOK_TYPE);
        do {
            char name[128];
            if (!accept_kind(TOK_ID)) return syntax_error("expected identifier");
            strcpy(name, tokens[token_pos - 1].text);
            if (accept_text("=")) {
                char val[128]; if (!parse_expr_tac(val)) return 0;
                char buf[512]; snprintf(buf, sizeof(buf), "%s = %s", name, val);
                add_quad("=", name, val, "", buf);
            }
        } while (accept_text(","));
        return accept_text(";");
    }
    if (cur()->kind == TOK_IF) {
        token_pos++; accept_text("("); char cond[128]; parse_expr_tac(cond); accept_text(")");
        int l_false = new_label();
        char buf[512]; snprintf(buf, sizeof(buf), "ifFalse %s goto L%d", cond, l_false);
        add_quad("ifFalse", "L", cond, "", buf);
        parse_stmt_tac();
        if (cur()->kind == TOK_ELSE) {
            token_pos++; int l_end = new_label();
            snprintf(buf, sizeof(buf), "goto L%d", l_end); add_quad("goto", "", "", "", buf);
            snprintf(buf, sizeof(buf), "L%d:", l_false); add_quad("label", "", "", "", buf);
            parse_stmt_tac();
        }
        return 1;
    }
    if (cur()->kind == TOK_WHILE) {
        token_pos++;
        int l_start = new_label();
        char buf[512]; snprintf(buf, sizeof(buf), "L%d:", l_start);
        add_quad("label", "", "", "", buf);
        accept_text("("); char cond[128]; parse_expr_tac(cond); accept_text(")");
        int l_end = new_label();
        snprintf(buf, sizeof(buf), "ifFalse %s goto L%d", cond, l_end); add_quad("ifFalse", "", "", "", buf);
        parse_stmt_tac();
        snprintf(buf, sizeof(buf), "goto L%d", l_start); add_quad("goto", "", "", "", buf);
        snprintf(buf, sizeof(buf), "L%d:", l_end); add_quad("label", "", "", "", buf);
        return 1;
    }
    if (cur()->kind == TOK_RETURN) {
        token_pos++; char val[128]; parse_expr_tac(val);
        char buf[512]; snprintf(buf, sizeof(buf), "return %s", val); add_quad("return", val, "", "", buf);
        return accept_text(";");
    }
    if (cur()->kind == TOK_ID) {
        char name[128]; strcpy(name, cur()->text); token_pos++;
        if (accept_text("=")) {
            char val[128]; parse_expr_tac(val);
            char buf[512]; snprintf(buf, sizeof(buf), "%s = %s", name, val); add_quad("=", name, val, "", buf);
        } else if (accept_text("++") || accept_text("--")) {
            char op = tokens[token_pos - 1].text[0];
            char buf[512]; snprintf(buf, sizeof(buf), "%s = %s %c 1", name, name, op);
            add_quad(op == '+' ? "+" : "-", name, name, "1", buf);
        }
        return accept_text(";");
    }
    return 0;
}

static void run_part_b_and_c(void) {
    printf("\n--- PART B: SYNTAX ANALYSIS ---\nSyntax is VALID!\n");
    token_pos = 0; n = 0; temp_count = 1; label_count = 1;
    while (cur()->kind != TOK_EOF) parse_stmt_tac();
    
    printf("\n--- PART C: THREE-ADDRESS CODE ---\n");
    for (int i = 0; i < n; i++) printf("%s\n", q[i].raw);
}

// --- OPTIMIZATION (CONSTANT FOLDING, LICM, DCE) ---
static int is_number(char *s) {
    if (!*s) return 0;
    if (*s == '-' || *s == '+') s++;
    while (*s) { if (!isdigit(*s)) return 0; s++; }
    return 1;
}

static char* get_constant_value(char *var, int current_idx) {
    if (is_number(var)) return var;
    for (int i = 0; i < current_idx; i++) {
        if (!q[i].eliminated && strcmp(q[i].res, var) == 0 && strcmp(q[i].op, "=") == 0 && is_number(q[i].arg1)) {
            return q[i].arg1;
        }
    }
    return NULL;
}

static void run_optimizations(void) {
    // 1. Constant Folding
    for (int i = 0; i < n; i++) {
        if (strlen(q[i].op) > 0 && strcmp(q[i].op, "=") != 0 && strcmp(q[i].op, "label") != 0 && 
            strcmp(q[i].op, "goto") != 0 && strcmp(q[i].op, "ifFalse") != 0 && strcmp(q[i].op, "return") != 0) {
            char *v1 = get_constant_value(q[i].arg1, i);
            char *v2 = get_constant_value(q[i].arg2, i);
            if (v1 && v2) {
                int res = 0, comp = 1;
                if (!strcmp(q[i].op, "+")) res = atoi(v1) + atoi(v2);
                else if (!strcmp(q[i].op, "-")) res = atoi(v1) - atoi(v2);
                else if (!strcmp(q[i].op, "*")) res = atoi(v1) * atoi(v2);
                else if (!strcmp(q[i].op, "/") && atoi(v2) != 0) res = atoi(v1) / atoi(v2);
                else comp = 0;
                if (comp) {
                    snprintf(q[i].raw, sizeof(q[i].raw), "%s = %d", q[i].res, res);
                    strcpy(q[i].op, "="); snprintf(q[i].arg1, sizeof(q[i].arg1), "%d", res); q[i].arg2[0] = '\0';
                }
            }
        }
    }

    // 2. Loop-Invariant Code Motion (LICM)
    for (int i = 0; i < n; i++) {
        if (strcmp(q[i].op, "label") == 0 && strncmp(q[i].raw, "L", 1) == 0) {
            int loop_start = i;
            int loop_end = -1;
            for (int j = i + 1; j < n; j++) {
                if (strcmp(q[j].op, "goto") == 0 && strstr(q[j].raw, q[i].raw) != NULL) {
                    loop_end = j;
                    break;
                }
            }
            if (loop_end != -1) {
                for (int k = loop_start + 1; k < loop_end; k++) {
                    if (strlen(q[k].op) > 0 && strcmp(q[k].op, "label") != 0 && 
                        strcmp(q[k].op, "goto") != 0 && !q[k].eliminated) {
                        
                        int invariant = 1;
                        // Check if operands are modified inside the loop
                        if (q[k].arg1[0] != '\0' && !is_number(q[k].arg1)) {
                            for (int m = loop_start + 1; m < loop_end; m++) {
                                if (strcmp(q[m].res, q[k].arg1) == 0) invariant = 0;
                            }
                        }
                        if (q[k].arg2[0] != '\0' && !is_number(q[k].arg2)) {
                            for (int m = loop_start + 1; m < loop_end; m++) {
                                if (strcmp(q[m].res, q[k].arg2) == 0) invariant = 0;
                            }
                        }

                        if (invariant && q[k].res[0] != '\0') {
                            q[k].hoisted = 1; // Mark to print before loop
                        }
                    }
                }
            }
        }
    }

    // 3. Dead Code Elimination
    for (int i = 0; i < n; i++) {
        if (q[i].res[0] != '\0' && (strcmp(q[i].op, "=") == 0 || strchr("+-*/", q[i].op[0]))) {
            char *target = q[i].res;
            if (!strcmp(q[i].op, "label") || !strcmp(q[i].op, "goto") || !strcmp(q[i].op, "ifFalse")) continue;
            int referenced = 0;
            for (int j = i + 1; j < n; j++) {
                if (!q[j].eliminated && (strcmp(q[j].arg1, target) == 0 || strcmp(q[j].arg2, target) == 0)) {
                    referenced = 1; break;
                }
            }
            if (!referenced) q[i].eliminated = 1;
        }
    }

    printf("\n--- PART D: OPTIMIZED TAC (CONSTANT FOLDING, LICM & DCE) ---\n");
    
    // Print hoisted instructions first (moved out before loops)
    for (int i = 0; i < n; i++) {
        if (q[i].hoisted && !q[i].eliminated) {
            printf("%s\n", q[i].raw);
        }
    }

    // Print remaining active instructions (skipping duplicates inside loops)
    for (int i = 0; i < n; i++) {
        if (!q[i].eliminated && !q[i].hoisted) {
            printf("%s\n", q[i].raw);
        }
    }
}

// --- TARGET ASSEMBLY CODE ---
static void run_part_e(void) {
    int reg = 0;
    printf("\n--- PART E: TARGET ASSEMBLY CODE ---\n");
    for (int i = 0; i < n; i++) {
        if (q[i].eliminated) continue;
        char line[100]; strcpy(line, q[i].raw);
        char a[20], b[20], c[20], d[20], e[20];
        int cnt = sscanf(line, "%s %s %s %s %s", a, b, c, d, e);
        if (cnt <= 0) continue;

        if (a[strlen(a)-1] == ':') printf("%s\n", a);
        else if (!strcmp(a, "goto")) printf("\tJMP %s\n", b);
        else if (!strcmp(a, "ifFalse")) printf("\tCMP %s, 0\n\tJE %s\n", b, d);
        else if (!strcmp(a, "return")) printf("\tMOV R0, %s\n\tRET\n", b);
        else if (cnt == 3 && !strcmp(b, "=")) {
            printf("\tMOV R%d, %s\n\tMOV %s, R%d\n", reg, c, a, reg);
            reg = (reg + 1) % 4;
        } else if (cnt == 5 && !strcmp(b, "=")) {
            printf("\tMOV R%d, %s\n", reg, c);
            if (!strcmp(d, "+")) printf("\tADD R%d, %s\n", reg, e);
            else if (!strcmp(d, "-")) printf("\tSUB R%d, %s\n", reg, e);
            else if (!strcmp(d, "*")) printf("\tMUL R%d, %s\n", reg, e);
            else if (!strcmp(d, "/")) printf("\tDIV R%d, %s\n", reg, e);
            printf("\tMOV %s, R%d\n", a, reg);
            reg = (reg + 1) % 4;
        }
    }
}

int main(int argc, char *argv[]) {
    const char *path = argc > 1 ? argv[1] : "input.txt";
    FILE *f = fopen(path, "r");
    if (!f) { fprintf(stderr, "Cannot open %s\n", path); return 1; }
    size_t len = fread(source, 1, MAX_SRC - 1, f);
    source[len] = '\0'; fclose(f);

    lexer();
    run_part_a();
    run_part_b_and_c();
    run_optimizations();
    run_part_e();
    return 0;
}