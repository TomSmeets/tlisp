// Type definitions
#pragma once
#include <memory.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

typedef __UINT8_TYPE__ u8;
typedef __UINT32_TYPE__ u32;
typedef __UINT64_TYPE__ u64;
typedef __INT64_TYPE__ i64;
typedef __INTPTR_TYPE__ intptr_t;
typedef __SIZE_TYPE__ size_t;
typedef float f32;

#define array_count(x) (sizeof((x)) / sizeof((x)[0]))
