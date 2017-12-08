#pragma once

#include "common.h"

#define TAGGED_MEMORY_VER "0.0.1"

typedef struct {
    size_t len;
    char mem[1];
} tagged_mem_t, tm_t;

tagged_mem_t *__tagged_mem_make(const void *mem, const size_t len);
tagged_mem_t *__tagged_mem_resize(const tagged_mem_t *mem, const size_t newLen);

#define tagged_mem_make __tagged_mem_make
#define tagged_mem_clone(mem) tagged_mem_make((mem)->mem, (mem)->len)
#define tagged_mem_destroy(mem) ( common_free((void *) mem) )
#define tagged_mem_resize __tagged_mem_resize

#define tagged_mem_offset(p, n) ( ( (char*) &(p)->mem ) + (n) )

#define tagged_mem_to_ptr(p, T) ( (T *) tagged_mem_offset(p, 0) )
#define tagged_mem_to_ptr_safe(p, T) ( assert( (p)->len >= sizeof(T) ), tagged_mem_to_ptr(p, T) )
#define tagged_mem_to_ptr_strict(p, T) ( assert( (p)->len == sizeof(T) ), tagged_mem_to_ptr(p, T) )

#define tagged_mem_to_arr(p, T, n) ( (T *) tagged_mem_offset(p, sizeof(T) * n) )
#define tagged_mem_to_arr_safe(p, T, n) ( assert( (p)->len >= (sizeof(T) * n) ), tagged_mem_to_arr(p, T, n) )
