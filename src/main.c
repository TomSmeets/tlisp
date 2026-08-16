#include "ast.h"
#include "parse.h"
#include "pretty.h"
#include "test.h"

int main(void) {
    test();
    Parse p = {.cursor = "(do (let x 5) (let y 8) (add x y))"};
    Expr *prog2 = parse_value(&p);
    Expr *a = expr_label("let");
    Expr *b = expr_label("x");
    Expr *c = expr_int(5);
    Expr *e = expr_cons(a, expr_cons(b, expr_cons(c, 0)));

    Expr *a1 = expr_label("let");
    Expr *b1 = expr_label("y");
    Expr *c1 = expr_int(8);
    Expr *e1 = expr_cons(a1, expr_cons(b1, expr_cons(c1, 0)));

    Expr *a2 = expr_label("add");
    Expr *b2 = expr_label("x");
    Expr *c2 = expr_label("y");
    Expr *e2 = expr_cons(a2, expr_cons(b2, expr_cons(c2, 0)));
    Expr *prog = expr_cons(expr_label("do"), expr_cons(e, expr_cons(e1, expr_cons(e2, 0))));

    pretty_value(prog2);
    printf("\n");
    pretty_value(prog);
    printf("\n");

    eval_value(parse("(print 1 2 3 4)"), 0);

    return 0;
}
