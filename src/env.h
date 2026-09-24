#pragma once
#include "ast.h"

// Search the environment for a symbol
static bool env_search(Expr env, Expr label, Expr *out) {
    // if (expr_eq(expr_label("env"), label)) return env;
    Expr it = env;
    for (;;) {
        if (expr_is_nil(it)) return false;
        Expr pair = expr_get_car(it);
        Expr next = expr_get_cdr(it);

        Expr key = expr_get_car(pair);
        Expr val = expr_get_cdr(pair);
        if (expr_eq(key, label)) {
            *out = val;
            return true;
        }
        it = next;
    }
}

// Search the environment for a symbol
static bool env_set(Expr env, Expr label, Expr value) {
    // if (expr_eq(expr_label("env"), label)) return env;
    Expr it = env;
    for (;;) {
        if (expr_is_nil(it)) return false;
        Expr pair = expr_get_car(it);
        Expr next = expr_get_cdr(it);

        Expr key = expr_get_car(pair);
        Expr val = expr_get_cdr(pair);
        if (expr_eq(key, label)) {
            expr_set_cdr(pair, value);
            return true;
        }
        it = next;
    }
}

static void env_add(Expr *env, Expr key, Expr value) {
    *env = expr_cons(expr_cons(key, value), *env);
}
