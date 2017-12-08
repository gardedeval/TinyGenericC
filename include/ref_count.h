#pragma once

#include "common.h"
#include "iden_concat.h"

#define REFERENCE_COUNTING_VER "0.0.1"

// TODO

#define ref_count_ptr(T) \
    struct { \
        T *ptr; \
        size_t count; \
        void (*fnFree)(T *); \
    }
#define rc_t ref_count_ptr

#define ref_count_get_count(rc) ( (rc)->count )
#define rc_get_count ref_count_get_count
#define rc_count ref_count_get_count

#define ref_count_get_free_fn(rc) ( (rc)->fnFree )
#define rc_get_free_fn ref_count_get_free_fn
#define rc_free_fn ref_count_get_free_fn

#define ref_count_get_ptr(rc) ( (rc)->ptr )
#define rc_get_ptr ref_count_get_ptr
#define rc_ptr ref_count_get_ptr

#define ref_count_valid(rc) \
    ( common_ensure((rc) != NULL), (rc) != NULL && rc_count(rc) > 0 && rc_free_fn(rc) != NULL && rc_ptr(rc) != NULL )
#define rc_valid ref_count_valid

#define ref_count_get(rc) ( common_ensure((rc) != NULL), rc_valid(rc) ? rc_get_ptr(rc) : NULL )
#define rc_get ref_count_get

#define ref_count_make(rc) \
    do { \
        common_ensure((rc) != NULL); \
        rc_count(rc) = 1; \
        *((void (**)(void *)) &rc_free_fn(rc)) = common_free; \
        rc_ptr(rc) = common_calloc(1, sizeof(*rc_get_ptr(rc))); \
        common_ensure_message(rc_ptr(rc) != NULL, "Out of memory"); \
    } while(0)
#define rc_make ref_count_make

#define ref_count_pin_memory(rc, mem, freeFn) \
    do { \
        void *ptr = (mem); \
        common_ensure((rc) != NULL); \
        common_ensure((ptr) != NULL); \
        common_ensure((freeFn) != NULL); \
        rc_count(rc) = 1; \
        rc_free_fn(rc) = (freeFn); \
        rc_ptr(rc) = (ptr); \
    } while(0)
#define rc_pin_mem ref_count_pin_memory

#define ref_count_destroy(rc) \
    ( common_ensure(rc_valid(rc)), rc_free_fn(rc)(rc_ptr(rc)), common_memset((rc), 0, sizeof(*(rc))) )
#define rc_destroy ref_count_destroy

#define ref_count_inflate(rc) ( common_ensure(rc_valid(rc)), ++rc_get_count(rc) )
#define rc_inflate ref_count_inflate

#define ref_count_increment ref_count_inflate
#define ref_inc ref_count_increment

#define ref_count_deflate(rc) \
    ( common_ensure(rc_valid(rc)), rc_get_count(rc) <= 1 ? (ref_count_destroy(rc), 0) : --rc_get_count(rc) )
#define rc_deflate ref_count_deflate

#define ref_count_decrement ref_count_inflate
#define rc_dec ref_count_decrement

#define ref_count_enter_gen iden_concat_2(__ref_count_enter_, __LINE__)

#define ref_count_enter(rc) \
    for (\
        int ref_count_enter_gen = rc_valid(rc); \
        ref_count_enter_gen != 0; \
        rc_deflate(rc), ref_count_enter_gen = 0 \
    )
#define rc_enter ref_count_enter
