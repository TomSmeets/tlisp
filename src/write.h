#pragma once
#include "assert.h"
#include "type.h"

typedef struct {
    u32 size;
    u32 used;
    u8 *data;
} Write;

static void write_u8(Write *w, u8 c) {
    if (w->used >= w->size) return;
    w->data[w->used++] = c;
}

static void write_c(Write *w, char c) {
    write_u8(w, c);
}

static void write_s(Write *w, char *s) {
    while (*s) write_c(w, *s++);
}

static void write_u64(Write *w, u64 n) {
    u32 digit_count = 0;
    u8 digit_list[20];
    do {
        digit_list[digit_count++] = (n % 10) + '0';
        n /= 10;
    } while (n);

    for (u32 i = 0; i < digit_count; ++i) {
        write_c(w, digit_list[digit_count - i - 1]);
    }
}

static void write_i64(Write *w, i64 n) {
    if (n < 0) {
        write_c(w, '-');
        write_u64(w, -n);
    } else {
        write_u64(w, n);
    }
}
