#pragma once
#include "type.h"
#include <assert.h>
#include <sys/mman.h>

// Label  -> String
// String -> List of Char
// char   -> int
// nil    -> '0'

typedef enum {
    EXPR_TYPE_NIL,
    EXPR_TYPE_CONS,
    EXPR_TYPE_VALUE,
    EXPR_TYPE_BUILTIN,
} Expr_Type;

// A 32 bit pointer
typedef struct {
    u32 index;
} Expr;
typedef Expr (*expr_builtin_t)(Expr *env, Expr in);

// API
static Expr_Type expr_get_type(Expr ptr);
static Expr expr_get_car(Expr ptr);
static Expr expr_get_cdr(Expr ptr);
static i64 expr_get_value(Expr ptr);
static bool expr_get_mark(Expr ptr);
static expr_builtin_t expr_get_builtin(Expr ptr);

static void expr_set_car(Expr ptr, Expr car);
static void expr_set_cdr(Expr ptr, Expr cdr);
static void expr_set_val(Expr ptr, i64 val);
static void expr_set_mark(Expr ptr, bool mark);
static void expr_set_builtin(Expr ptr, expr_builtin_t fcn);

static void expr_free(Expr ptr);
static Expr expr_alloc(Expr_Type type);

static Expr expr_nil(void) {
    return expr_alloc(EXPR_TYPE_NIL);
}

static Expr expr_cons(Expr car, Expr cdr) {
    Expr e = expr_alloc(EXPR_TYPE_CONS);
    expr_set_car(e, car);
    expr_set_cdr(e, cdr);
    return e;
}

static Expr expr_value(i64 value) {
    Expr e = expr_alloc(EXPR_TYPE_VALUE);
    expr_set_val(e, value);
    return e;
}

static Expr expr_builtin(expr_builtin_t fcn) {
    Expr ix = expr_alloc(EXPR_TYPE_BUILTIN);
    expr_set_builtin(ix, fcn);
    return ix;
}

// Create a list of values
static Expr expr_bytes(size_t len, u8 *data) {
    Expr list = expr_nil();
    for (size_t i = 0; i < len; ++i) {
        list = expr_cons(expr_value(data[len - i - 1]), list);
    }
    return list;
}

// Create a list of chars (string)
static Expr expr_str(char *str) {
    return expr_bytes(strlen(str), (u8 *)str);
}

static size_t expr_get_str(Expr expr, size_t size, char *buffer) {
    int i = 0;
    while (expr_get_type(expr) != EXPR_TYPE_NIL && i + 1 < size) {
        buffer[i++] = expr_get_value(expr_get_car(expr));
        expr = expr_get_cdr(expr);
    }
    buffer[i++] = 0;
    return i;
}

static bool expr_eq(Expr a, Expr b) {
    // Same pointer
    if (a.index == b.index) return true;

    // Different types
    if (expr_get_type(a) != expr_get_type(b)) return false;

    // Nil is always equal to itself
    Expr_Type type = expr_get_type(a);
    if (type == EXPR_TYPE_NIL) return true;

    // Recurse into cons
    if (type == EXPR_TYPE_CONS) {
        if (!expr_eq(expr_get_car(a), expr_get_car(b))) return false;
        if (!expr_eq(expr_get_cdr(a), expr_get_cdr(b))) return false;
        return true;
    }

    // Value
    if (type == EXPR_TYPE_VALUE) {
        return expr_get_value(a) == expr_get_value(b);
    }

    // Builtin
    if (type == EXPR_TYPE_BUILTIN) {
        return expr_get_builtin(a) == expr_get_builtin(b);
    }

    return false;
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
        Expr (*builtin)(Expr *env, Expr e);
    };
} Expr_Int;

static u32 free_count = 0;
static Expr expr_freelist;

static u32 expr_count = 0;
static Expr_Int expr_heap[1024 * 1024];

