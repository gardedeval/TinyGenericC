#pragma once

#include "common.h"
#include "vector.h"
#include "linked_list.h"
#include "static_assert.h"
#include "iden_concat.h"

/* generic (weakly generic variant) hash table implementation
* relies on padding hacks, aliasing,
* expression hacks, temporary fields
* and memory layout exploiting,
* granting you multiple cancers at once!
*
* this will not work on big-endian systems.
*
* uses separate chaining with list head cell.
* complexities:
* space: O(n)
* search: O(n/m) [or O(1) for amortized constant time]
*   where m is the total amount of bucket
* insert: same as search
* delete: same as search
*
* !!! LACKS SECURITY SUPPORT !!!
* !!! NOT RESISTANT TO DOS ATTACK !!
* !!! YOU SHOULD NOT THIS IN PRODUCTION !!!
*
* usage: see test
*/

#define HASH_TABLE_VER "0.0.4"

// your choice, i like 8 in flavor of sufficiently-spreaded buckets
// and less linked list traversal
// if you set this to 1, it behaves like linear probing,
// super cool eh?
#define HASH_SEPARATE_CHAIN_MAXLINKS 8  

// integral type for hash table hash value
typedef uint32_t hash_table_hash_t, ht_hash_t;

// currently using fnv1a-32 and djb2 for double hashing (not figured out yet sorry)
#define fnv1a_prime 16777619
#define fnv1a_offset_basis 2166136261
INLINE
ht_hash_t __fnv1a(const char *mem, const size_t len) {
    if (mem == NULL) {
        return 0;
    }

    uint32_t hash = fnv1a_offset_basis;
    for (size_t i = 0; i < len; i++) {
        hash ^= mem[i];
        hash *= fnv1a_prime;
    }
    return (ht_hash_t)hash;
}

INLINE
ht_hash_t __djb2(const char *mem, const size_t len) {
    if (mem == NULL) {
        return 0;
    }

    uint32_t hash = 5381;
    for (size_t i = 0; i < len; i++) {
        hash = hash * 33 + mem[i];
    }
    return (ht_hash_t)hash;
}
#define hash_table_hash(mem, len) ( __fnv1a((char*) (mem), (len)) )
#define ht_hash hash_table_hash

#define hash_table_hash_backup(mem, len) ( __djb2((char*) (mem), (len)) )
#define ht_hash_backup hash_table_hash_backup

#define hash_table_locate_index(bucket, hash) ( (hash) % vec_cap(bucket) )
#define ht_locate_index hash_table_locate_index

#define hash_table_entry_t_gen(K, V) iden_concat_2(__hash_table_entry, __LINE__)

// hash_table_entry_t, ht_entry_t:
// struct for hash table entry
// 
// we have 2 hash function 
#define hash_table_entry_t(K, V) \
    struct hash_table_entry_t_gen(K, V) { \
        ht_hash_t hash, hashBackup; /* a backup hash is introduced to resolve collision */ \
        K key; \
        V value; \
    }
#define ht_entry_t hash_table_entry_t

// hash_table_entry_nodef_t, ht_entry_nodef_t:
// struct for hash table entry without struct definition
#define hash_table_entry_nodef_t(K, V) \
    struct { \
        ht_hash_t hash, hashBackup; \
        K key; \
        V value; \
    } 
#define ht_entry_nodef_t hash_table_entry_nodef_t

typedef struct {
    ht_hash_t hash, hashBackup;
    char value[1];
} hash_table_opaque_entry, ht_opaque_entry;

// hash_table_link_entry_t, ht_link_entry_t:
// struct for singly linked list hash table node
#define hash_table_link_entry_t(K, V) sll_nodef_t(ht_entry_nodef_t(K, V))
#define ht_link_entry_t hash_table_link_entry_t

// hash_table, ht_t:
// main struct for hash table
// just a bucket, a type safe entry slot
// and a temporary entry iterator
// yes, i'm using the entry field
// as an unsafe type erasure
#define hash_table(K, V) \
    struct { \
        vec(ht_link_entry_t(K, V) *) buckets; /* separate chaining head buckets */ \
        ht_entry_t(K, V) entry, *it; \
    }
#define ht_t hash_table

// hash_table_iterator_result, ht_it_ret:
// iterator result for __hash_table_find_entry_hash
// 
// needed to simulate doubly linked list here
// because deletion needed it...
typedef struct {
    sll_opaque *it; // node location
                    // if it == NULL, then there are no such field
    sll_opaque *prev; // previous node, used to remove current node
    size_t arr, n; // query for which head is the node residing
                   // if n == 0, then the entry is dangling in bucket head on bucket 'arr'
                   // need special treatment to 'delete' that 
} hash_table_iterator_result, ht_it_ret;

