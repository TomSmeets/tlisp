// Evaluate tlisp expressions
#pragma once
#include "ast.h"
#include "env.h"
#include "pretty.h"
#include "str.h"

// ================
// Eval
// ================


// A pair of value and environment
typedef struct {
    Expr *value;
    Expr *env;
} Scope;

static Scope scope(Expr *value, Expr *env) {
    return (Scope){value,env};
}

static Scope eval_list(Scope s);

static Scope eval_value(Scope s) {
    // Nil
    if (!s.value) return s;

    Expr_Type type = expr_type(s.value);

    // Cons
    if (type == Expr_Cons) return eval_list(s);

    // Deref label
    if (type == Expr_Label) {
        Expr *value2 = env_search(s.env, s.value);
        if (!value2) {
            printf("Value not found: ");
            printf("Label = ");
            pretty_value(s.value);
            printf("\n");
            printf("Env = ");
            pretty_value(s.env);
            printf("\n");
        }
        return scope(value2, s.env);
    }
    return s;
}

// Return argument without evaluating
static Scope eval_quote(Scope s) {
    Expr *car = expr_car(s.value);
    Expr *cdr = expr_cdr(s.value);
    return scope(expr_car(cdr), s.env);
}

// Sum all arguments
static Scope eval_add(Scope s) {
    long sum = 0;
    Expr *car = expr_car(s.value);
    Expr *cdr = expr_cdr(s.value);
    while(cdr) {
        car = expr_car(cdr);
        cdr = expr_cdr(cdr);
        sum += expr_get_int(eval_value(scope(car, s.env)).value);
    }
    return scope(expr_int(sum), s.env);
}

static Scope eval_let(Scope s) {
    Expr *cdr = expr_cdr(s.value);
    Expr *key = expr_car(cdr);
    cdr = expr_cdr(cdr);
    Expr *value = expr_car(cdr);
    cdr = expr_cdr(cdr);
    Expr *env2 = env_add(s.env, key, eval_value(scope(value, s.env)).value);
    return scope(0, env2);
}

static Scope eval_do(Scope s) {
    Expr *cdr = expr_cdr(s.value);
    Expr *ret = 0;
    Expr *env = s.env;
    while(cdr) {
        Expr *car = expr_car(cdr);
        Scope rets = eval_value(scope(car, env));
        env = rets.env;
        ret = rets.value;
        cdr = expr_cdr(cdr);
    }
    return scope(ret, s.env);
}

// Get environment
static Scope eval_env_get(Scope s) {
    return scope(s.env, s.env);
}

// Set environment
static Scope eval_env_set(Scope s) {
    Expr *car = expr_car(expr_cdr(s.value));
    return scope(0, car);
}

// Set environment
static Scope eval_print(Scope s) {
    Expr *cdr = expr_cdr(s.value);
    while(cdr) {
        Expr *car = expr_car(cdr);
        car = eval_value(scope(car, s.env)).value;
        pretty_value(car);
        printf(" ");
        cdr = expr_cdr(cdr);
    }
    printf("\n");
    return scope(0, s.env);
}

static Scope eval_list(Scope s) {
    Expr_Type type = expr_type(s.value);
    Expr *car = expr_car(s.value);
    Expr *cdr = expr_cdr(s.value);

    if (expr_eq(car, expr_label("add"))) return eval_add(s);
    if (expr_eq(car, expr_label("quote"))) return eval_quote(s);
    if (expr_eq(car, expr_label("do"))) return eval_do(s);
    if (expr_eq(car, expr_label("let"))) return eval_let(s);
    if (expr_eq(car, expr_label("print"))) return eval_print(s);
    if (expr_eq(car, expr_label("fn"))) return s;
    if (expr_eq(car, expr_label("env?"))) return eval_env_get(s);
    if (expr_eq(car, expr_label("env!"))) return eval_env_set(s);

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

    // return e;
    return s;
}
