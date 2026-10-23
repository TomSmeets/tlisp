#pragma once
#include "type.h"

typedef enum {
    LISP_TYPE_NIL,
    LISP_TYPE_CONS,
    LISP_TYPE_NUMBER,
    LISP_TYPE_STRING,
    LISP_TYPE_LABEL,
    LISP_TYPE_BUILTIN,
} LispType;

typedef struct {
    u32 type : 3;
    u32 index : 29;
} LispPtr;

typedef union {
    struct {
        LispPtr car;
        LispPtr cdr;
    } cons;

    i64 number;

    struct {
        u32 data;
        LispPtr cdr;
    } string, label;

    void *builtin;
} LispValue;

static const LispPtr LISP_NIL = {};

static LispType lisp_type(LispPtr ptr) {
    return ptr.type;
}
