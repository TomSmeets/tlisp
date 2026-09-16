#pragma once
#include "ast.h"

static void pretty_value(Expr e);
static void pretty_list(Expr e);

static bool is_string(Expr e) {
    Expr_Type t = expr_get_type(e);
    if (t == Expr_Nil) return true;
    if (t != Expr_Cons) return false;

    Expr car = expr_get_car(e);
    if (expr_get_type(car) != Expr_Value) return false;

    i64 val = expr_get_value(car);
    if (!(val >= ' ' && val <= '~')) return false;

    Expr cdr = expr_get_cdr(e);
    return is_string(cdr);
}

static void pretty_string(Expr e) {
    if (!e) return;
    printf("%c", (char)expr_get_value(expr_get_car(e)));
    pretty_string(expr_get_cdr(e));
}
static void pretty_value(Expr e) {
    Expr_Type t = expr_get_type(e);
    if (t == Expr_Nil) {
        printf("()");
    } else if (is_string(e)) {
        printf("\"");
        pretty_string(e);
        printf("\"");
    } else if (t == Expr_Value) {
        i64 val = expr_get_value(e);
        printf("%ld", val);
    } else if (t == Expr_Cons) {
        printf("(");
        pretty_list(e);
        printf(")");
    }
}

static void pretty_list(Expr e) {
    Expr car = expr_get_car(e);
    Expr cdr = expr_get_cdr(e);
    pretty_value(car);

    Expr_Type t = expr_get_type(cdr);
    if (t == Expr_Nil) {
        return;
    } else if (t == Expr_Cons) {
        printf(" ");
        pretty_list(cdr);
    } else {
        printf(" . ");
        pretty_value(cdr);
    }
}
