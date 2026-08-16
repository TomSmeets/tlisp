#pragma once
#include "type.h"
#include <sys/mman.h>

typedef enum {
    Expr_Cons,
    Expr_Label,
    Expr_Integer,
    Expr_Builtin,
} Expr_Type;

typedef struct {
    union {
        struct {
            u64 type : 3;
            u64 mark : 1;
            u64 car : 30;
            u64 cdr : 30;
        };
        struct {
            i64 _pad : 4;
            i64 value : 60;
        };
    };
} Expr;
static_assert(sizeof(Expr) == 8);

static u32 expr_free_count;
static u32 expr_free_list;
static u32 expr_count;
static Expr *expr_list;

static Expr *expr_get(u32 index) {
    if (index == 0) return 0;
    return expr_list + index - 1;
}

static u32 expr_index(Expr *expr) {
    if (!expr) return 0;
    return expr - expr_list + 1;
}

static Expr *expr_new(void) {
    Expr *e = expr_get(expr_free_list);
    if (!expr_list) expr_list = mmap(0, (size_t)sizeof(Expr) * (1ULL << 30), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (!e) return expr_list + expr_count++;
    expr_free_list = e->cdr;
    expr_free_count--;
    *e = (Expr){};
    return e;
}

static Expr_Type expr_type(Expr *expr) {
    return expr->type;
}

static Expr *expr_car(Expr *expr) {
    return expr_get(expr->car);
}

static Expr *expr_cdr(Expr *expr) {
    return expr_get(expr->cdr);
}

static void expr_mark(Expr *expr) {
    // Nil
    if (!expr) return;

    // Already marked
    if (expr->mark) return;

    // Mark expression
    expr->mark = 1;
    Expr_Type type = expr_type(expr);
    if (type == Expr_Cons) expr_mark(expr_car(expr));
    expr_mark(expr_cdr(expr));
}

static void expr_sweep(void) {
    expr_free_list = 0;
    expr_free_count = 0;
    for (int i = expr_count - 1; i >= 0; --i) {
        Expr *e = expr_list + i;

        // Object is in use
        if (e->mark) {
            e->mark = 0;
            continue;
        }

        // Object is free
        e->cdr = expr_free_list;
        expr_free_list = i + 1;
        expr_free_count++;
    }
}

static Expr *expr_cons(Expr *car, Expr *cdr) {
    Expr *e = expr_new();
    e->type = Expr_Cons;
    e->car = expr_index(car);
    e->cdr = expr_index(cdr);
    return e;
}

static Expr *expr_int(long value) {
    Expr *e = expr_new();
    e->type = Expr_Integer;
    e->value = value;
    return e;
}

static long expr_get_int(Expr *e) {
    return e->value;
}

static u8 chr_encode(u8 c) {
    if (c == 0) return 0;
    if (c >= 'a' && c <= 'z') return c - 'a' + 1;
    if (c >= 'A' && c <= 'Z') return c - 'A' + 1;
    if (c == '-') return 27;
    if (c == '?') return 28;
    if (c == '!') return 29;
    if (c == '*') return 30;
    return 0;
}

static u8 chr_decode(u8 code) {
    if (code == 0) return 0;
    if (code >= 1 && code <= 26) return code - 1 + 'a';
    if (code == 27) return '-';
    if (code == 28) return '?';
    if (code == 29) return '!';
    if (code == 30) return '*';
    return 0;
}

static Expr *expr_label(char *str) {
    if (*str == 0) return 0;

    u64 value = 0;
    for (u32 i = 0; i < 12; ++i) {
        if (!*str) break;
        u8 c = chr_encode(*str++);
        value |= c << (5 * i);
    }

    Expr *e = expr_new();
    e->type = Expr_Label;
    e->value = value;
    return e;
}

static void expr_get_label(Expr *e, char label[13]) {
    for (u32 i = 0; i < 12; ++i) {
        label[i] = chr_decode((e->value >> (5 * i)) & 31);
    }
    label[12] = 0;
}

static bool expr_eq(Expr *a, Expr *b) {
    if (a == 0 && b == 0) return true;
    if (a == 0 || b == 0) return false;
    if (a->type != b->type) return false;
    Expr_Type type = expr_type(a);
    if (type == Expr_Cons) {
        Expr *a_car = expr_car(a);
        Expr *b_car = expr_car(b);
        Expr *a_cdr = expr_cdr(a);
        Expr *b_cdr = expr_cdr(b);
        return expr_eq(a_car, b_car) && expr_eq(a_cdr, b_cdr);
    }
    return a->value == b->value;
}
