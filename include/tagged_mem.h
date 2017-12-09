#pragma once

#include "common.h"

#define TAGGED_MEMORY_VER "0.0.1"

typedef struct {
    size_t len;
    char mem[1];
} tagged_mem_t, tm_t;

INLINE
tagged_mem_t *__tagged_mem_make(const void *mem, const size_t len) {
    if (mem == NULL || len < 1) {
        return NULL;
    }

    tagged_mem_t *clone = (tagged_mem_t *)common_malloc(sizeof(size_t) + len);
    if (clone != NULL) {
        clone->len = len;
        common_memcpy(clone->mem, mem, len);
    }
    return clone;
}

INLINE
tagged_mem_t *__tagged_mem_resize(const tagged_mem_t *mem, const size_t newLen) {
    if (mem == NULL || newLen < 1) {
        return NULL;
    }

    tagged_mem_t *newMem = common_realloc((void *)mem, sizeof(size_t) + newLen);
    if (newMem != NULL) {
        newMem->len = newLen;
    }
    return newMem;
}

#define tagged_mem_make __tagged_mem_make
#define tagged_mem_clone(mem) tagged_mem_make((mem)->mem, (mem)->len)
#define tagged_mem_destroy(mem) ( common_free((void *) mem) )
#define tagged_mem_resize __tagged_mem_resize

#define tagged_mem_offset(p, n) ( ( (char*) &(p)->mem ) + (n) )

#define tagged_mem_to_ptr(p, T) ( (T *) tagged_mem_offset(p, 0) )
#define tagged_mem_to_ptr_safe(p, T) ( common_ensure( (p)->len >= sizeof(T) ), tagged_mem_to_ptr(p, T) )
#define tagged_mem_to_ptr_strict(p, T) ( common_ensure( (p)->len == sizeof(T) ), tagged_mem_to_ptr(p, T) )

#define tagged_mem_to_arr(p, T, n) ( (T *) tagged_mem_offset(p, sizeof(T) * n) )
#define tagged_mem_to_arr_safe(p, T, n) ( common_ensure( (p)->len >= (sizeof(T) * n) ), tagged_mem_to_arr(p, T, n) )
