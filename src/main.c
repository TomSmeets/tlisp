#include "lisp_new.h"
#include "lisp_parse.h"
#include "lisp_pretty.h"
#include "read.h"
#include "type.h"

int main(void) {
    LispPtr list = LISP_NIL;
    lisp_push(&list, lisp_new_number(1));
    lisp_push(&list, lisp_new_number(2));
    lisp_push(&list, lisp_new_label("three"));
    lisp_push(&list, lisp_new_string("Four"));
    lisp_push(&list, LISP_NIL);

    printf("\n=== Print ===\n");
    lisp_pretty_print("NIL:  ", LISP_NIL, "\n");
    lisp_pretty_print("CONS: ", lisp_new_cons(LISP_NIL, LISP_NIL), "\n");
    lisp_pretty_print("NUM:  ", lisp_new_number(69), "\n");
    lisp_pretty_print("STR:  ", lisp_new_string("Hello World"), "\n");
    lisp_pretty_print("LAB:  ", lisp_new_label("hello-world"), "\n");
    lisp_pretty_print("LIST: ", list, "\n");

    printf("\n=== Parse ===\n");
    lisp_pretty_print("NIL:  ", lisp_parse("()"), "\n");
    lisp_pretty_print("CONS: ", lisp_parse("(1)"), "\n");
    lisp_pretty_print("NUM:  ", lisp_parse("69"), "\n");
    lisp_pretty_print("STR:  ", lisp_parse("\"Hello World\""), "\n");
    lisp_pretty_print("LAB:  ", lisp_parse("hello-world"), "\n");
    lisp_pretty_print("LIST: ", lisp_parse("(1 2 three \"Four\")"), "\n");
}
