#pragma once

#include "common.h"
#error "TODO"

#define BINARY_TREE_VER "0.0.1"

#define binary_tree_gen3(T, count) T##count
#define binary_tree_gen2(T, count) binary_tree_gen3(T, count)
#define binary_tree_gen binary_tree_gen2(__binary_tree, __LINE__)

#define binary_tree(T) \
    struct binary_tree_gen { \
        struct binary_tree_gen *parent; \
        struct binary_tree_gen *left, *right; \
        T value; \
    }

#define binary_tree_nodef(T) \
    struct { \
        struct binary_tree_gen *parent; \
        struct binary_tree_gen *left, *right; \
        T value; \
    }

typedef struct binary_tree_opaque {
    binary_tree_opaque *parent;
    binary_tree_opaque *left, *right;
    char value[1];
} binary_tree_opaque;

#define binary_tree_sizeof(t) ( sizeof(*(t)) )
#define binary_tree_type_sizeof(t) ( sizeof((t)->value) )

#define binary_tree_parent(t) ( (t)->parent )
#define binary_tree_left(t) ( (t)->left )
#define binary_tree_right(t) ( (t)->right )
#define binary_tree_value(t) ( (t)->value )

#define binary_tree_clear(t) ( common_memset( (t), 0, binary_tree_sizeof(t) ) )
#define binary_tree_make(t) *( binary_tree_clear(t), &(t)->value )