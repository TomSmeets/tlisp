#pragma once
#include "lisp_heap.h"

// Constructors
static LispPtr lisp_new_cons(LispPtr car, LispPtr cdr) {
    LispPtr ptr = lisp_alloc(LISP_TYPE_CONS);
    LispValue *val = lisp_value(ptr);
    val->cons.car = car;
    val->cons.cdr = cdr;
    return ptr;
}

static LispPtr lisp_new_number(i64 number) {
    LispPtr ptr = lisp_alloc(LISP_TYPE_NUMBER);
    LispValue *val = lisp_value(ptr);
    val->number = number;
    return ptr;
}

static LispPtr lisp_new_string(char *str) {
    u32 len = strlen(str);
    LispPtr cdr = LISP_NIL;
    for (u32 i = 0; i < len; ++i) {
        LispPtr ptr = lisp_alloc(LISP_TYPE_STRING);
        LispValue *val = lisp_value(ptr);
        val->string.data = str[len - i - 1];
        val->string.cdr = cdr;
        cdr = ptr;
    }

    return cdr;
}

static LispPtr lisp_new_label(char *str) {
    u32 len = strlen(str);
    LispPtr cdr = LISP_NIL;
    for (u32 i = 0; i < len; ++i) {
        LispPtr ptr = lisp_alloc(LISP_TYPE_LABEL);
        LispValue *val = lisp_value(ptr);
        val->label.data = str[len - i - 1];
        val->label.cdr = cdr;
        cdr = ptr;
    }
    return cdr;
}

static void lisp_push(LispPtr *list, LispPtr item) {
    *list = lisp_new_cons(item, *list);
}

static LispPtr lisp_pop(LispPtr *list) {
    LispValue *cons = lisp_value(*list);
    LispPtr car = cons->cons.car;
    LispPtr cdr = cons->cons.cdr;
    *list = cdr;
    return car;
}
