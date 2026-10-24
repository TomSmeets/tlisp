#pragma once
#include "lisp_type.h"
#include "lisp_new.h"
#include "assert.h"

typedef struct {
    // list of items to eval()
    // (1 2 3 4) -> (1 2 3 4)
    LispPtr input; // Right hand side, unevaluated
    LispPtr arg_first;
    LispPtr arg_last; // left hand side, evaluated

    // When entering a function call, store the frame
    LispPtr saved;

    // Environment ((k v) ...)
    LispPtr env;
} LispVm;

static LispPtr lisp_builtin_add(LispVm *env, LispPtr args) {
    i64 out = 0;
    return LISP_NIL;
}

static void lisp_step(LispVm *vm) {
    if(lisp_type(vm->input) == LISP_TYPE_NIL) {
        // Program done
        if(lisp_type(vm->saved) == LISP_TYPE_NIL) return;

        LispPtr fun  = lisp_pop(&vm->arg_first);
        LispPtr args = vm->arg_first;

        // Must be a builtin
        assert(lisp_type(fun) == LISP_TYPE_BUILTIN);

        // Call
        LispPtr ret = ((LispPtr (*)(LispPtr *env, LispPtr args))lisp_value(fun)->builtin)(&vm->env, args);

        // End function call
        vm->arg_last = lisp_pop(&vm->saved);
        vm->arg_first = lisp_pop(&vm->saved);
        vm->input = lisp_pop(&vm->saved);
        lisp_append(&vm->arg_first, &vm->arg_last, ret);
        return;
    }

    LispPtr ptr = lisp_pop(&vm->input);
    LispPtr ret = LISP_NIL;
    LispType type= lisp_type(ptr);

    if(type == LISP_TYPE_LABEL) {
        // Lookup label
        LispPtr ret = LISP_NIL;
        lisp_append(&vm->arg_first, &vm->arg_last, ptr);
        return;
    }

    if(type == LISP_TYPE_CONS) {
        // Begin function call, store current context and start evaluating the function
        lisp_push(&vm->saved, vm->input);
        lisp_push(&vm->saved, vm->arg_first);
        lisp_push(&vm->saved, vm->arg_last);
        vm->input = ptr;
        vm->arg_first = LISP_NIL;
        vm->arg_last = LISP_NIL;
        return;
    }

    // Rest evaluates to itself
    lisp_append(&vm->arg_first, &vm->arg_last, ptr);
}
