// Evaluate tlisp expressions
#pragma once
#include "ast.h"
#include "env.h"
#include "pretty.h"
#include "str.h"

// ================
// Eval
// ================

static Expr *eval_list(Expr *e, Expr *env);

static Expr *eval_value(Expr *e, Expr *env) {
    if (!e) return 0;

    Expr_Type type = expr_type(e);
    if (type == Expr_Cons) return eval_list(e, env);
    if (type == Expr_Label) {
        Expr *e2 = env_search(env, e);
        if (!e2) {
            printf("Value not found: ");
            printf("Label = ");
            pretty_value(e);
            printf("\n");
            printf("Env = ");
            pretty_value(env);
            printf("\n");
        }
        return e2;
    }
    return e;
}

static Expr *eval_list(Expr *e, Expr *env) {
    Expr_Type type = expr_type(e);

    Expr *car = expr_car(e);
    Expr *cdr = expr_cdr(e);

    if (expr_eq(car, expr_label("add"))) {
        long sum = 0;
        Expr *it = cdr;
        while (it) {
            car = expr_car(it);
            cdr = expr_cdr(it);
            Expr *val = eval_value(car, env);
            if (val->type != Expr_Integer) {
                printf("ERROR, expected a number, got:");
                pretty_value(val);
                printf("\n");
                break;
            }
            sum += expr_get_int(val);
            it = cdr;
        }
        return expr_int(sum);
    }

    if (expr_eq(car, expr_label("quote"))) {
        return expr_car(cdr);
    }

    if (expr_eq(car, expr_label("let"))) {
        Expr *name = expr_car(cdr);
        cdr = expr_cdr(cdr);
        Expr *value = expr_car(cdr);
        cdr = expr_cdr(cdr);
        Expr *rest = expr_car(cdr);
        cdr = expr_cdr(cdr);

        if (cdr != 0) {
            printf("ERROR: Too many arguments\n");
            return 0;
        }

        value = eval_value(value, env);
        Expr *env2 = env_add(env, name, value);
        return eval_value(rest, env2);
    }

    // if (str_eq(label, "do")) {
    //     Expr *it = e->cdr;
    //     Expr *ret = 0;
    //     while (it) {
    //         ret = eval_value(it->car, env);
    //         it = it->cdr;
    //     }
    //     return ret;
    // }

    if (expr_eq(car, expr_label("print"))) {
        while (cdr) {
            pretty_value(eval_value(expr_car(cdr), env));
            printf(" ");
            cdr = expr_cdr(cdr);
        }
        printf("\n");
        return 0;
    }

    // if (str_eq(label, "fn")) {
    //     return e;
    // }

    // Expr *lam = eval_value(e->car, env);

    // // (X y . .)
    // // (fn (x y z) (+ x y z))
    // if (str_eq(lam->car->label, "fn")) {
    //     Expr *args = lam->cdr->car;
    //     Expr *body = lam->cdr->cdr->car;
    //     if (lam->cdr->cdr->cdr != 0) printf("ERROR\n");

    //     // Iterate over arguments and create a new env for the function body
    //     Expr *arg_values = e->cdr;
    //     Expr *arg_names = args;
    //     Expr *body_env = env;
    //     for (;;) {
    //         if (arg_names == 0 && arg_values == 0) break;

    //         if (arg_names == 0) {
    //             printf("Too many arguments\n");
    //             break;
    //         }

    //         if (arg_values == 0) {
    //             printf("Missing arguments\n");
    //             break;
    //         }

    //         body_env = env_add(body_env, arg_names->car, eval_value(arg_values->car, env));
    //         arg_names = arg_names->cdr;
    //         arg_values = arg_values->cdr;
    //     }

    //     Expr *result = eval_value(body, body_env);
    //     return result;
    // }

    return e;
}
