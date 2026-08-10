// Evaluate tlisp expressions
#pragma once
#include "ast.h"
#include "str.h"
#include "env.h"

// ================
// Eval
// ================


static Expr *eval_list(Expr *e, Expr *env);

static Expr *eval_value(Expr *e, Expr *env) {
    if (e->type == Expr_Cons) return eval_list(e, env);
    if (e->type == Expr_Label) {
        Expr *e2 = env_search(env, e->label);
        if(!e2)  printf("Value not found: %s\n", e->label);
        return e2;
    }
    return e;
}
static Expr *eval_list(Expr *e, Expr *env) {
    char *label = e->car->type == Expr_Label ? e->car->label : 0;
    if(str_eq(label, "+")) {
        long sum = 0;
        Expr *it = e->cdr;
        while(it) {
            Expr *val =eval_value(it->car, env);
            if (val->type != Expr_Num) {
                printf("ERROR\n");
                break;
            }
            sum += val->num;
            it = it->cdr;
        }
        return expr_num(sum);
    }

    if(str_eq(label, "let")) {
        // Label
        Expr *arg0 = e->cdr;

        // value
        Expr *arg1 = arg0->cdr;

        // Rest
        Expr *arg2 = arg1->cdr;


        if(arg2->cdr != 0) {
            printf("ERROR: Too many arguments\n");
        }

        Expr *key = arg0->car;
        Expr *value = eval_value(arg1->car, env);
        Expr *env2 = env_add(env, key, value);
        return eval_value(arg2->car, env2);
    }

    if(str_eq(label, "fn")) {
        return e;
    }

    Expr *lam = eval_value(e->car, env);

    // (X y . .)
    // (fn (x y z) (+ x y z))
    if(str_eq(lam->car->label, "fn")) {
        Expr *args = lam->cdr->car;
        Expr *body = lam->cdr->cdr->car;
        if(lam->cdr->cdr->cdr != 0) printf("ERROR\n");

        // Iterate over arguments and create a new env for the function body
        Expr *arg_values = e->cdr;
        Expr *arg_names  = args;
        Expr *body_env = env;
        for(;;) {
            if(arg_names == 0 && arg_values == 0) break;

            if(arg_names == 0) {
                printf("Too many arguments");
                break;
            }

            if(arg_values == 0) {
                printf("Missing arguments");
                break;
            }

            body_env = env_add(body_env, arg_names->car, eval_value(arg_values->car, env));
            arg_names = arg_names->cdr;
            arg_values = arg_values->cdr;
        }

        Expr *result = eval_value(body, body_env);
        return result;
    }

    return e;
}