static Expr expr_alloc(Expr_Type type) {
    Expr ix;
    if (free_count > 0) {
        ix = expr_freelist;
        expr_freelist = expr_get_cdr(expr_freelist);
        free_count--;
    } else {
        assert(expr_count < array_count(expr_heap));
        ix = (Expr){expr_count++};
    }

    Expr_Int *e = expr_heap + ix.index;
    (*e) = (Expr_Int){.type = type};
    return ix;
}

static void expr_free(Expr e) {
    Expr_Int *ev = expr_heap + e.index;
    ev->type = EXPR_TYPE_CONS;
    ev->car = e;
    ev->cdr = expr_freelist;
    expr_freelist = e;
    free_count++;
}

static Expr_Type expr_get_type(Expr ptr) {
    return expr_heap[ptr.index].type;
}

static Expr expr_get_car(Expr ptr) {
    assert(expr_get_type(ptr) == EXPR_TYPE_CONS);
    return expr_heap[ptr.index].car;
}

static Expr expr_get_cdr(Expr ptr) {
    assert(expr_get_type(ptr) == EXPR_TYPE_CONS);
    return expr_heap[ptr.index].cdr;
}

static i64 expr_get_value(Expr ptr) {
    assert(expr_get_type(ptr) == EXPR_TYPE_VALUE);
    return expr_heap[ptr.index].value;
}

static bool expr_get_mark(Expr ptr) {
    return expr_heap[ptr.index].mark;
}

static expr_builtin_t expr_get_builtin(Expr ptr) {
    assert(expr_get_type(ptr) == EXPR_TYPE_BUILTIN);
    return expr_heap[ptr.index].builtin;
}

static void expr_set_car(Expr ptr, Expr car) {
    assert(expr_get_type(ptr) == EXPR_TYPE_CONS);
    expr_heap[ptr.index].car = car;
}

static void expr_set_cdr(Expr ptr, Expr cdr) {
    assert(expr_get_type(ptr) == EXPR_TYPE_CONS);
    expr_heap[ptr.index].cdr = cdr;
}

static void expr_set_val(Expr ptr, i64 value) {
    assert(expr_get_type(ptr) == EXPR_TYPE_VALUE);
    expr_heap[ptr.index].value = value;
}

static void expr_set_mark(Expr ptr, bool mark) {
    expr_heap[ptr.index].mark = mark;
}

static void expr_set_builtin(Expr ptr, expr_builtin_t fcn) {
    assert(expr_get_type(ptr) == EXPR_TYPE_BUILTIN);
    expr_heap[ptr.index].builtin = fcn;
}

static void expr_set_type(Expr ptr, Expr_Type type) {
    expr_heap[ptr.index].type = type;
}

// GC
static void expr_mark(Expr expr) {
    // Already marked
    if (expr_get_mark(expr)) return;

    // Mark expression
    expr_set_mark(expr, 1);

    // Recurse
    if (expr_get_type(expr) == EXPR_TYPE_CONS) {
        expr_mark(expr_get_car(expr));
        expr_mark(expr_get_cdr(expr));
    }
}

static void expr_sweep(void) {
    free_count = 0;
    for (int i = 0; i < expr_count; ++i) {
        Expr ptr = {expr_count - i - 1};

        // Object is in use
        if (expr_get_mark(ptr)) {
            expr_set_mark(ptr, 0);
            continue;
        }

        // Object is free
        expr_free(ptr);
    }
}

static bool expr_is_nil(Expr expr) {
    return expr_get_type(expr) == EXPR_TYPE_NIL;
}

// Pop one element of the list
static Expr expr_pop(Expr *expr) {
    Expr car = expr_get_car(*expr);
    Expr cdr = expr_get_cdr(*expr);
    *expr = cdr;
    return car;
}

static void expr_push(Expr *list, Expr value) {
    *list = expr_cons(value, *list);
}

static void expr_append(Expr *first, Expr *last, Expr value) {
    Expr cons = expr_cons(value, expr_nil());

    if (expr_is_nil(*first)) {
        *first = *last = cons;
    } else {
        expr_set_cdr(*last, cons);
        *last = cons;
    }
}
