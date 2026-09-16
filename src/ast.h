#pragma once
#include "type.h"
#include <assert.h>
#include <sys/mman.h>

// Label  -> String
// String -> List of Char
// char   -> int
// nil    -> '0'

typedef enum {
    Expr_Cons,
    Expr_Value,
    Expr_Nil,
} Expr_Type;

typedef u32 Expr;

// API
static Expr_Type expr_get_type(Expr ptr);
static Expr expr_get_car(Expr ptr);
static Expr expr_get_cdr(Expr ptr);
static i64 expr_get_value(Expr ptr);
static bool expr_get_mark(Expr ptr);

static void expr_set_car(Expr ptr, Expr car);
static void expr_set_cdr(Expr ptr, Expr cdr);
static void expr_set_val(Expr ptr, i64 val);
static void expr_set_mark(Expr ptr, bool mark);

static void expr_free(Expr ptr);
static Expr expr_alloc(Expr_Type type);

static Expr expr_cons(Expr car, Expr cdr) {
    Expr ix = expr_alloc(Expr_Cons);
    expr_set_car(ix, car);
    expr_set_cdr(ix, cdr);
    return ix;
}

static Expr expr_value(i64 value) {
    Expr ix = expr_alloc(Expr_Value);
    expr_set_val(ix, value);
    return ix;
}

// Create a list of values
static Expr expr_bytes(size_t len, u8 *data) {
    Expr list = 0;
    for (size_t i = 0; i < len; ++i) {
        list = expr_cons(expr_value(data[len - i - 1]), list);
    }
    return list;
}

// Create a list of chars (string)
static Expr expr_str(char *str) {
    return expr_bytes(strlen(str), (u8 *)str);
}

static bool expr_eq(Expr a, Expr b) {
    if (a == b) return true;
    Expr_Type type = expr_get_type(a);
    if (expr_get_type(a) != expr_get_type(b)) return false;
    if (type == Expr_Nil) {
        return true;
    } else if (type == Expr_Cons) {
        if (!expr_eq(expr_get_car(a), expr_get_car(b))) return false;
        if (!expr_eq(expr_get_cdr(a), expr_get_cdr(b))) return false;
        return true;
    } else {
        return expr_get_value(a) == expr_get_value(b);
    }
}

// internals
// TODO: encode type int
typedef struct {
    u8 mark;
    u8 type;
    union {
        struct {
            Expr car, cdr;
        };
        i64 value;
    };
} Expr_Int;

static u32 expr_count = 1;
static Expr expr_freelist = 0;
static Expr_Int expr_heap[1024 * 64];

static Expr expr_alloc(Expr_Type type) {
    if (type == Expr_Nil) return 0;

    Expr ix;
    if (expr_freelist == 0) {
        assert(expr_count < array_count(expr_heap));
        ix = expr_count++;
    } else {
        ix = expr_freelist;
        expr_freelist = expr_get_cdr(expr_freelist);
    }

    expr_heap[ix].type = type;
    expr_heap[ix].mark = 0;
    expr_heap[ix].value = 0;
    return ix;
}

static void expr_free(Expr ptr) {
    if (ptr == 0) return;
    expr_heap[ptr].mark = 0;
    expr_heap[ptr].type = Expr_Cons;
    expr_heap[ptr].car = 0;
    expr_heap[ptr].cdr = expr_freelist;
    expr_freelist = ptr;
}

static Expr_Type expr_get_type(Expr ptr) {
    if (ptr == 0) return Expr_Nil;
    return expr_heap[ptr].type;
}

static Expr expr_get_car(Expr ptr) {
    assert(expr_get_type(ptr) == Expr_Cons);
    return expr_heap[ptr].car;
}

static Expr expr_get_cdr(Expr ptr) {
    assert(expr_get_type(ptr) == Expr_Cons);
    return expr_heap[ptr].cdr;
}

static i64 expr_get_value(Expr ptr) {
    assert(expr_get_type(ptr) == Expr_Value);
    return expr_heap[ptr].value;
}

static bool expr_get_mark(Expr ptr) {
    return expr_heap[ptr].mark;
}

static void expr_set_car(Expr ptr, Expr car) {
    assert(expr_get_type(ptr) == Expr_Cons);
    expr_heap[ptr].car = car;
}

static void expr_set_cdr(Expr ptr, Expr cdr) {
    assert(expr_get_type(ptr) == Expr_Cons);
    expr_heap[ptr].cdr = cdr;
}

static void expr_set_val(Expr ptr, i64 value) {
    assert(expr_get_type(ptr) == Expr_Value);
    expr_heap[ptr].value = value;
}

static void expr_set_mark(Expr ptr, bool mark) {
    expr_heap[ptr].mark = mark;
}

// GC
static void expr_mark(Expr expr) {
    // Nil
    if (expr == 0) return;

    // Already marked
    if (expr_get_mark(expr)) return;

    // Mark expression
    expr_set_mark(expr, 1);
    expr_mark(expr_get_car(expr));
    expr_mark(expr_get_cdr(expr));
}

static void expr_sweep(void) {
    expr_freelist = 0;
    for (Expr ptr = expr_count - 1; ptr >= 0; --ptr) {

        // Object is in use
        if (expr_get_mark(ptr)) {
            expr_set_mark(ptr, 0);
            continue;
        }

        // Object is free
        expr_free(ptr);
    }
}