// hash_table_opaque_buckets, ht_opaque_buckets:
// it is really annoying too see too much warning about pointers so i made this opaque type
typedef vec(sll_opaque *) hash_table_opaque_buckets, ht_opaque_buckets;

typedef enum {
    OK,
    HASH_EXISTS,
    EMPTY_NODE, // 1. hash table is not valid, 2. no head on a specific bucket position, 3. all buckets are empty
    LINKS_FULL // used internally
} hash_table_return_code, ht_ret_code;

INLINE void __hash_table_resize(size_t entrySize, ht_opaque_buckets *buckets);

INLINE sll_opaque *__hash_table_find_best_free_slot(size_t idx, ht_hash_t hash, ht_opaque_buckets *buckets, ht_ret_code *err);

INLINE ht_it_ret __hash_table_get_entry(char *entryMem, size_t entrySize, ht_opaque_buckets *buckets);

INLINE ht_ret_code __hash_table_put_entry(char *entryMem, size_t entrySize, ht_opaque_buckets *buckets);

// hash_table_sizeof, ht_sizeof:
// determines the size for the WHOLE hash table record
#define hash_table_sizeof(ht) ( sizeof( *(ht) ) )
#define ht_sizeof hash_table_sizeof

// hash_table_entry_sizeof, ht_entry_sizeof:
// determines the size for the hash table entry
#define hash_table_entry_sizeof(ht) ( sizeof( (ht)->entry ) )
#define ht_entry_sizeof hash_table_entry_sizeof

// hash_table_key_sizeof, ht_key_sizeof:
// determines the key size for the hash table
#define hash_table_key_sizeof(ht) ( sizeof( (ht)->entry.key ) )
#define ht_key_sizeof hash_table_key_sizeof

// hash_table_value_sizeof, ht_value_sizeof:
// determines the value size for the hash table
#define hash_table_value_sizeof(ht) ( sizeof( (ht)->entry.value ) )
#define ht_value_sizeof hash_table_value_sizeof

/* hash table properties */
#define hash_table_buckets(ht) ( (ht)->buckets )
#define ht_buckets hash_table_buckets

#define hash_table_iterator(ht) ( (ht)->it )
#define ht_iter hash_table_iterator

#define hash_table_entry(ht) ( (ht)->entry )
#define ht_entry hash_table_entry

#define hash_table_entry_key(ht) ( hash_table_entry(ht).key )
#define ht_entry_key hash_table_entry_key

#define hash_table_entry_value(ht) ( hash_table_entry(ht).value )
#define ht_entry_value hash_table_entry_value

#define hash_table_entry_hash(ht) ( hash_table_entry(ht).hash )
#define ht_entry_hash hash_table_entry_hash

#define hash_table_entry_hash_backup(ht) ( hash_table_entry(ht).hashBackup )
#define ht_entry_hash_backup hash_table_entry_hash_backup

/* hash_table_valid, ht_valid: */
/* check if the hash table is valid. */
#define hash_table_valid(ht) ( (ht) != NULL && vector_valid(&ht_buckets(ht)) )
#define ht_valid hash_table_valid

// hash_table_entry, ht_entry:
// get the entry field of the target hash table
#define hash_table_entry_clear(ht) ( common_memset(&ht_entry(ht), 0, ht_entry_sizeof(ht)) )
#define ht_entry_clear hash_table_entry_clear

// hash_table_make, ht_make:
// initialize essential fields in the hash table
#define hash_table_make(ht) \
    do {\
        common_ensure((ht) != NULL); \
        common_memset((ht), 0, ht_sizeof(ht)); \
        vec_make(&ht_buckets(ht)); \
    } while(0)
#define ht_make hash_table_make

// hash_table_destroy, ht_destroy:
// destroys the hash table and fill zeroes
#define hash_table_destroy(ht) \
    do { \
        common_ensure(ht_valid(ht)); \
        for (size_t i = 0; i < vec_cap(&ht_buckets(ht)); i++) { /* iterate the buckets */ \
            /* iterate the bucket heads */ \
            vec_get(&ht_buckets(ht), 0) = vec_get(&ht_buckets(ht), i); \
            if (vec_get(&ht_buckets(ht), 0) != NULL) { /* if it is a valid list head */ \
                sll_destroy_all(vec_get(&ht_buckets(ht), 0)); /* kill them all one by one */ \
            } \
        } \
        vec_destroy(&ht_buckets(ht)); /* finalize the buckets as well */ \
        common_memset((ht), 0, ht_sizeof(ht)); \
    } while(0)
