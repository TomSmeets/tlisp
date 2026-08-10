#pragma once
#include "mem.h"

typedef struct Expr Expr;

struct Expr {
    enum {
        Expr_None,
        Expr_Label,
        Expr_Num,
        Expr_Cons,
    } type;

    union {
        char *label;
        long num;
        struct {
            Expr *car;
            Expr *cdr;
        };
    };
};

static Expr *expr_cons(Expr *car, Expr *cdr) {
    Expr *e = mem_struct(Expr);
    e->type = Expr_Cons;
    e->car = car;
    e->cdr = cdr;
    return e;
}

static Expr *expr_label(char *label) {
    Expr *e = mem_struct(Expr);
    e->type = Expr_Label;
    e->label = label;
    return e;
}

static Expr *expr_num(long num) {
    Expr *e = mem_struct(Expr);
    e->type = Expr_Num;
    e->num = num;
    return e;
}
