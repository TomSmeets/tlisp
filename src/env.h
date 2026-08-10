#pragma  once
#include "ast.h"
#include "str.h"

// Search the environment for a symbol
static Expr *env_search(Expr *env, char *label) {
    if(str_eq(label, "env")) return env;

    for(;;) {
        if(env == 0) return 0;

        // Pair
        Expr *key = env->car->car;
        Expr *val = env->car->cdr->car;
        if(str_eq(key->label, label)) return val;
        env = env->cdr;
    }
}

static Expr *env_add(Expr *env, Expr *key, Expr *value) {
    return expr_cons(expr_cons(key, expr_cons(value, 0)), env);
}
