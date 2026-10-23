#pragma once
#include "lisp_type.h"

#define LISP_VALUE_COUNT (1024 * 1024)
typedef struct {
    u32 count;
    LispValue values[LISP_VALUE_COUNT];
} LispHeap;

static LispHeap lisp_heap;

static LispPtr lisp_alloc(LispType type) {
    u32 index = ++lisp_heap.count;
    return (LispPtr){type, index};
}

static LispValue *lisp_value(LispPtr ptr) {
    return lisp_heap.values + ptr.index;
}
