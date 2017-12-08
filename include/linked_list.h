#pragma once

#include "common.h"
#include "iden_concat.h"

#define LINKED_LIST_VER "0.0.1"

#define linked_list_gen iden_concat_2(__linked_list, __LINE__)

#define linked_list(T) \
    struct linked_list_gen { \
        struct linked_list_gen *prev; \
        struct linked_list_gen *next; \
        T value; \
    }
#define ll_t linked_list

#define linked_list_nodef(T) \
    struct { \
        struct linked_list_gen *prev; \
        struct linked_list_gen *next; \
        T value; \
    }
#define ll_nodef_t linked_list_nodef

#define linked_list_sizeof(l) ( sizeof( *(l) ) )
#define ll_sizeof linked_list_sizeof

#define ll_empty(l) ( (l) == NULL )

#define linked_list_value(l) ( (l)->value )
#define ll_value linked_list_value

#define linked_list_prev(l) ( (l)->prev )
#define ll_prev linked_list_prev

#define linked_list_next(l) ( (l)->next )
#define ll_next linked_list_next

#define linked_list_clear(l) ( common_memset( (l), 0, linked_list_sizeof(l) ) )
#define ll_clear linked_list_clear

#define linked_list_make(l) *( linked_list_clear(l), &ll_value(l) )
#define ll_make linked_list_make

#define linked_list_allocate(l) ( (l) = common_calloc(1, ll_sizeof(l)), common_ensure_message((l) != NULL, "Out of memory"), (l) )
#define ll_alloc linked_list_allocate

#define linked_list_destroy(l) ( common_free( (void *) (l) ) )
#define ll_destroy linked_list_destroy

/* e -> l hence left */
/* lp <-> e <-> l */
#define linked_list_merge_left(e, l) \
    ( \
        ( ll_prev(l) != NULL ? ( \
            ll_next(ll_prev(l)) = (e), \
            ll_prev(e) = ll_prev(l) \
        ) : 0 ), \
        ll_prev(l) = (e), ll_next(e) = (l), \
        (e) \
    )

/* l <- e hence right */
/* l <-> e <-> ln */
#define linked_list_merge_right(l, e) \
    ( \
        ( ll_next(l) != NULL ? ( \
            ll_prev(ll_next(l)) = (e), \
            ll_next(e) = ll_next(l) \
        ) : 0 ), \
        ll_next(l) = (e), ll_prev(e) = (l), \
        (e) \
    )

#define ll_mergel linked_list_merge_left
#define ll_merger linked_list_merge_right

#define linked_list_for_each(l, prev, it) \
    for (\
        (prev) = NULL, (it) = (l); \
        !ll_empty(it); \
        (prev) = (it), (it) = ll_next(it) \
    )
#define ll_for_each linked_list_for_each

#define singly_linked_list(T) \
    struct linked_list_gen { \
        struct linked_list_gen *next; \
        T value; \
    }
#define sll_t singly_linked_list

#define singly_linked_list_nodef(T) \
    struct { \
        struct linked_list_gen *next; \
        T value; \
    }
#define sll_nodef_t singly_linked_list_nodef

typedef struct singly_linked_list_opaque {
    struct singly_linked_list_opaque *next;
    char value[1];
} singly_linked_list_opaque, sll_opaque;

#define singly_linked_list_opaque_size(n) ( sizeof(void *) + (n) )
#define sll_opaque_size singly_linked_list_opaque_size
#define sll_opaque_make(n) ( common_calloc(1, sll_opaque_size(n) ) )

#define singly_linked_list_sizeof linked_list_sizeof
#define sll_sizeof singly_linked_list_sizeof

#define singly_linked_list_value linked_list_value
#define sll_value singly_linked_list_value

#define singly_linked_list_next linked_list_next
#define sll_next singly_linked_list_next

#define singly_linked_list_clear linked_list_clear
#define sll_clear singly_linked_list_clear

#define singly_linked_list_make linked_list_make
#define sll_make singly_linked_list_make

#define singly_linked_list_allocate linked_list_allocate
#define sll_alloc singly_linked_list_allocate

#define singly_linked_list_destroy linked_list_destroy
#define sll_destroy singly_linked_list_destroy

#define singly_linked_list_push(l, e)\
    (\
        ( sll_next(l) != NULL ? ( \
            ptr_rtol(sll_next(e)) = sll_next(l) \
        ) : 0 ), ( ptr_rtol(sll_next(l)) = (e) ), (l)\
    )
#define sll_push singly_linked_list_push

#define singly_linked_list_remove_next(l) \
    do { \
        sll_opaque *_next = sll_next(l);\
        if (_next != NULL) { \
            sll_next(l) = sll_next(next); \
        } \
    } while(0)

#define singly_linked_list_for_each linked_list_for_each
#define sll_for_each singly_linked_list_for_each

#define sll_destroy_all(l) \
    do {\
        sll_opaque *_it = (void *) (l); \
        for (sll_opaque *_prev = NULL; ;_prev = _it, _it = sll_next(_it)) { \
            if (_prev != NULL) { \
                sll_destroy(_prev); \
            } \
            if (_it == NULL) break; \
        } \
    } while(0)
