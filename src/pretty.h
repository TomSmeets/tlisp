#pragma once
#include "ast.h"

static void pretty_value(Expr e);
static void pretty_list(Expr e);

static bool is_string(Expr e) {
    Expr_Type t = expr_get_type(e);
    if (t == EXPR_TYPE_NIL) return true;
    if (t != EXPR_TYPE_CONS) return false;

    Expr car = expr_get_car(e);
    if (expr_get_type(car) != EXPR_TYPE_VALUE) return false;

    i64 val = expr_get_value(car);
    if (!((val >= ' ' && val <= '~') || val == '\n')) return false;

    Expr cdr = expr_get_cdr(e);
    return is_string(cdr);
}

static void pretty_string(Expr e) {
    if (expr_is_nil(e)) return;
    char c = expr_get_value(expr_get_car(e));
    if (c == '\n') {
        printf("\\n");
    } else {
        printf("%c", c);
    }
    pretty_string(expr_get_cdr(e));
}
static void pretty_value(Expr e) {
    Expr_Type t = expr_get_type(e);
    if (t == EXPR_TYPE_NIL) {
        printf("()");
    } else if (is_string(e)) {
        printf("\"");
        pretty_string(e);
        printf("\"");
    } else if (t == EXPR_TYPE_VALUE) {
        i64 val = expr_get_value(e);
        printf("%ld", val);
    } else if (t == EXPR_TYPE_BUILTIN) {
        printf("##");
    } else if (t == EXPR_TYPE_CONS) {
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
    if (t == EXPR_TYPE_NIL) {
        return;
    } else if (t == EXPR_TYPE_CONS) {
        printf(" ");
        pretty_list(cdr);
    } else {
        printf(" . ");
        pretty_value(cdr);
    }
}
