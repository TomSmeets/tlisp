#pragma once
#include "type.h"

typedef struct {
    u32 size;
    u32 used;
    u8 *data;

    char *error;
    u32 error_index;
} Read;

static void read_error(Read *read, char *error) {
    read->error_index = read->used;
    read->error = error;
}

static bool read_eof(Read *read) {
    return read->used >= read->size;
}

static u8 read_next(Read *read) {
    if (read->used >= read->size) return 0;
    return read->data[read->used++];
}

static u8 read_peek(Read *read) {
    if (read->used >= read->size) return 0;
    return read->data[read->used];
}

static u64 read_u64(Read *read) {
    u64 value = 0;
    bool made_progress = false;
    for (;;) {
        if (read_eof(read)) break;
        u8 c = read_peek(read);
        bool is_digit = c >= '0' && c <= '9';
        if (!is_digit) break;
        value *= 10;
        value += c - '0';
        read_next(read);
        made_progress = true;
    }

    if (!made_progress) read_error(read, "Expected a digit");
    return value;
}

static i64 read_i64(Read *read) {
    u8 c = read_peek(read);

    if (c == '-' || c == '+') {
        read_next(read);
    }

    i64 out = read_u64(read);
    if (c == '-') out = -out;
    return out;
}
