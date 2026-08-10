#include <memory.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// ================
// Types
// ================
typedef __UINT8_TYPE__ u8;
typedef __INTPTR_TYPE__ intptr_t;
typedef __SIZE_TYPE__ size_t;
typedef float f32;

// ================
// Memory
// ================
static u8 heap_data[1024 * 1024];
static size_t heap_used;
static void *mem_alloc_x(size_t size, size_t align, bool zero) {
    heap_used = (heap_used + (align - 1)) & ~(align - 1);
    void *ptr = heap_data + heap_used;
    heap_used += size;
    if (zero) memset(ptr, 0, size);
    return ptr;
}

static void *mem_alloc(size_t size) {
    return mem_alloc_x(size, 1, false);
}
#define mem_struct(T) (T *)mem_alloc_x(sizeof(T), alignof(T), true)

// ================
// Strings
// ================
static void *str_dup(size_t len, char *s) {
    char *buf = mem_alloc(len + 1);
    for (size_t i = 0; i < len; ++i) buf[i] = s[i];
    buf[len] = 0;
    return buf;
}

// ================
// AST
// ================
typedef struct Expr Expr;

struct Expr {
    char *label;
    Expr *car;
    Expr *cdr;
};

static Expr *expr_cons(Expr *car, Expr *cdr) {
    Expr *e = mem_struct(Expr);
    e->car = car;
    e->cdr = cdr;
    return e;
}

static Expr *expr_label(char *label) {
    Expr *e = mem_struct(Expr);
    e->label = label;
    return e;
}

// ================
// Parser
// ================
typedef struct {
    char *cursor;
} Parse;

static char parse_peek(Parse *p) {
    return *p->cursor;
}

static void parse_next(Parse *p) {
    p->cursor++;
}

static void parse_whitespace(Parse *p) {
    for (;;) {
        char c = parse_peek(p);
        if (c != ' ' && c != '\n') break;
        parse_next(p);
    }
}

static Expr *parse_list(Parse *p);
static Expr *parse_value(Parse *p);

static Expr *parse_value(Parse *p) {
    parse_whitespace(p);

    char c = parse_peek(p);

    if (c == '(') {
        parse_next(p);
        return parse_list(p);
    }

    if (c == '\0') return 0;

    char *expr_start = p->cursor;
    for (;;) {
        c = parse_peek(p);
        if (c == '\0') break;
        if (c == ' ') break;
        if (c == ')') break;
        if (c == '(') break;
        parse_next(p);
    }
    char *expr_end = p->cursor;
    char *label = str_dup(expr_end - expr_start, expr_start);
    Expr *e = expr_label(label);
    return e;
}

// Returns a list
static Expr *parse_list(Parse *p) {
    Expr *list = 0;
    Expr *last = 0;

    parse_whitespace(p);
    char c = parse_peek(p);

    // Nil
    if (c == ')') {
        parse_next(p);
        return 0;
    }

    if (c == '\0') {
        return 0;
    }

    Expr *car = parse_value(p);
    Expr *cdr = parse_list(p);
    Expr *ret = expr_cons(car, cdr);
    return ret;
}

// ================
// Printer
// ================
static void pretty_value(Expr *e);
static void pretty_list(Expr *e);

static void pretty_value(Expr *e) {
    if (e == 0) {
        printf("()");
    } else if (e->label) {
        printf("%s", e->label);
    } else {
        printf("(");
        pretty_list(e);
        printf(")");
    }
}

static void pretty_list(Expr *e) {
    if (e == 0) return;
    pretty_value(e->car);

    if (e->cdr) {
        printf(" ");
        pretty_list(e->cdr);
    }
}

int main(void) {
    const char code[] = "(a () b (c) (d e) ())";
    printf("Code: %s\n", code);

    char *cursor = (char *)code;
    char *end = cursor + sizeof(code);

    Parse p = {.cursor = (char *)code};
    Expr *e = parse_list(&p);
    pretty_list(e);
    printf("\n");
}
