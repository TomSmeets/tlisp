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


// ((fnapp (x y z) env body) x y z)
static Expr eval_fnapp(Expr *env, Expr fn, Expr args) {
    // printf("Eval_fnapp:");
    // printf(" fn=");
    // pretty_value(fn);
    // printf(" args=");
    // pretty_value(args);
    // printf("\n");

    Expr fn_kw = expr_get_car(fn);
    fn = expr_get_cdr(fn);
    Expr fn_args = expr_get_car(fn);
    fn = expr_get_cdr(fn);
    Expr fn_env = expr_get_car(fn);
    fn = expr_get_cdr(fn);
    Expr fn_body = expr_get_car(fn);
    fn = expr_get_cdr(fn);
    assert(fn == 0);

    // Add to function env
    while(args && fn_args) {
        Expr arg_name = expr_get_car(fn_args);
        fn_args = expr_get_cdr(fn_args);

        Expr arg_value = eval_value(env, expr_get_car(args));
        args = expr_get_cdr(args);

        // Add to env
        env_add(&fn_env, arg_name, arg_value);
    }
    assert(args == 0);
    assert(fn_args == 0);

    // eval body
    return eval_value(&fn_env, fn_body);
}

// (fn args body)
static Expr eval_fn(Expr *env, Expr args) {
    Expr fn_args = expr_get_car(args);
    args = expr_get_cdr(args);
    Expr fn_body = expr_get_car(args);
    args = expr_get_cdr(args);
    assert(args == 0);

    // (fnapp args env body)
    Expr out = 0;
    out = expr_cons(fn_body, out);
    out = expr_cons(*env, out);
    out = expr_cons(fn_args, out);
    out = expr_cons(expr_builtin((void *)eval_fnapp), out);
    return out;
}

// (def name args body)
static Expr eval_def(Expr *env, Expr args) {
    Expr fn_name = expr_get_car(args);
    args = expr_get_cdr(args);
    Expr fn_args = expr_get_car(args);
    args = expr_get_cdr(args);
    Expr fn_body = expr_get_car(args);
    args = expr_get_cdr(args);
    assert(args == 0);


    // (let name (fn args body))
    return eval_value(
        env, expr_cons(expr_builtin(eval_let), expr_cons(fn_name, expr_cons(expr_cons(expr_builtin(eval_fn), expr_cons(fn_args, expr_cons(fn_body, 0))), 0)))
    );
}
// (list ...)
static Expr eval_builtin_list(Expr *env, Expr args) {
    if (args == 0) return 0;
    Expr car = eval_value(env, expr_get_car(args));
    Expr cdr = expr_get_cdr(args);
    return expr_cons(car, eval_builtin_list(env, cdr));
}

// (readfile name)
static Expr eval_builtin_readfile(Expr *env, Expr args) {
    char path[1024];
    expr_get_str(expr_get_car(args), sizeof(path), path);
    args = expr_get_cdr(args);
    assert(args == 0);

    FILE *f = fopen(path, "rb");
    Expr first = 0;
    Expr last = 0;

    for (;;) {
        char byte = 0;
        int n = fread(&byte, 1, 1, f);
        if (n != 1) break;
        Expr e = expr_cons(expr_value(byte), 0);

        if (last) {
            expr_set_cdr(last, e);
            last = e;
        } else {
            last = e;
            first = e;
        }
    }

    return first;
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
    env_add(env, expr_str("list"), expr_builtin(eval_builtin_list));
    env_add(env, expr_str("fn"),    expr_builtin(eval_fn));
    env_add(env, expr_str("def"),   expr_builtin(eval_def));
    env_add(env, expr_str("readfile"),   expr_builtin(eval_builtin_readfile));
    // env_add(env, expr_str("cons?"), expr_builtin(eval_is_cons));
    // env_add(env, expr_str("nil?"),  expr_builtin(eval_is_nil));
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
    // printf("Eval_List: list=");
    // pretty_value(list);
    // printf(" name=");
    // pretty_value(name);
    // printf("\n");
    if (expr_get_type(name) == Expr_Builtin) {
        if (expr_get_builtin(name) == (void *)eval_fnapp) return list;
        return expr_get_builtin(name)(env, args);
    } else {
        // Must be in the form ((fn ..) ..)
        assert(expr_get_type(name) == Expr_Cons);
        assert(expr_get_type(expr_get_car(name)) == Expr_Builtin);
        assert(expr_get_builtin(expr_get_car(name)) == (void*)eval_fnapp);
        return eval_fnapp(env, name, args);
    }
   return 0;
}
