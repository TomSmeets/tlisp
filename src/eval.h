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
static Expr eval_value(Expr *env, Expr value);
static Expr eval_list(Expr *env, Expr list);

// Return argument without evaluating
static Expr eval_quote(Expr *env, Expr args) {
    Expr arg = expr_pop(&args);
    assert(expr_is_nil(args));
    return arg;
}

// Sum all arguments
static Expr eval_add(Expr *env, Expr args) {
    i64 sum = 0;
    while (!expr_is_nil(args)) {
        Expr car = eval_value(env, expr_pop(&args));
        sum += expr_get_value(car);
    }
    return expr_value(sum);
}

// (let x y)
static Expr eval_let(Expr *env, Expr args) {
    Expr key = expr_pop(&args);
    Expr value = expr_pop(&args);
    assert(expr_is_nil(args));

    // Evaluate value
    value = eval_value(env, value);

    // Add to environment
    env_add(env, key, value);

    // Return nil
    return expr_nil();
}

// (do ...) Scoped block, returns last value
static Expr eval_do(Expr *env, Expr args) {
    Expr inner = *env;
    for (;;) {
        Expr result = eval_value(&inner, expr_pop(&args));
        if (expr_is_nil(args)) return result;
    }
}

static Expr eval_cons(Expr *env, Expr args) {
    Expr arg1 = eval_value(env, expr_pop(&args));
    Expr arg2 = eval_value(env, expr_pop(&args));
    assert(expr_is_nil(args));
    return expr_cons(arg1, arg2);
}

static Expr eval_car(Expr *env, Expr args) {
    Expr arg1 = eval_value(env, expr_pop(&args));
    assert(expr_is_nil(args));
    return expr_get_car(arg1);
}

static Expr eval_cdr(Expr *env, Expr args) {
    Expr arg1 = eval_value(env, expr_pop(&args));
    assert(expr_is_nil(args));
    return expr_get_cdr(arg1);
}

// Get current scope (fun meta function)
static Expr eval_env_get(Expr *env, Expr args) {
    assert(expr_is_nil(args));
    return *env;
}

// Set current scope (fun meta function)
static Expr eval_env_set(Expr *env, Expr args) {
    Expr arg1 = eval_value(env, expr_pop(&args));
    assert(expr_is_nil(args));
    *env = arg1;
    return expr_nil();
}

static Expr eval_print(Expr *env, Expr args) {
    for (;;) {
        Expr car = eval_value(env, expr_pop(&args));
        pretty_value(car);

        if (expr_is_nil(args)) {
            printf("\n");
            return expr_nil();
        }

        printf(" ");
    }
}

// ((fnapp (x y z) env body) x y z)
static Expr eval_fnapp(Expr *env, Expr fn, Expr args) {
    Expr fn_kw = expr_pop(&fn);
    Expr fn_args = expr_pop(&fn);
    Expr fn_env = expr_pop(&fn);
    Expr fn_body = expr_pop(&fn);
    assert(expr_is_nil(fn));

    // Add to function env
    while (!expr_is_nil(args) && !expr_is_nil(fn_args)) {
        Expr arg_name = expr_pop(&fn_args);
        Expr arg_value = eval_value(env, expr_pop(&args));

        // Add to env
        env_add(&fn_env, arg_name, arg_value);
    }
    assert(expr_is_nil(args));
    assert(expr_is_nil(fn_args));

    // eval body
    return eval_value(&fn_env, fn_body);
}

// (fn args body)
static Expr eval_fn(Expr *env, Expr args) {
    Expr fn_args = expr_pop(&args);
    Expr fn_body = expr_pop(&args);
    assert(expr_is_nil(args));

    // (fnapp args env body)
    Expr out = expr_nil();
    out = expr_cons(fn_body, out);
    out = expr_cons(*env, out);
    out = expr_cons(fn_args, out);
    out = expr_cons(expr_builtin((void *)eval_fnapp), out);
    return out;
}