#define ht_destroy hash_table_destroy

// hash_table_exists, ht_exists: 
// check if there's already an entry with a specific hash
#ifndef hash_table_exists_value
#define hash_table_exists_value(ht, key) \
    ( \
        common_ensure(ht_valid(ht)), \
        ht_entry_clear(ht), \
        ht_entry_key(ht) = (key), \
        ht_entry_hash(ht) = ht_hash(&ht_entry_key(ht), ht_key_sizeof(ht)), \
        ht_entry_hash_backup(ht) = ht_hash_backup(&ht_entry_key(ht), ht_key_sizeof(ht)), \
        __hash_table_get_entry((char *) &ht_entry(ht), ht_entry_sizeof(ht), (ht_opaque_buckets *) &ht_buckets(ht)).it != NULL \
    )
#define ht_exists_val hash_table_exists_value
#endif

#ifndef hash_table_exists_pointer
#define hash_table_exists_pointer(ht, ptr, len) \
    ( \
        common_ensure(ht_valid(ht)), \
        ht_entry_clear(ht), \
        ht_entry_key(ht) = (ptr), \
        ht_entry_hash(ht) = ht_hash(ht_entry_key(ht), len), \
        ht_entry_hash_backup(ht) = ht_hash_backup(ht_entry_key(ht), len), \
        __hash_table_get_entry((char *) &ht_entry(ht), ht_entry_sizeof(ht), (ht_opaque_buckets *) &ht_buckets(ht)).it != NULL \
    )
#define ht_exists_ptr hash_table_exists_pointer
#endif

#ifndef hash_table_exists_string
#define hash_table_exists_string(ht, str) ht_exists_ptr(ht, (str), common_strlen(str))
#define ht_exists_str hash_table_exists_string
#endif

// hash_table_put_entry_value, ht_put_val: 
// 'insert'/put the entry using a specfic key-value pair
// the key an value will be value-copied to interal temporary field (for type safety and locality)
// and the key will be hashed afterward
// returns a HASH_EXISTS error if a field already exists
#ifndef hash_table_put_entry_value
#define hash_table_put_entry_value(ht, k, v) \
    ( \
        common_ensure(ht_valid(ht)), \
        ht_entry_key(ht) = (k), \
        ht_entry_value(ht) = (v), \
        ht_entry_hash(ht) = ht_hash(&ht_entry_key(ht), ht_key_sizeof(ht)), \
        ht_entry_hash_backup(ht) = ht_hash_backup(&ht_entry_key(ht), ht_key_sizeof(ht)), \
        __hash_table_put_entry((char *) &ht_entry(ht), ht_entry_sizeof(ht), (ht_opaque_buckets *) &ht_buckets(ht)) \
    )
#define ht_put_val hash_table_put_entry_value
#endif

// hash_table_put_entry_pointer, ht_put_ptr: 
// 'insert'/put the entry using a pointer-value pair
// returns a HASH_EXISTS error if a field already exists
#ifndef hash_table_put_entry_pointer
#define hash_table_put_entry_pointer(ht, ptr, v, len) \
    ( \
        common_ensure(ht_valid(ht)), \
        ht_entry_key(ht) = (ptr), \
        ht_entry_value(ht) = (v), \
        ht_entry_hash(ht) = ht_hash(ht_entry_key(ht), len), \
        ht_entry_hash_backup(ht) = ht_hash_backup(ht_entry_key(ht), len), \
        __hash_table_put_entry((char *) &ht_entry(ht), ht_entry_sizeof(ht), (ht_opaque_buckets *) &ht_buckets(ht)) \
    )
#define ht_put_ptr hash_table_put_entry_pointer
#endif

// hash_table_put_entry_string, ht_put_str: 
// 'insert'/put the entry using the pointer with a NULL-terminated-string-value pair
// returns a HASH_EXISTS error if a field already exists
#ifndef hash_table_put_entry_string
#define hash_table_put_entry_string(ht, str, v) ht_put_ptr(ht, str, v, common_strlen((str)))
#define ht_put_str hash_table_put_entry_string
#endif

