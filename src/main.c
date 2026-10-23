#include "type.h"
#include "lisp_pretty.h"
#include "lisp_new.h"

int main(void) {
    LispPtr list = LISP_NIL;
    lisp_push(&list, lisp_new_number(1));
    lisp_push(&list, lisp_new_number(2));
    lisp_push(&list, lisp_new_label("three"));
    lisp_push(&list, lisp_new_string("Four"));
    lisp_push(&list, LISP_NIL);

    lisp_pretty_print("NIL:  ", LISP_NIL, "\n");
    lisp_pretty_print("CONS: ", lisp_new_cons(LISP_NIL, LISP_NIL), "\n");
    lisp_pretty_print("NUM:  ", lisp_new_number(69), "\n");
    lisp_pretty_print("STR:  ", lisp_new_string("Hello World"), "\n");
    lisp_pretty_print("LAB:  ", lisp_new_label("hello-world"), "\n");
    lisp_pretty_print("LIST: ", list, "\n");
}
