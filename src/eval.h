// Evaluate tlisp expressions
#pragma once
#include "ast.h"
#include "env.h"
#include "error.h"
#include "pretty.h"
#include "str.h"
#include <assert.h>

// ================
// Eval
// ================

// A pair of value and environment
typedef struct {
    Expr *value;
    Expr *env;
} Scope;

static Scope scope(Expr *value, Expr *env) {
    return (Scope){value, env};
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
    while (cdr) {
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
    while (cdr) {
        Expr *car = expr_car(cdr);
        Scope rets = eval_value(scope(car, env));
        env = rets.env;
        ret = rets.value;
        cdr = expr_cdr(cdr);
    }
    return scope(ret, s.env);
}

static Expr *eval_builtin_list2(Expr *expr, Expr *env) {
    if (!expr) return expr;

    if (expr_type(expr) == Expr_Cons) {
        Expr *car = eval_value(scope(expr_car(expr), env)).value;
        Expr *cdr = expr_cdr(expr);
        return expr_cons(car, eval_builtin_list2(cdr, env));
    } else {
        return eval_value(scope(expr, env)).value;
    }
}

static Scope eval_builtin_list(Scope s) {
    return scope(eval_builtin_list2(expr_cdr(s.value), s.env), s.env);
}

static Expr *eval_builtin_cons(Expr *value, Expr *env) {
    // Drop keyword
    value = expr_cdr(value);
    Expr *arg1 = expr_car(value);
    value = expr_cdr(value);
    Expr *arg2 = expr_car(value);
    value = expr_cdr(value);
    assert(value == 0);
    return expr_cons(eval_value(scope(arg1, env)).value, eval_value(scope(arg2, env)).value);
}

static Expr *eval_builtin_car(Expr *value, Expr *env) {
    // Drop keyword
    value = expr_cdr(value);
    Expr *arg1 = eval_value(scope(expr_car(value), env)).value;
    value = expr_cdr(value);
    assert(value == 0);
    return expr_car(arg1);
}

static Expr *eval_builtin_cdr(Expr *value, Expr *env) {
    // Drop keyword
    value = expr_cdr(value);
    Expr *arg1 = eval_value(scope(expr_car(value), env)).value;
    value = expr_cdr(value);
    assert(value == 0);
    return expr_cdr(arg1);
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
    while (cdr) {
        Expr *car = expr_car(cdr);
        car = eval_value(scope(car, s.env)).value;
        pretty_value(car);
        printf(" ");
        cdr = expr_cdr(cdr);
    }
    printf("\n");
    return scope(0, s.env);
}

// Set environment
static Scope eval_fn(Scope s) {
    Expr *exp = s.value;

    Expr *label = expr_car(s.value);
    Expr *rest = expr_cdr(s.value);

    // Add env between fn keyword and the rest
    return scope(expr_cons(label, expr_cons(s.env, rest)), s.env);
}

static Scope eval_list(Scope s) {
    Expr_Type type = expr_type(s.value);
    assert(type == Expr_Cons);

    Expr *car = expr_car(s.value);
    Expr *cdr = expr_cdr(s.value);
    assert(car);

    // Builtin
    if (expr_eq(car, expr_label("add"))) return eval_add(s);
    if (expr_eq(car, expr_label("quote"))) return eval_quote(s);
    if (expr_eq(car, expr_label("list"))) return eval_builtin_list(s);
    if (expr_eq(car, expr_label("cons"))) return scope(eval_builtin_cons(s.value, s.env), s.env);
    if (expr_eq(car, expr_label("car"))) return scope(eval_builtin_car(s.value, s.env), s.env);
    if (expr_eq(car, expr_label("cdr"))) return scope(eval_builtin_cdr(s.value, s.env), s.env);
    if (expr_eq(car, expr_label("do"))) return eval_do(s);
    if (expr_eq(car, expr_label("let"))) return eval_let(s);
    if (expr_eq(car, expr_label("print"))) return eval_print(s);
    if (expr_eq(car, expr_label("fn"))) return eval_fn(s);
    if (expr_eq(car, expr_label("env?"))) return eval_env_get(s);
    if (expr_eq(car, expr_label("env!"))) return eval_env_set(s);

    // Eval
    // // (X y . .)
    // // (fn (x y z) (+ x y z))
    Expr *lam = eval_value(scope(car, s.env)).value;
    if (expr_eq(expr_car(lam), expr_label("fn"))) {
        lam = expr_cdr(lam);
        Expr *body_env = expr_car(lam);
        lam = expr_cdr(lam);
        Expr *args = expr_car(lam);
        lam = expr_cdr(lam);
        Expr *body = expr_car(lam);
        lam = expr_cdr(lam);
        assert(lam == 0);

        // Iterate over arguments and create a new env for the function body
        Expr *arg_values = cdr;
        Expr *arg_names = args;

        for (;;) {
            if (arg_names == 0 && arg_values == 0) break;
            assert(arg_names != 0);
            assert(arg_values != 0);

            Expr *arg_name = expr_car(arg_names);
            assert(expr_type(arg_name) == Expr_Label);

            Expr *arg_value = eval_value(scope(expr_car(arg_values), s.env)).value;
            body_env = env_add(body_env, arg_name, arg_value);
            arg_names = expr_cdr(arg_names);
            arg_values = expr_cdr(arg_values);
        }

        Expr *result = eval_value(scope(body, body_env)).value;
        return scope(result, s.env);
    }

    // return e;
    return s;
}
