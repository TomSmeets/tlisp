#pragma once
#include "read.h"
#include "lisp_new.h"

// Consume any number of whitespace chars
static void read_whitespace(Read *read) {
    bool comment = false;
    for (;;) {
        if (read_eof(read)) break;

        u8 c = read_peek(read);
        if (c == ';') comment = true;
        if (c == '\n') comment = false;

        // Done
        if (!comment && c != ' ' && c != '\n') break;

        // Next
        read_next(read);
    }
}

static LispPtr read_lisp(Read *read) {
    read_whitespace(read);
    u8 c = read_peek(read);
    if (c == '-' || c == '+' || (c >= '0' && c <= '9')) {
        return lisp_new_number(read_i64(read));
    }

    if (c == '(') {
        read_next(read);
        LispPtr list_start = LISP_NIL;
        LispPtr list_end   = LISP_NIL;
        for(;;) {
            read_whitespace(read);

            if (read_peek(read) == ')') {
                read_next(read);
                return list_start;
            }

            if (read->error) return LISP_NIL;

            LispPtr item = read_lisp(read);
            lisp_append(&list_start, &list_end, item);
        }
    }

    if(c == '"') {
        read_next(read);
        u32 str_start = read->used;
        u32 str_end   = str_start;
        while (1) {
            str_end = read->used;
            if(read_eof(read)) break;
            if(read->error) return LISP_NIL;
            if (read_peek(read) == '\"') {
                read_next(read);
                break;
            }
            read_next(read);
        }
        return lisp_new_string_x((char *)(read->data + str_start), str_end - str_start);
    }

    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_') {
        u32 lab_start = read->used;
        read_next(read);

        u32 lab_end;
        while (1) {
            lab_end = read->used;
            if(read_eof(read)) break;
            if(read->error) return LISP_NIL;

            u8 c = read_peek(read);
            if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_' || (c >= '0' && c <= '9') || c == '-' || c == '?' || c == '!')) {
                break;
            }
            read_next(read);
        }
        return lisp_new_label_x((char *)(read->data + lab_start), lab_end - lab_start);
    }

    read_error(read, "Unexpected");
    return LISP_NIL;
}

static LispPtr lisp_parse(char *str) {
    Read read = {.size = strlen(str), .data = (u8 *)str};
    LispPtr p = read_lisp(&read);

    if(read.error) {
        printf("Parse error at ix=%i: %s\n", read.error_index, read.error);

        printf("%s\n", read.data);
        for (u32 i = 0; i < read.error_index; ++i) printf(" ");
        printf("^--- %s\n", read.error);
    }

    return p;
}
