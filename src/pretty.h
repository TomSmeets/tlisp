#pragma once
#include "ast.h"

static void pretty_value(Expr *e);
static void pretty_list(Expr *e);

static void pretty_value(Expr *e) {
    if (e == 0) {
        printf("()");
    } else if (e->type == Expr_Label) {
        printf("%s", e->label);
    } else if (e->type == Expr_Num) {
        printf("%ld", e->num);
    } else if (e->type == Expr_Cons) {
        printf("(");
        pretty_list(e);
        printf(")");
    }
}

static void pretty_list(Expr *e) {
    if (e == 0) return;
    pretty_value(e->car);
    if (e->cdr) {
        if(e->cdr->type == Expr_Cons) {
            printf(" ");
            pretty_list(e->cdr);
        } else {
            printf(" . ");
            pretty_value(e->cdr);
        }
    }
}
