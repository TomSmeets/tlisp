// #include "ast.h"
// #include "error.h"
#include "eval.h"
#include "parse.h"
#include "pretty.h"
#include "test.h"

static char *mem_read_file(char *path) {
    FILE *f = fopen(path, "r");
    void *ptr = heap_data + heap_used;
    heap_used += fread(ptr, 1, sizeof(heap_data) - heap_used, f);
    heap_data[heap_used++] = 0;
    return (char *)ptr;
}

int main(int argc, char **argv) {
    test();

    if (argc != 2) {
        printf("usage: tlisp [SOURCE]\n");
        return 1;
    }
    char *path = argv[1];
    printf("Path: %s\n", path);

    char *source = mem_read_file(path);
    printf("Source: %s\n", source);

    Expr p = parse(source);
    printf("Parse: ");
    pretty_value(p);
    printf("\n");

    Expr env = expr_nil();
    eval_add_builtins(&env);
    Expr e = eval_value(&env, p);

    printf("EVAL: ");
    pretty_value(e);
    printf("\n");

    // test();
    // eval_value(scope(parse(source), 0));
    return 0;
}
