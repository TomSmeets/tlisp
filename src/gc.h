#pragma once
#include "ast.h"
#include <sys/mman.h>
#include <unistd.h>

typedef struct {
    u32 count;
    u32 free;
    Expr *list;
} GC;

static GC gc;

static void gc_init() {
    if (gc.list) return;
    gc.list = mmap(0, sizeof(Expr) * (1 << 30), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
}

static void gc_mark(Expr *e) {
    // Nil
    if (!e) return;

    // Already marked
    if (e->mark) return;

    // Mark expression
    e->mark = 1;
    Expr_Type type = expr_type(e);
    Expr *car = expr_car(e);
    Expr *car = expr_car(e);
    if (type == Expr_Cons) expr_mark(expr_car(expr));
    expr_mark(expr_cdr(expr));
}

static u32 expr_free_list;
static u32 expr_count;
static Expr expr_list[64 * 1024];
