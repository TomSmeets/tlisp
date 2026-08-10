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

// Return next char
static char parse_peek2(Parse *p) {
    if(!parse_peek(p)) return 0;
    return p->cursor[1];
}

// Advance to next char
static void parse_next(Parse *p) {
    p->cursor++;
}

// Consume any number of whitespace chars
static void parse_whitespace(Parse *p) {
    for (;;) {
        char c = parse_peek(p);
        if (c != ' ' && c != '\n') break;
        parse_next(p);
    }
}

// Parse a literal number
static long parse_long(Parse *p) {
    bool negative = false;
    long num = 0;

    if(parse_peek(p) == '-') {
        negative = true;
        parse_next(p);
    } else if(parse_peek(p) == '+') {
        parse_next(p);
    }

    for(;;) {
        char c = parse_peek(p);
        if(c == 0) break;
        if (!(c >= '0' && c <= '9')) break;

        num *= 10;
        num += c - '0';
        parse_next(p);
    }

    if(negative) num = -num;
    return num;
}


static Expr *parse_list(Parse *p);

static Expr *parse_value(Parse *p) {
    // Skip any whitespace
    parse_whitespace(p);

    char c = parse_peek(p);
    char c2 = parse_peek2(p);

    if (c == '(') {
        parse_next(p);
        return parse_list(p);
    }

    if (c == '\0') {
        return 0;
    }

    if ((c >= '0' && c <= '9') || ((c == '-' || c == '+') && (c2 >= '0' && c2 <= '9'))) {
        return expr_num(parse_long(p));
    }

    char *expr_start = p->cursor;
    for (;;) {
        c = parse_peek(p);
        if (c == '\0') break;
        if (c == ' ') break;
        if (c == ')') break;
        if (c == '(') break;
        parse_next(p);
    }
    char *expr_end = p->cursor;
    char *label = str_dup(expr_end - expr_start, expr_start);
    return expr_label(label);
}

// Returns a list
static Expr *parse_list(Parse *p) {
    Expr *list = 0;
    Expr *last = 0;

    parse_whitespace(p);
    char c = parse_peek(p);

    // Nil
    if (c == ')') {
        parse_next(p);
        return 0;
    }

    if (c == '\0') {
        return 0;
    }
    Expr *car = parse_value(p);
    Expr *cdr = parse_list(p);
    Expr *ret = expr_cons(car, cdr);
    return ret;
}

