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
    Expr env;
} Scope;

static Expr eval_value(Expr *env, Expr value);
static Expr eval_list(Expr *env, Expr list);

// Return argument without evaluating
static Expr eval_quote(Expr *env, Expr args) {
    Expr arg = expr_get_car(args);
    args = expr_get_cdr(args);
    assert(args == 0);
    return arg;
}

// Sum all arguments
static Expr eval_add(Expr *env, Expr args) {
    i64 sum = 0;
    while (args) {
        Expr car = eval_value(env, expr_get_car(args));
        sum += expr_get_value(car);
        args = expr_get_cdr(args);
    }
    return expr_value(sum);
}

// (let x y)
static Expr eval_let(Expr *env, Expr args) {
    Expr key = expr_get_car(args);
    args = expr_get_cdr(args);
    Expr value = expr_get_car(args);
    args = expr_get_cdr(args);
    assert(args == 0);

    // Evaluate value
    value = eval_value(env, value);

    // Add to environment
    env_add(env, key, value);

    // Return nil
    return 0;
}

// (do ...) Scoped block, returns last value
static Expr eval_do(Expr *env, Expr args) {
    Expr inner = *env;
    Expr result = 0;
    while (args) {
        // Evaluate expression
        result = eval_value(&inner, expr_get_car(args));

        // Advance to next argument
        args = expr_get_cdr(args);
    }
    return result;
}

static Expr eval_cons(Expr *env, Expr args) {
    Expr arg1 = eval_value(env, expr_get_car(args));
    args = expr_get_cdr(args);
    Expr arg2 = eval_value(env, expr_get_car(args));
    args = expr_get_cdr(args);
    assert(args == 0);
    return expr_cons(arg1, arg2);
}

static Expr eval_car(Expr *env, Expr args) {
    Expr arg1 = eval_value(env, expr_get_car(args));
    args = expr_get_cdr(args);
    assert(args == 0);
    return expr_get_car(arg1);
}

static Expr eval_cdr(Expr *env, Expr args) {
    Expr arg1 = eval_value(env, expr_get_car(args));
    args = expr_get_cdr(args);
    assert(args == 0);
    return expr_get_cdr(arg1);
}

// Get current scope (fun meta function)
static Expr eval_env_get(Expr *env, Expr args) {
    assert(args == 0);
    return *env;
}

// Set current scope (fun meta function)
static Expr eval_env_set(Expr *env, Expr args) {
    Expr arg1 = eval_value(env, expr_get_car(args));
    args = expr_get_cdr(args);
    assert(args == 0);
    *env = arg1;
    return 0;
}

static Expr eval_print(Expr *env, Expr args) {
    while (args) {
        Expr car = eval_value(env, expr_get_car(args));
        pretty_value(car);
        printf(" ");
        args = expr_get_cdr(args);
    }
    printf("\n");
    return 0;
}

static void eval_add_builtins(Expr *env) {
    env_add(env, expr_str("quote"), expr_builtin(eval_quote));
    env_add(env, expr_str("add"), expr_builtin(eval_add));
    env_add(env, expr_str("let"),   expr_builtin(eval_let));
    env_add(env, expr_str("do"),    expr_builtin(eval_do));
    env_add(env, expr_str("cons"),  expr_builtin(eval_cons));
    env_add(env, expr_str("car"),   expr_builtin(eval_car));
    env_add(env, expr_str("cdr"),   expr_builtin(eval_cdr));
    env_add(env, expr_str("env?"),   expr_builtin(eval_env_get));
    env_add(env, expr_str("env!"), expr_builtin(eval_env_set));
    env_add(env, expr_str("print"), expr_builtin(eval_print));
    // env_add(env, expr_str("nil?"),  expr_builtin(eval_is_nil));
    // env_add(env, expr_str("fn"),    expr_builtin(eval_fn));
    // env_add(env, expr_str("cons?"), expr_builtin(eval_is_cons));
}

static Expr eval_value(Expr *env, Expr value) {
    // nil / value / builtin
    if (expr_get_type(value) != Expr_Cons) return value;

    // Label
    bool error = false;
    if (env_search(*env, value, &value)) return value;

    // Function application
    return eval_list(env, value);
}

// Function evaluation
static Expr eval_list(Expr *env, Expr list) {
    // Must be a list
    assert(expr_get_type(list) == Expr_Cons);

    // car -> function
    // cdr -> arguments
    // Eval function name -> will deref any labels, eval any expression and result in either a builtin (N ...) or ((fn ..) ..)
    Expr name = eval_value(env, expr_get_car(list));
    Expr args = expr_get_cdr(list);

    // Should be either a builtin, represented by a number: (123 ..)
    if (expr_get_type(name) == Expr_Builtin) {
        return expr_get_builtin(name)(env, args);
    } else {
        // Must be in the form ((fn ..) ..)
        assert(expr_get_type(name) == Expr_Cons);

        // Expr fn_kw = expr_get_car(name);
        // assert(expr_get_type(fn_kw) == Expr_Value);
        // assert(expr_value(fn_kw) == EXPR_FN);
        // name = expr_get_cdr(name);

        // Expr fn_args = expr_get_car(name);
        // assert(expr_get_type(fn_args) == Expr_Cons);
        // name = expr_get_cdr(name);

        // Expr fn_body = expr_get_car(name);
        // assert(expr_get_type(fn_args) == Expr_Cons);
        // name = expr_get_cdr(name);
        // assert(name == 0);

        // TODO: apply body
        return 0;
    }

    // // or a lambda
    // // // ((fn (x y z) (+ x y z)) ... )
    // assert(expr_get_type(car) == Expr_Cons) {
    // Expr fn = expr_get_car(car);
    // assert(expr_get_type(fn) == Expr_Value);
    // assert(expr_value(fn) == EXPR_FN);

    // Expr args = expr_get_car(cdr);
    // cdr = expr_get_cdr(cdr);

    // Expr args = expr_get_car(cdr);
    // cdr = expr_get_cdr(cdr);

    // // Eval
    // Expr lam = eval_value(scope(car, s.env)).value;
    // if (expr_eq(expr_get_car(lam), expr_label("fn"))) {
    //     lam = expr_get_cdr(lam);
    //     Expr body_env = expr_get_car(lam);
    //     lam = expr_get_cdr(lam);
    //     Expr args = expr_get_car(lam);
    //     lam = expr_get_cdr(lam);
    //     Expr body = expr_get_car(lam);
    //     lam = expr_get_cdr(lam);
    //     assert(lam == 0);

    //     // Iterate over arguments and create a new env for the function body
    //     Expr arg_values = cdr;
    //     Expr arg_names = args;

    //     for (;;) {
    //         if (arg_names == 0 && arg_values == 0) break;
    //         assert(arg_names != 0);
    //         assert(arg_values != 0);

    //         Expr arg_name = expr_get_car(arg_names);
    //         assert(expr_get_type(arg_name) == Expr_Label);

    //         Expr arg_value = eval_value(scope(expr_get_car(arg_values), s.env)).value;
    //         body_env = env_add(body_env, arg_name, arg_value);
    //         arg_names = expr_get_cdr(arg_names);
    //         arg_values = expr_get_cdr(arg_values);
    //     }

    //     Expr result = eval_value(scope(body, body_env)).value;
    //     return scope(result, s.env);
    // }

    // return e;
    return 0;
}
