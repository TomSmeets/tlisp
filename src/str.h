#pragma once
#include "mem.h"

// Duplicate and trim the input string
static void *str_dup(size_t len, char *s) {
    if (!s) return 0;
    char *buf = mem_alloc(len + 1);
    for (size_t i = 0; i < len; ++i) buf[i] = s[i];
    buf[len] = 0;
    return buf;
}

// Check if two strings are equal
static bool str_eq(char *a, char *b) {
    if (!a && !b) return true;
    if (!a) return false;
    if (!b) return false;
    for (;;) {
        if (*a != *b) return false;
        if (*a == 0) return true;
        a++;
        b++;
    }
}
