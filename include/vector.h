#pragma once

#include "common.h"
#include "static_assert.h"

#define VECTOR_VER "0.0.1"

#define vector(T) \
    struct { \
        T *mem; \
        size_t tmp; \
        size_t idx; \
        size_t cap; \
        T memTmp; \
    }
#define vec vector

#define vector_tmp(v, T) ( *( (T*) &(v)->tmp ) )
#define vec_tmp vector_tmp

#define vector_index(v) ( (v)->idx )
#define vec_index vector_index

#define vector_capacity(v) ( (v)->cap )
#define vec_cap vector_capacity

#define vector_memory(v) ( (v)->mem )
#define vec_mem vector_memory

#define vector_valid(v) ( (v) != NULL && vec_mem(v) != NULL )
#define vec_valid vector_valid

#define vector_sizeof(v) ( sizeof( *(v) ) )
#define vec_sizeof vector_sizeof

#define vector_type_sizeof(v) ( sizeof( *vec_mem(v) ) )
#define vec_type_sizeof vector_type_sizeof

#define vector_used_memory(v) ( vec_type_sizeof(v) * vec_cap(v) )
#define vec_used_mem vector_used_memory

#define vector_empty(v) ( vec_index(v) < 1 )
#define vec_empty vector_empty

#define vector_assert_underflow(v) ( common_ensure((v) != NULL), common_ensure( vec_cap(v) >= 0 ) )
#define vec_assert_underflow vector_assert_underflow

#define vector_assert_boundary(v, n) ( common_ensure((v) != NULL), common_ensure( ( (n) >= 0 ) && ( vec_index(v) >= (n) ) ) )
#define vec_assert_boundary vector_assert_boundary

#define vector_clear_memory(v) \
    do { \
        common_ensure (vec_valid(v)); \
        common_memset(vec_mem(v), 0, vec_used_mem(v)); \
    } while (0)
#define vec_clear_mem vector_clear_memory

#define vector_make(v) \
    do { \
        common_ensure((v) != NULL); \
        common_memset((v), 0, vec_sizeof(v)); \
        vec_cap(v) = 1; \
        vec_mem(v) = common_malloc( vec_used_mem(v) ); \
        common_ensure_message(vec_mem(v) != NULL, "Out of memory"); \
        vec_clear_mem(v); \
    } while(0)
#define vec_make vector_make

#define vector_destroy(v) \
    do { \
        common_ensure((v) != NULL); \
        if (vec_mem(v) != NULL) { \
            common_free( (void *) vec_mem(v) ); \
        } \
        common_memset((v), 0, vec_sizeof(v)); \
    } while(0) 
#define vec_destroy vector_destroy

/* simple full struct shallow clone */
#define vector_clone(dest, src) \
    do { \
        STATIC_ASSERT(vec_type_sizeof(dest) == vec_type_sizeof(src), "underlying size unmatch"); \
        common_ensure_message(vec_type_sizeof(dest) == vec_type_sizeof(src), "underlying size unmatch"); \
        common_ensure((dest) != NULL && vec_valid(src)); \
        common_memcpy(dest, src, vec_sizeof(src)); \
        vec_mem(dest) = common_malloc( vec_used_mem(src) ); \
        common_ensure_message(vec_mem(dest) != NULL, "Out of memory"); \
        common_memcpy(vec_mem(dest), vec_mem(src), vec_used_mem(src)); \
    } while(0)
#define vec_clone vector_clone

#define vector_resize_safe(v, n) \
    do { \
        size_t _n = (n); \
        size_t old_cap = vec_cap(v);\
        common_ensure(vec_valid(v)); \
        vec_mem(v) = common_realloc(vec_mem(v), vec_type_sizeof(v) * _n); \
        common_ensure_message(vec_mem(v) != NULL, "Out of memory"); \
        common_memset(vec_mem(v) + old_cap, 0, vec_type_sizeof(v) * (_n - old_cap));\
        vec_cap(v) = _n; \
    } while(0)
#define vec_resize_safe vector_resize_safe

#define vector_resize_unsafe(v, n) \
    ( \
        common_ensure((v) != NULL), \
        vec_mem(v) = common_realloc(vec_mem(v), vec_type_sizeof(v) * (n)), \
        common_ensure_message(vec_mem(v) != NULL, "Out of memory"), \
        vec_tmp(v, size_t) =  vec_type_sizeof(v) * ((n) - vec_cap(v)), \
        common_memset(vec_mem(v) + vec_cap(v), 0, vec_tmp(v, size_t)), \
        vec_cap(v) = (n) \
    )
#define vec_resize_unsafe vector_resize_unsafe

