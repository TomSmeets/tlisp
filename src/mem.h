// Simple heap allocator
#pragma once
#include "type.h"

// A very simple heap
static u8 heap_data[1024 * 1024];
static size_t heap_used;

// Allocation method
static void *mem_alloc_x(size_t size, size_t align, bool zero) {
    heap_used = (heap_used + (align - 1)) & ~(align - 1);
    void *ptr = heap_data + heap_used;
    heap_used += size;
    if (zero) memset(ptr, 0, size);
    return ptr;
}

// Allocate unaligned memory
static void *mem_alloc(size_t size) {
    return mem_alloc_x(size, 1, false);
}

// Allocate a struct with correct alignment and init to zero
#define mem_struct(T) (T *)mem_alloc_x(sizeof(T), alignof(T), true)
