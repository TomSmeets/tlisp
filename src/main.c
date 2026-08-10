#include "eval.h"
#include "mem.h"
#include "parse.h"
#include "pretty.h"
#include "str.h"
#include "type.h"

int main(void) {
    char *code = (void *)heap_data;
    FILE *f = fopen("src/input.lisp", "r");
    heap_used += fread(heap_data, 1, sizeof(heap_data) - heap_used, f);
    heap_data[heap_used++] = 0;
    fclose(f);

    printf("Code: %s\n", code);
    printf("Mem: %lu\n", heap_used);
    Parse p = {.cursor = (char *)code};
    Expr *e = parse_value(&p);
    pretty_value(e);
    printf("\n");
    pretty_value(eval_value(e, 0));
    printf("\n");
    printf("Mem: %lu\n", heap_used);
}
