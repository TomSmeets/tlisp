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

    Expr env = expr_nil();
    eval_add_builtins(&env);
    Expr expect_exp = parse(expect);
    Expr eval = eval_value(&env, input_exp);
    if (!expr_eq(eval, expect_exp)) {
        printf("!!!!! FAIL !!!!!\n");
        printf("Test:  ");
        pretty_value(input_exp);
        printf("\n");
        printf("Eval:  ");
        pretty_value(eval);
        printf("\n");
        printf("Expect: ");
        pretty_value(expect_exp);
        printf("\n");
        printf("\n");
    }
}

static void test(void) {
    test_parse("1", expr_value(1));
    test_parse("-1", expr_value(-1));
    test_parse("(hello world-1 5)", expr_cons(expr_str("hello"), expr_cons(expr_str("world-1"), expr_cons(expr_value(5), expr_nil()))));
    test_parse("(\"hello world-1 5\")", expr_cons(expr_str("hello world-1 5"), expr_nil()));
    test_parse("12345678", expr_value(12345678));
    test_parse("()", expr_nil());
    test_parse("(1 . 2)", expr_cons(expr_value(1), expr_value(2)));
    test_parse("(add 1 2)", expr_cons(expr_str("add"), expr_cons(expr_value(1), expr_cons(expr_value(2), expr_nil()))));

    test_eval("1", "1");
    test_eval("(add 1 2 -4)", "-1");
    test_eval("(sub 1 2 3)", "-4");
    test_eval("(eq 1 1)", "1");
    test_eval("(eq 1 2)", "0");
    test_eval("(eq 2 1)", "0");
    test_eval("(not 1)", "0");
    test_eval("(not (eq 1 1))", "0");
    test_eval("(eq () 1)", "0");
    test_eval("(eq (list 1 2) (cons 1 (cons 2 ())))", "1");
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
    test_eval("(if (add 1 1) 123 456)", "123");
    test_eval("(if (add 0 0) 123 456)", "456");
    test_eval(
        "(let i 5)"
        "(let j 1)"
        "(let xs ())"
        "(while i (do"
        "  (set i (sub i 1))"
        "  (set j (mul j 2))"
        "  (print (list i j))"
        "  (set xs (cons (list i j) xs))"
        "))"
        "xs",
        "((0 32) (1 16) (2 8) (3 4) (4 2))"
    );
}