// hash_table_get_entry_pointer, ht_get_ptr:
// find the entry by using a pointer with specific length
// most likely used for string key...
#ifndef hash_table_get_entry_pointer
#define hash_table_get_entry_pointer(ht, ptr, len) \
    ( \
        common_ensure(ht_valid(ht)), \
        ptr_rtol(ht_iter(ht)) = (ht_exists_ptr(ht, ptr, len) ? ( \
            __hash_table_get_entry((char *) &ht_entry(ht), ht_entry_sizeof(ht), (ht_opaque_buckets *) &ht_buckets(ht)).it->value \
        ) : NULL), \
        ht_iter(ht) \
    )
#define ht_get_ptr hash_table_get_entry_pointer
#endif

// hash_table_get_entry_string, ht_get_str:
// find the entry by using a NULL-terminated string
// !!! if the string is not NULL-terminated (e.g. tagged)
// then use ht_get_ptr instead
#ifndef hash_table_get_entry_string
#define hash_table_get_entry_string(ht, str) ht_get_ptr(ht, str, common_strlen((str)))
#define ht_get_str hash_table_get_entry_string
#endif

// hash_table_get_entry_value, ht_get_val:
// find the entry by using a specific valued-copied key
// the key will be value-copied to interal temporary field (for type safety and locality)
// and hashed afterward
#ifndef hash_table_get_entry_value
#define hash_table_get_entry_value(ht, key) \
    ( \
        common_ensure(ht_valid(ht)), \
        ptr_rtol(ht_iter(ht)) = (ht_exists_val(ht, key) ? ( \
            __hash_table_get_entry((char *) &ht_entry(ht), ht_entry_sizeof(ht), (ht_opaque_buckets *) &ht_buckets(ht)).it->value \
        ) : NULL), \
        ht_iter(ht) \
    )
#define ht_get_val hash_table_get_entry_value
#endif

// hash_table_get_entry_pointer_auto, ht_get_ptr_auto:
// find the entry by using a pointer with specific length
// if there are no such field, it will be assigned 'automatically'
#ifndef hash_table_get_entry_pointer_auto
#define hash_table_get_entry_pointer_auto(ht, ptr, len) \
    ( \
        common_ensure(ht_valid(ht)), \
        (!ht_exists_ptr(ht, ptr, len) ? ( \
            __hash_table_put_entry((char *) &ht_entry(ht), ht_entry_sizeof(ht), (ht_opaque_buckets *) &ht_buckets(ht)) \
        ): OK), \
        ht_get_ptr(ht, ptr, len) \
    )
#define ht_get_ptr_auto hash_table_get_entry_pointer_auto
#endif

// hash_table_get_entry_string_auto, ht_get_str_auto:
// find the entry by using a NULL-terminated string
// if there are no such field, it will be assigned 'automatically'
// !!! if the string is not NULL-terminated (e.g. tagged)
// then use ht_get_ptr_auto instead
#ifndef hash_table_get_entry_string_auto
#define hash_table_get_entry_string_auto(ht, str) ht_get_ptr_auto(ht, str, common_strlen((str)))
#define ht_get_str_auto hash_table_get_entry_string_auto
#endif

// hash_table_get_entry_value_auto, ht_get_val_auto:
// find the entry by using a specific valued-copied key
// the key will be value-copied to interal temporary field (for type safety and locality)
// and hashed afterward
// if there are no such field, it will be assigned 'automatically'
#ifndef hash_table_get_entry_value_auto
#define hash_table_get_entry_value_auto(ht, key) \
    ( \
        common_ensure(ht_valid(ht)), \
        (!ht_exists_val(ht, key) ? ( \
            __hash_table_put_entry(&ht_entry(ht), ht_entry_sizeof(ht), &ht_buckets(ht)) \
        ): OK), \
        ht_get_val(ht, key) \
    )
#define ht_get_val_auto hash_table_get_entry_value_auto
#endif

// __hash_table_try_find_and_unlink: 
// find a node that contains the specific hash
// then try to remove its existence between links
// and that node will finally be destroyed
#ifndef __hash_table_try_find_entry_and_unlink
#define __hash_table_try_find_entry_and_unlink(ht) \
    do { \
        ht_it_ret ret = __hash_table_get_entry((char *) &ht_entry(ht), ht_entry_sizeof(ht), (ht_opaque_buckets *) &ht_buckets(ht)); \
        if (ret.it != NULL) { \
            if (ret.prev != NULL) { \
                ret.prev->next = ret.it->next; \
            } \
            if (ret.n == 0) { /* if the entry is a head */ \
                ptr_rtol(vec_get(&ht_buckets(ht), ret.arr)) = ret.it->next; /* it->next is the new head */ \
            } \
            sll_destroy(ret.it); \
        } \
    } while(0)
