#pragma once
#include "ast.h"

static void pretty_value(Expr *e);
static void pretty_list(Expr *e);

static void pretty_value(Expr *e) {
    if (e == 0) {
        printf("()");
    } else if (e->type == Expr_Label) {
        char label[13];
        expr_get_label(e, label);
        printf("%s", label);
    } else if (e->type == Expr_Integer) {
        printf("%ld", expr_get_int(e));
    } else if (e->type == Expr_Cons) {
        printf("(");
        pretty_list(e);
        printf(")");
    }
}

static void pretty_list(Expr *e) {
    Expr *car = expr_car(e);
    Expr *cdr = expr_cdr(e);

    pretty_value(car);
    if (!e->cdr) return;

    if (expr_type(cdr) == Expr_Cons) {
        printf(" ");
        pretty_list(cdr);
    } else {
        printf(" . ");
        pretty_value(cdr);
    }
}
