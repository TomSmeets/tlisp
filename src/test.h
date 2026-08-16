#pragma once
#include "eval.h"
#include "parse.h"
#include "pretty.h"

static void test_parse(char *input, Expr *expect) {
    Expr *input_exp = parse(input);
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
    Expr *input_exp = parse(input);
    Expr *expect_exp = parse(expect);
    Expr *eval = eval_value(scope(input_exp, 0)).value;
    if (!expr_eq(eval, expect_exp)) {
        printf("FAIL:\n");
        printf("  Input:  %s\n", input);
        printf("  Parse:  ");
        pretty_value(input_exp);
        printf("\n");
        printf("  Expect: ");
        pretty_value(expect_exp);
        printf("\n");
        printf("  Eval:   ");
        pretty_value(eval);
        printf("\n");
    }
}

static void test(void) {
    test_parse("1", expr_int(1));
    test_parse("-1", expr_int(-1));
    test_parse("12345678", expr_int(12345678));
    test_parse("()", 0);
    test_parse("(1 . 2)", expr_cons(expr_int(1), expr_int(2)));
    test_parse("(add 1 2)", expr_cons(expr_label("add"), expr_cons(expr_int(1), expr_cons(expr_int(2), 0))));

    test_eval("1", "1");
    test_eval("(add 1 2 -4)", "-1");
    test_eval("(add (add 1 2) 3 (add 4 5 6) 7)", "28");
    test_eval("(quote (add 1 2))", "(add 1 2)");
    test_eval("(1 . 2)", "(1 . 2)");
    test_eval("(do (let x 5) x)", "5");
    test_eval("(do (let x 2) (let y -1) (add x x y y))", "2");
}