#endif

// hash_table_delete_entry_pointer, ht_del_ptr:
// find and delete an entry with a pointer and a specific length
#ifndef hash_table_delete_entry_pointer
#define hash_table_delete_entry_pointer(ht, ptr, len) \
    do { \
        common_ensure(ht_valid(ht)); \
        ht_entry_key(ht) = (ptr); \
        ht_entry_hash(ht) = ht_hash(ht_entry_key(ht), len); \
        ht_entry_hash_backup(ht) = ht_hash_backup(ht_entry_key(ht), len); \
        __hash_table_try_find_entry_and_unlink(ht); \
    } while(0)
#define ht_del_ptr hash_table_delete_entry_pointer
#endif

// hash_table_delete_entry_string, ht_del_str:
// find and delete an entry with a NULL-terminated string
#ifndef hash_table_delete_entry_string
#define hash_table_delete_entry_string(ht, str) ht_del_ptr(ht, str, common_strlen((str)))
#define ht_del_str hash_table_delete_entry_string
#endif

// hash_table_delete_entry_string, ht_del_str:
// find and delete an entry with a specific key
#ifndef hash_table_delete_entry_value
#define hash_table_delete_entry_value(ht, k) \
    do { \
        common_ensure(ht_valid(ht)); \
        ht_entry_key(ht) = (k); \
        ht_entry_hash(ht) = ht_hash(&ht_entry_key(ht), ht_key_sizeof(ht)); \
        ht_entry_hash_backup(ht) = ht_hash_backup(&ht_entry_key(ht), ht_key_sizeof(ht)); \
        __hash_table_try_find_entry_and_unlink(ht); \
    } while(0)
#define ht_del_val hash_table_delete_entry_value
#endif

// hash_table_for_each_entry, ht_for_each:
// iterate all the entries
// working in progress
#ifndef hash_table_for_each_entry
#define hash_table_for_each_entry(ht, bucket, head, it) \
for ((bucket) = 0; (bucket) < vec_cap(&ht_buckets(ht)); (bucket)++) \
    for (ptr_rtol(it) = ptr_rtol(head) = vector_get(&ht_buckets(ht), (bucket)); (it) != NULL; ptr_rtol(it) = (it)->next)
#define ht_for_each hash_table_for_each_entry
#endif

// hash_table_clone, ht_clone:
// clone the hash table to a new memory location
// right now it was just a simple shallow clone, sorry
// 
// !!! SIMPLE SHALLOW CLONE ONLY !!!
// !!! LINK ORDER IS NOT PRESERVED !!!
#ifndef hash_table_clone
#define hash_table_clone(dest, src) \
    do { \
        sll_opaque *it, *head; \
        size_t i; \
        common_ensure(ht_entry_sizeof(dest) == ht_entry_sizeof(src)); /* weakly ensure the fields are compatible */ \
        common_ensure ((dest) != NULL && ht_valid(src)); \
        vec_clone(&ht_buckets(dest), &ht_buckets(src)); /* preserve the same layout of buckets */ \
        vec_clear_mem(&ht_buckets(dest)); /* but emptied the heads */ \
        ht_for_each(src, i, head, it) { \
            __hash_table_put_entry((char *)&it->value, ht_entry_sizeof(src), typecast(ht_opaque_buckets *, &ht_buckets(dest))); \
        } \
    } while(0)
#define ht_clone hash_table_clone
#endif

INLINE
void __hash_table_resize(size_t entrySize, ht_opaque_buckets *buckets) {
    /* algorithm:
    * 1. clone the old buckets where the entries are merged into one linear array
    * 2. relink all the bucket content one by one
    *      -> recursive call to __hash_table_put_entry
    * 3. put the current entry again
    */

    if (buckets == NULL) return;

    hash_table_opaque_buckets snapshot; // the previous view of the buckets, before expansion
    vec_clone(&snapshot, buckets); // clone the buckets for a snapshot

    vec_expand(buckets); // we're lucky here, we have them entries pointer-sized
    vec_clear_mem(buckets); // erase all old bucket heads

    for (size_t i = 0; i < vec_cap(&snapshot); i++) { // iterate the snapshot
        sll_opaque *it = vec_get(&snapshot, i);
        if (it != NULL) { // if it is not an empty head
            for (sll_opaque *it1 = it; it1 != NULL; it1 = it1->next) { // loop the list
                __hash_table_put_entry(typecast(char *, &it1->value), entrySize, buckets); // linear value-copied rehash
            }
            sll_destroy_all(it); // destroy all the old nodes
        }
    }

    vec_destroy(&snapshot);
}

