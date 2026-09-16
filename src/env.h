#pragma once
#include "ast.h"

// Search the environment for a symbol
static Expr env_search(Expr env, Expr label) {
    // if (expr_eq(expr_label("env"), label)) return env;
    Expr it = env;
    for (;;) {
        if (it == 0) return expr_cons(expr_str("INVALID_VAR"), expr_cons(label, expr_cons(env, 0)));
        Expr pair = expr_get_car(it);
        Expr next = expr_get_cdr(it);

        Expr key = expr_get_car(pair);
        Expr val = expr_get_cdr(pair);
        if (expr_eq(key, label)) return val;
        it = next;
    }
}

static void env_add(Expr *env, Expr key, Expr value) {
    *env = expr_cons(expr_cons(key, value), *env);
}