#define vector_needs_expanding(v) (vec_index(v) >= vec_cap(v))
#define vec_needs_expanding vector_needs_expanding

#define vector_expand(v) vec_resize_safe(v, vec_cap(v) * 2)
#define vec_expand vector_expand

#define vector_check_for_expand(v) \
    do { \
        common_ensure(vec_valid(v)); \
        if (vec_needs_expanding(v)) { \
            vec_expand(v); \
        } \
    } while (0)
#define vec_check_for_expand vector_check_for_expand

#define vector_get(v, n) *( common_ensure((v) != NULL), &vec_mem(v)[(n)] )
#define vec_get vector_get

#define vector_get_ref(v, n) ( &vector_get(v, n) )
#define vec_get_ref vector_get_ref

#define vector_at(v, n) *( common_ensure(vec_valid(v)), vec_assert_boundary(v, (size_t) (n)), &vec_get(v, n) )
#define vec_at vector_at

#define vector_at_with_expand(v, n) \
    *( \
        common_ensure( vec_valid(v) ), \
        common_ensure( (n) >= 0 ), \
        (n) >= vec_index(v) ? \
        ( \
            vec_resize_unsafe(v, size_expand_to_2n((n) + 1)), \
            vec_index(v) = (n) + 1 \
        ) : 0, \
        &vec_mem(v)[(n)] \
    )
#define vec_at_expand vector_at_with_expand

#define vector_top(v) ( common_ensure(vec_valid(v)), vec_get(v, vec_index(v) > 0 ? (vec_index(v) - 1) : 0) )
#define vec_top vector_top

#define vector_bottom(v) ( common_ensure(vec_valid(v)), vec_get(v, 0) )
#define vec_bottom vector_bottom

#define vector_push(v, val) \
    do { \
        common_ensure(vec_valid(v)); \
        vec_check_for_expand(v); \
        vec_mem(v)[vec_index(v)++] = (val); \
    } while(0)
#define vec_push vector_push

#define vector_pop(v) ( common_ensure(vec_valid(v)), vec_assert_underflow(v), vec_get(v, --vec_index(v)) )
#define vec_pop vector_pop

#define vector_enqueue(v, val) \
    do {\
        common_ensure(vec_valid(v)); \
        vec_check_for_expand(v); \
        common_memmove(vec_get_ref(v, 1), vec_get_ref(v, 0), vec_type_sizeof(v) * vec_index(v)); \
        vec_index(v)++; \
        vector_get(v, 0) = (val); \
    } while(0)
#define vec_enqueue vector_enqueue

#define vector_dequeue(v) \
    ( \
      common_ensure(vec_valid(v)), vec_assert_underflow(v), \
      (v)->memTmp = vec_get(v, 0), \
      common_memmove(vec_get_ref(v, 0), vec_get_ref(v, 1), vec_type_sizeof(v) * vec_index(v)), \
      (v)->memTmp \
    )
#define vec_dequeue vector_dequeue

#define vector_for_each(v, it, value) if (vec_valid(v)) for ((it) = 0; (value) = vec_get_ref(v, it), (it) < vec_index(v); ++(it))
#define vec_for_each vector_for_each

#define vector_for_each_index_only(v, it) if (vec_valid(v)) for ((it) = 0; (it) < vec_index(v); ++(it))
#define vec_for_each_index_only vector_for_each_index_only

#define vector_qsort(v, comp) ( common_qsort(vec_mem(v), vec_index(v), vec_type_sizeof(v), (comp)) )
#define vec_qsort vector_qsort

#define vector_slice(v, dest, start, end) \
    do { \
        STATIC_ASSERT(vec_type_sizeof(dest) == vec_type_sizeof(src), "underlying size unmatch"); \
        common_ensure_message(vec_type_sizeof(dest) == vec_type_sizeof(src), "underlying size unmatch"); \
        common_ensure(vec_valid(v) && (dest) != NULL); \
        common_ensure ((start) > 0 && (end) > 0); \
        common_ensure ((start) < (end)); \
        common_ensure ((end) < vec_index(v) && (start) < vec_index(v)); \
        size_t n = size_expand_to_2n((end) - (start)); /* 2^n padded! */ \
        size_t cb = ((end) - (start)) * vec_type_sizeof(v); \
        common_memset((v), 0, vec_sizeof(v)); \
        vec_index(dest) = 0; \
        vec_cap(dest) = n; \
        vec_mem(dest) = common_malloc( n * vec_type_sizeof(v) ); \
        common_ensure_message(vec_mem(dest) != NULL, "Out of memory"); \
        common_memcpy((dest), vec_get(v, start), cb); \
    } while(0)
