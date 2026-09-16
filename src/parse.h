#pragma once
#include "ast.h"
#include "str.h"

typedef struct {
    // Current parser position
    char *cursor;
} Parse;

// Return next char
static char parse_peek(Parse *p) {
    return p->cursor[0];
}

// Advance to next char
static void parse_next(Parse *p) {
    p->cursor++;
}

// Consume any number of whitespace chars
static void parse_whitespace(Parse *p) {
    bool comment = false;
    for (;;) {
        char c = parse_peek(p);
        if (c == ';') comment = true;
        if (c == '\n') comment = false;
        if (!comment && c != ' ' && c != '\n') break;
        parse_next(p);
    }
}

// Parse a literal number
static long parse_long(Parse *p) {
    bool negative = false;
    long num = 0;

    if (parse_peek(p) == '-') {
        negative = true;
        parse_next(p);
    } else if (parse_peek(p) == '+') {
        parse_next(p);
    }

    for (;;) {
        char c = parse_peek(p);
        if (c == 0) break;
        if (!(c >= '0' && c <= '9')) break;
        num *= 10;
        num += c - '0';
        parse_next(p);
    }

    if (negative) num = -num;
    return num;
}

static u32 parse_list(Parse *p);

static u32 parse_value(Parse *p) {
    // Skip any whitespace
    parse_whitespace(p);
    char c = parse_peek(p);

    if (c == '(') {
        parse_next(p);
        return parse_list(p);
    }

    if (c == '\0') {
        return 0;
    }

    if ((c >= '0' && c <= '9') || (c == '-' || c == '+')) {
        return expr_value(parse_long(p));
    }

    char *expr_start = p->cursor;
    for (;;) {
        c = parse_peek(p);
        if (c == '\0') break;
        if (c == ' ') break;
        if (c == '\n') break;
        if (c == ')') break;
        if (c == '(') break;
        parse_next(p);
    }
    char *expr_end = p->cursor;
    return expr_bytes(expr_end - expr_start, (u8 *)expr_start);
}

// Returns a list
static u32 parse_list(Parse *p) {
    parse_whitespace(p);
    char c = parse_peek(p);

    // Nil
    if (c == ')') {
        parse_next(p);
        return 0;
    }

    if (c == '.') {
        parse_next(p);
        return parse_value(p);
    }

    if (c == '\0') {
        return 0;
    }

    u32 car = parse_value(p);
    u32 cdr = parse_list(p);
    u32 ret = expr_cons(car, cdr);
    return ret;
}

static u32 parse(char *input) {
    Parse p = {.cursor = input};
    return expr_cons(expr_str("do"), parse_list(&p));
}
