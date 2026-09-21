#pragma once
#include "eval.h"
#include "parse.h"
#include "pretty.h"

static void test_parse(char *input, Expr expect) {
    Expr input_exp = parse(input);
    if (!expr_eq(input_exp, expect)) {
        printf("FAIL:\n");
        printf("  Input:  %s\n", input);
        printf("  Parse:  ");
        pretty_value(input_exp);
        printf("\n");
        printf("  Expect: ");
        pretty_value(expect);
        printf("\n");
    }
}

static void test_eval(char *input, char *expect) {
    Expr input_exp = parse(input);
    printf("Test:  ");
    pretty_value(input_exp);
    printf("\n");

    Expr env = expr_nil();
    eval_add_builtins(&env);
    Expr expect_exp = parse(expect);
    Expr eval = eval_value(&env, input_exp);
    printf("Eval:  ");
    pretty_value(eval);
    printf("\n");
    if (!expr_eq(eval, expect_exp)) {
        printf("Expect: ");
        pretty_value(expect_exp);
        printf("\n");
        printf("!!!!! FAIL !!!!!\n");
    }
    printf("\n");
}

static void test(void) {
    test_parse("1", expr_value(1));
    test_parse("-1", expr_value(-1));
    test_parse("12345678", expr_value(12345678));
    test_parse("()", expr_nil());
    test_parse("(1 . 2)", expr_cons(expr_value(1), expr_value(2)));
    test_parse("(add 1 2)", expr_cons(expr_str("add"), expr_cons(expr_value(1), expr_cons(expr_value(2), expr_nil()))));

    test_eval("1", "1");
    test_eval("(add 1 2 -4)", "-1");
    test_eval("(add (add 1 2) 3 (add 4 5 6) 7)", "28");
    test_eval("(quote (add 1 2))", "(add 1 2)");
    test_eval("(quote (1 . 2))", "(1 . 2)");
    test_eval("(quote (1 2 . 3))", "(1 2 . 3)");
    test_eval("(let x 5) x", "5");
    test_eval("(let x 2) (let y -1) (add x x y y)", "2");
    test_eval("(car (quote (1 2 3)))", "1");
    test_eval("(cdr (quote (1 2 3)))", "(2 3)");
    test_eval("((fn (x) (add x x)) 2)", "4");
    test_eval("((fn (x y) (add x y)) 1 2)", "3");
    test_eval("(def inc (x) (add x 1)) (inc 4)", "5");
    test_eval("(let f (fn (a) (add a a))) (f 3)", "6");
    test_eval(
        "(let a 1)"
        "(let b 2)"
        "(let f (fn () (add a b)))"
        "(f)",
        "3"
    );
    test_eval(
        "(let a 1)"
        "(let b 2)"
        "(let f (fn (x) (fn (y) (add x y))))"
        "((f a) b)",
        "3"
    );
    test_eval(
        "(let a 1)"
        "(let b a)"
        "(let f (fn () a))"
        "(let a 2)"
        "(list a b (f))",
        "(2 1 1)"
    );
}
