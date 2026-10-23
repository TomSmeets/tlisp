#pragma once
#include "assert.h"
#include "lisp_heap.h"
#include "lisp_type.h"
#include "type.h"
#include "write.h"

static void lisp_pretty(LispPtr ptr, Write *w) {
    LispType type = lisp_type(ptr);

    if(type == LISP_TYPE_NIL) {
        write_s(w, "()");
        return;
    }

    if(type == LISP_TYPE_CONS) {
        write_s(w, "(");
        for (;;) {
            LispValue *val = lisp_value(ptr);
            LispPtr car = val->cons.car;
            LispPtr cdr = val->cons.cdr;
            lisp_pretty(car, w);

            // Normal ending to a list
            if (lisp_type(cdr) == LISP_TYPE_NIL) break;

            // Not a normal list
            if (lisp_type(cdr) != LISP_TYPE_CONS) {
                write_s(w, " . ");
                lisp_pretty(cdr, w);
                break;
            }

            // Keep going
            write_s(w, " ");
            ptr = cdr;
        }
        write_s(w, ")");
        return;
    }

 if(type == LISP_TYPE_STRING) {
        write_s(w, "\"");
        for (;;) {
            LispValue *val = lisp_value(ptr);
            u32 data = val->string.data;
            LispPtr cdr = val->string.cdr;
            if(data == '\\') write_c(w, '\\');
            if(data == '\"') write_c(w, '\\');
            write_c(w, data);

            // normal end to string
            if (lisp_type(cdr) == LISP_TYPE_NIL) break;

            // Abnormal end -> throw error
            assert(lisp_type(cdr) == LISP_TYPE_STRING);

            // Keep going
            ptr = cdr;
        }
        write_s(w, "\"");
        return;
    }

 if(type == LISP_TYPE_LABEL) {
        for (;;) {
            LispValue *val = lisp_value(ptr);
            u32 data = val->label.data;
            LispPtr cdr = val->label.cdr;
            write_c(w, data);

            // normal end to string
            if (lisp_type(cdr) == LISP_TYPE_NIL) break;

            // Abnormal end -> throw error
            assert(lisp_type(cdr) == LISP_TYPE_LABEL);

            // Keep going
            ptr = cdr;
        }
        return;
    }

     if(type == LISP_TYPE_BUILTIN) {
        write_s(w, "#BUILTIN");
        return;
    }

    if (type == LISP_TYPE_NUMBER) {
        i64 num = lisp_value(ptr)->number;
        write_i64(w, num);
        return;
    }

    assert(false);
}

static void lisp_pretty_print(char *prefix, LispPtr ptr, char *suffix) {
    u8 buffer[16*1024];
    Write write = { .size = sizeof(buffer), .data = buffer };
    write_s(&write, prefix);
    lisp_pretty(ptr, &write);
    write_s(&write, suffix);
    fwrite(buffer, 1, write.used, stdout);
}
