#pragma once
#include "ast.h"
#include "gc.h"

static Expr *expr_cons(Expr *car, Expr *cdr) {
    Expr *e = gc_alloc();
    e->type = Expr_Cons;
    e->car = car;
    e->cdr = cdr;
    return e;
}

static Expr *expr_label(char *label) {
    Expr *e = gc_alloc();
    e->type = Expr_Label;
    e->label = label;
    return e;
}

static Expr *expr_num(long num) {
    Expr *e = gc_alloc();
    e->type = Expr_Num;
    e->num = num;
    return e;
}