INLINE
sll_opaque *__hash_table_find_best_free_slot(size_t idx, ht_hash_t hash, ht_opaque_buckets *buckets, ht_ret_code *err) {
    if (buckets == NULL || err == NULL) return NULL;

    const size_t cap = vec_cap(buckets);
    const size_t k = HASH_SEPARATE_CHAIN_MAXLINKS * cap; // pseudo-size

    size_t n = 0;
    for (; idx < cap; idx++) { // iterate the buckets
        sll_opaque *it = vec_get(buckets, idx);

        if (it == NULL) { // empty head
            *err = EMPTY_NODE;
            return typecast(sll_opaque *, vec_get_ref(buckets, idx)); // just assign the slot at the vector array head
        }

        // doubly linked list emulation
        sll_opaque *prev;
        uint32_t chainSize = 0;
        for (prev = it; ; n++, ++chainSize, prev = it, it = it->next) {
            if (prev != NULL) {
                if (typecast(ht_opaque_entry *, prev->value)->hash == hash) {
                    *err = HASH_EXISTS; // an entry is already here
                    return NULL;
                }
                if (chainSize >= HASH_SEPARATE_CHAIN_MAXLINKS) { // beyond max limit
                    break;
                }
            }
            if (it == NULL) { // end of the line
                break;
            }
        }
        if (chainSize < HASH_SEPARATE_CHAIN_MAXLINKS) {
            *err = OK;
            return (sll_opaque *)prev;
        }
        if (n / (double)k >= 0.75) { // load factor is unacceptable beyond 75%
            break;
        }
    }

    *err = LINKS_FULL;
    return NULL;
}

INLINE
ht_it_ret __hash_table_get_entry(char *entryMem, size_t entrySize, ht_opaque_buckets *buckets) {
    ht_it_ret ret; // node context
    common_memset(&ret, 0, sizeof(ht_it_ret)); // empty the context

    if (buckets == NULL || entryMem == NULL || entrySize < 1) return ret;

    const ht_opaque_entry *currentEntry = (ht_opaque_entry *)entryMem;
    const ht_hash_t currentHash = currentEntry->hash;
    const ht_hash_t currentHashBackup = currentEntry->hashBackup;

    const size_t cap = vec_cap(buckets);
    for (size_t idx = ht_locate_index(buckets, currentHash); idx < cap; idx++) { // probe start
        size_t i = 0;
        sll_opaque *prev = NULL; // doubly linked list simulation
        for (sll_opaque *it = vec_get(buckets, idx); it != NULL; i++, prev = it, it = it->next) {
            const ht_opaque_entry *entry;
            
            ptr_rtol(entry) = it->value;

            const ht_hash_t entryHash = entry->hash;
            const ht_hash_t entryHashBackup = entry->hashBackup;

            if (entryHash == currentHash && entryHashBackup == currentHashBackup) {
                ret.it = it;
                ret.prev = prev;
                ret.arr = idx;
                ret.n = i;
                return ret;
            }
        }
    }
    return ret;
}

INLINE
ht_ret_code __hash_table_put_entry(char *entryMem, size_t entrySize, ht_opaque_buckets *buckets) {
    if (entryMem == NULL || buckets == NULL) return EMPTY_NODE;

    const ht_hash_t hash = typecast(ht_opaque_entry *, entryMem)->hash;
    const size_t idx = ht_locate_index(buckets, hash);

    ht_ret_code err;
    sll_opaque *slot = __hash_table_find_best_free_slot(idx, hash, buckets, &err);

    if (err == HASH_EXISTS) return err; // wat

    if (err == LINKS_FULL) {
        __hash_table_resize(entrySize, buckets); // enlarge the bucket and rehash the entries
        return __hash_table_put_entry(entryMem, entrySize, buckets); // re-install the entry
    }

    sll_opaque **slotRef = err == EMPTY_NODE ? typecast(sll_opaque **, slot) : &slot->next;

    if (slotRef != NULL) {
        *slotRef = sll_opaque_make(entrySize); // create the node
        common_memcpy(&(*slotRef)->value, entryMem, entrySize); // shovel up memory from known entry address
    }
    return err;
}