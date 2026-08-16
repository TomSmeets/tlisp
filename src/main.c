#include "ast.h"
#include "parse.h"
#include "pretty.h"
#include "test.h"

int main(void) {
    FILE *f = fopen("src/input.lisp", "r");
    heap_used = fread(heap_data, 1, sizeof(heap_data), f);
    heap_data[heap_used++] = 0;
    char *source = (char *)&heap_data[0];

    printf("Source: %s\n", source);
    Expr *p = parse(source);
    printf("Parse: ");
    pretty_value(p);
    printf("\n");

    Expr *e = eval_value(scope(p, 0)).value;
    pretty_value(e);
    printf("\n");

    test();
    // eval_value(scope(parse(source), 0));
    return 0;
}
