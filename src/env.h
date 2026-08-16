#pragma once
#include "ast.h"

// Search the environment for a symbol
static Expr *env_search(Expr *env, Expr *label) {
    if (expr_eq(expr_label("env"), label)) return env;

    for (;;) {
        if (env == 0) return 0;
        Expr *pair = expr_car(env);
        Expr *next = expr_cdr(env);
        Expr *key = expr_car(pair);
        Expr *val = expr_cdr(pair);
        if (expr_eq(key, label)) return val;
        env = next;
    }
}

static Expr *env_add(Expr *env, Expr *key, Expr *value) {
    return expr_cons(expr_cons(key, value), env);
}
