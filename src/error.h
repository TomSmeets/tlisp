#pragma once

typedef const char *Error;

#define TO_STRING0(x) #x
#define TO_STRING(x) TO_STRING0(x)
#define check(E, cond) error_set((E), (cond), __FILE__ ":" TO_STRING(__LINE__) ": " #cond)

static void error_set(Error *error, bool cond, const char *message) {
    if (cond && !*error) *error = message;
}