// (def name args body)
static Expr eval_def(Expr *env, Expr args) {
    Expr fn_name = expr_pop(&args);
    Expr fn_args = expr_pop(&args);
    Expr fn_body = expr_pop(&args);
    assert(expr_is_nil(args));

    Expr let_body = expr_nil();
    expr_push(&let_body, fn_body);
    expr_push(&let_body, fn_args);
    expr_push(&let_body, expr_builtin(eval_fn));

    Expr let_expr = expr_nil();
    expr_push(&let_expr, let_body);
    expr_push(&let_expr, fn_name);
    expr_push(&let_expr, expr_builtin(eval_let));

    // (let name (fn args body))
    return eval_value(env, let_expr);
}

// (list ...)
static Expr eval_builtin_list(Expr *env, Expr args) {
    if (expr_is_nil(args)) return args;
    Expr car = eval_value(env, expr_get_car(args));
    Expr cdr = expr_get_cdr(args);
    return expr_cons(car, eval_builtin_list(env, cdr));
}

// (readfile name)
static Expr eval_builtin_readfile(Expr *env, Expr args) {
    Expr arg0 = eval_value(env, expr_pop(&args));
    assert(expr_is_nil(args));

    // Extract path
    char path[1024];
    expr_get_str(arg0, sizeof(path), path);

    // Read bytes
    FILE *f = fopen(path, "rb");
    Expr first = expr_nil();
    Expr last = expr_nil();

    for (;;) {
        char byte = 0;
        int n = fread(&byte, 1, 1, f);
        if (n != 1) break;
        expr_append(&first, &last, expr_value(byte));
    }

    return first;
}

static void eval_add_builtins(Expr *env) {
    env_add(env, expr_str("quote"), expr_builtin(eval_quote));
    env_add(env, expr_str("add"), expr_builtin(eval_add));
    env_add(env, expr_str("let"), expr_builtin(eval_let));
    env_add(env, expr_str("do"), expr_builtin(eval_do));
    env_add(env, expr_str("cons"), expr_builtin(eval_cons));
    env_add(env, expr_str("car"), expr_builtin(eval_car));
    env_add(env, expr_str("cdr"), expr_builtin(eval_cdr));
    env_add(env, expr_str("env?"), expr_builtin(eval_env_get));
    env_add(env, expr_str("env!"), expr_builtin(eval_env_set));
    env_add(env, expr_str("print"), expr_builtin(eval_print));
    env_add(env, expr_str("list"), expr_builtin(eval_builtin_list));
    env_add(env, expr_str("fn"), expr_builtin(eval_fn));
    env_add(env, expr_str("def"), expr_builtin(eval_def));
    env_add(env, expr_str("readfile"), expr_builtin(eval_builtin_readfile));
    // env_add(env, expr_str("cons?"), expr_builtin(eval_is_cons));
    // env_add(env, expr_str("nil?"),  expr_builtin(eval_is_nil));
}

static Expr eval_value(Expr *env, Expr value) {
    // nil / value / builtin
    if (expr_get_type(value) != EXPR_TYPE_CONS) return value;

    // Label
    bool error = false;
    if (env_search(*env, value, &value)) return value;

    // Function application
    return eval_list(env, value);
}

// Function evaluation
static Expr eval_list(Expr *env, Expr list) {
    // Must be a list
    assert(expr_get_type(list) == EXPR_TYPE_CONS);

    // car -> function
    // cdr -> arguments
    // Eval function name -> will deref any labels, eval any expression and result in either a builtin (N ...) or ((fn ..) ..)
    Expr name = eval_value(env, expr_get_car(list));
    Expr args = expr_get_cdr(list);

    // Should be either a builtin, represented by a number: (123 ..)
    // printf("Eval_List: list=");
    // pretty_value(list);
    // printf(" name=");
    // pretty_value(name);
    // printf("\n");
    if (expr_get_type(name) == EXPR_TYPE_BUILTIN) {
        if (expr_get_builtin(name) == (void *)eval_fnapp) return list;
        return expr_get_builtin(name)(env, args);
    } else {
        // Must be in the form ((fn ..) ..)
        assert(expr_get_type(name) == EXPR_TYPE_CONS);
        assert(expr_get_type(expr_get_car(name)) == EXPR_TYPE_BUILTIN);
        assert(expr_get_builtin(expr_get_car(name)) == (void *)eval_fnapp);
        return eval_fnapp(env, name, args);
    }
    return expr_nil();
}
