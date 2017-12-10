#pragma once

#include "vector.h"

#define VECTOR_VIEW_VER "0.0.1"

#define vector_view vector
#define vec_view vector_view

#define vector_view_make(vv, mem, idx, cap) \
    do {\
        type_assert_by_size(vec_type_sizeof(vv), array_type_sizeof(buf)); \
        common_memset((vv), 0, vec_sizeof(vv)); \
        vec_mem(vv) = (mem);\
        vec_index(vv) = (idx);\
        vec_cap(vv) = (cap);\
    } while(0)

#define vec_view_make vector_view_make

#define vector_view_make_array(vv, buf, idx) \
    do {\
        type_assert_by_size(vec_type_sizeof(vv), array_type_sizeof(buf)); \
        common_memset((vv), 0, vec_sizeof(vv)); \
        vec_mem(vv) = (buf);\
        vec_index(vv) = (idx);\
        vec_cap(vv) = sizeof((buf));\
    } while(0)

#define vec_view_make_arr vector_view_make_array

#define vector_view_get vector_get
#define vec_view_get vector_view_get

#define vector_view_get_ref vector_get_ref
#define vec_view_get_ref vector_view_get_ref

#define vector_view_at vector_at
#define vec_view_at vector_view_at

#define vector_view_empty vector_empty 
#define vec_view_empty vector_view_empty

#define vector_view_push(vv, val) \
    do { \
        common_ensure(vec_valid(vv)); \
        common_ensure(vec_index(vv) < vec_cap(vv)); \
        vec_mem(vv)[vec_index(vv)++] = (val); \
    } while(0)
#define vec_view_push vector_view_push

#define vector_view_pop vector_pop
#define vec_view_pop vector_view_pop

#define vector_view_top vector_top
#define vec_view_top vector_view_top

#define vector_view_bottom vector_bottom
#define vec_view_bottom vector_view_bottom

#define vector_view_enqueue(vv, val) \
    do {\
        common_ensure(vec_valid(vv)); \
        vec_assert_boundary(vec_indexv(v) + 1); \
        common_memmove(vec_get_ref(vv, 1), vec_get_ref(vv, 0), vec_type_sizeof(vv) * vec_index(vv)); \
        vec_index(vv)++; \
        vector_get(vv, 0) = (val); \
    } while(0)
#define vec_view_enqueue vector_view_enqueue

#define vector_view_dequeue vector_dequeue
#define vec_view_dequeue vector_view_dequeue

#define vector_view_for_each vector_for_each
#define vec_view_for_each vector_view_for_each

#define vector_view_for_each_index_only vector_for_each_index_only
#define vec_view_for_each_index_only vector_view_for_each_index_only

#define vector_view_qsort vector_qsort
#define vec_view_qsort vector_view_qsort

#define vector_view_slice vector_slice
#define vec_view_slice vector_view_slice
