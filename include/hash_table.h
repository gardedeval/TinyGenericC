#pragma once

#include "common.h"
#include "vector.h"
#include "linked_list.h"

/* generic (weakly generic variant) hash table implementation
 * relies on undefined behaviors, padding hacks
 * expression hacks, temporary fields
 * and memory layout exploiting,
 * granting you multiple cancers at once!
 * 
 * judging from the hacks i used,
 * i think would not work on non-x86/x64 or big-endian system,
 * but i dont have such hardware, so please test for me.
 * 
 * uses separate chaining.
 * complexities:
 * space: O(n)
 * search: O(n/km) [or O(1) for amortized constant time]
 *   where m is the total amount of bucket
 *   and k is the maximum amount of link in a bucket (defined below)
 * insert: same as search
 * delete: same as search
 * 
 * !!! LACKS MULTITHREAD AND SECURITY SUPPORT !!!
 * !!! IT IS NOT RESISTANT TO DOS ATTACK !!
 * !!! BESIDES THERE ARE PROBABLY SOME SERIOUS FLAWS !!!
 * !!! YOU SHOULD NOT THIS IN PRODUCTION !!!
 * 
 * usage: {
 *   ht_t(int, int) table; // create the table definition
 *   ht_t(const char *, double) kv; // string-number table
 *   ht_make(&table); // do this to initialize the fields
 *   ht_make(&kv);
 *   
 *   ht_put_val(&table, 1234, 5678); // drop in some value
 *   ht_put_val(&table, 12345, 1234);
 *   ht_put_val(&table, 1561, 1734);
 *   
 *   ht_get_val(&table, 1234)->value; // 5678
 *   ht_get_val(&table, 12345)->value; // 1234
 *   ht_get_val(&table, 1561)->value; // 1734
 *   
 *   ht_del_val(&table, 12345);
 *   ht_get_val(&table, 12345); // NULL
 *   
 *   ht_put_str(&kv, "the answer to life, the universe and everything", 42); // add a string-int pair
 *   ht_put_str(&kv, "e", 2.71828);
 *   ht_put_str(&kv, "pi", 3.14159);
 *   ht_put_str(&kv, "phi", 1.61803);
 *   
 *   ht_get_str(&kv, "the answer to life, the universe and everything")->value; // 42.0 (double representation)
 *   ht_get_str(&kv, "e")->value; // 2.71828
 *   ht_get_str(&kv, "pi")->value; // 3.14159
 *   ht_get_str(&kv, "phi")->value; // 1.61803
 *   
 *   ht_get_str_auto(&kv, "g")->value = 9.80665; // if the field does not exist, automatically create one
 *   ht_get_str(&kv, "g")->value; // 9.80665
 *   
 *   ht_del_str(&kv, "e");
 *   ht_get_str(&kv, "e"); // NULL
 *
 *   ht_destroy(&table); // clean up the resources used
 *   ht_destroy(&kv);
 * }
 */

#define HASH_TABLE_VER "0.0.1" // yep, still alpha

// your choice, i like 8 in flavor of sufficiently-spreaded buckets
// and less linked list traversal
// if you set this to 1, it behaves like linear probing,
// super cool eh?
#define HASH_SEPARATE_CHAIN_MAXLINKS 8  

// integral type for hash table hash value
typedef uint32_t hash_table_hash_t;

// hash_table_entry_t, ht_entry_t:
// struct for hash table entry
#define hash_table_entry_t(K, V) \
    struct { \
        hash_table_hash_t hash; /* the only non-arbitary field so I put it on top for pointer hacking */ \
        K key; \
        V value; \
    } \

#define ht_entry_t hash_table_entry_t

// hash_table_link_entry_t, ht_link_entry_t:
// struct for singly linked list hash table node
#define hash_table_link_entry_t(K, V) sll_nodef_t(ht_entry_t(K, V))
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
        ht_entry_t(K, V) entry, *it; /* temporary fields */ \
    }
#define ht_t hash_table

// hash_table_opaque_buckets, ht_opaque_buckets:
// it is really annoying too see too much warning about pointers so i made this opaque type
typedef vec(sll_opaque *) hash_table_opaque_buckets, ht_opaque_buckets;

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

// hash_table_buckets, ht_buckets:
// get the buckets field of the target hash table
// NULL CHECK NOT INCLUDED
#define hash_table_buckets(ht) ( (ht)->buckets )
#define ht_buckets hash_table_buckets

// hash_table_entry, ht_entry:
// get the entry field of the target hash table
// NULL CHECK NOT INCLUDED
#define hash_table_entry(ht) ( (ht)->entry )
#define ht_entry hash_table_entry

// hash_table_entry, ht_entry:
// get the entry field of the target hash table
// NULL CHECK NOT INCLUDED
#define hash_table_entry_key(ht) ( (ht)->entry.key )
#define ht_entry_key hash_table_entry_key

// hash_table_entry, ht_entry:
// get the entry field of the target hash table
// NULL CHECK NOT INCLUDED
#define hash_table_entry_value(ht) ( (ht)->entry.value )
#define ht_entry_value hash_table_entry_value

// hash_table_entry, ht_entry:
// get the entry field of the target hash table
// NULL CHECK NOT INCLUDED
#define hash_table_entry_hash(ht) ( (ht)->entry.hash )
#define ht_entry_hash hash_table_entry_hash

// hash_table_entry, ht_entry:
// get the entry field of the target hash table
// NULL CHECK NOT INCLUDED
#define hash_table_valid(ht) ( (ht) != NULL && vector_valid(&ht_buckets(ht)) )
#define ht_valid hash_table_valid

// hash_table_entry, ht_entry:
// get the entry field of the target hash table
// NULL CHECK NOT INCLUDED
#define hash_table_entry_clear(ht) ( common_memset(&(ht)->entry, 0, sizeof((ht)->entry)) )
#define ht_entry_clear hash_table_entry_clear

// __hash_table_hash, hash_table_hash, ht_hash:
// for a memory block with a size n, 
// calculate the (hopefully unique) hash of that memory block byte-by-byte
// currently using fnv1a-32 but can be change to something better
hash_table_hash_t __hash_table_hash(char *mem, size_t len);
#define hash_table_hash(mem, len) ( __hash_table_hash((char*) (mem), len) )
#define ht_hash hash_table_hash

// hash_table_make, ht_make:
// initialize essential fields in the hash table
#define hash_table_make(ht) \
    do {\
        common_ensure((ht) != NULL); \
        common_memset((ht), 0, ht_sizeof(ht)); \
        vec_make(&ht_buckets(ht)); \
    } while(0)
#define ht_make hash_table_make

// __hash_table_destroy:
// helper for deleting hash table without duplicating common code
// hash_table_destroy, ht_destroy:
// destroys the hash table and fill zeroes
void __hash_table_destroy(hash_table_opaque_buckets *buckets);
#define hash_table_destroy(ht) \
    do { \
        common_ensure(ht_valid(ht)); \
        __hash_table_destroy((ht_opaque_buckets *) &ht_buckets(ht)); \
        vec_destroy(&ht_buckets(ht)); /* finalize the buckets as well */ \
        common_memset((ht), 0, ht_sizeof(ht)); \
    } while(0)

#define ht_destroy hash_table_destroy

// hash_table_return_code, ht_ret_code:
// return code for __hash_table_find_best_free_slot
typedef enum {
    OK, 
    HASH_EXISTS,
    EMPTY_NODE, // 1. hash table is not valid, 2. no head on a specific bucket position, 3. all buckets are empty
    LINKS_FULL // used internally
} hash_table_return_code, ht_ret_code;

sll_opaque *__hash_table_find_best_free_slot(size_t idx, hash_table_hash_t hash, hash_table_opaque_buckets *buckets, ht_ret_code *err);

// hash_table_iterator_result, ht_it_ret:
// iterator result for __hash_table_find_entry_hash
// 
// needed to simulate doubly linked list here
// because we deletion support need it...
typedef struct {
    sll_opaque *it; // node location
    // if it == NULL, then there are no such field
    sll_opaque *prev; // previous node, used to remove current node
    size_t arr, n; // query for which head is the node residing
    // if n == 0, then the entry is dangling in bucket head on bucket 'arr'
    // need special treatment to 'delete' that 
} hash_table_iterator_result, ht_it_ret;

// __hash_table_find_entry_hash: find the entry with a corresponding hash 
ht_it_ret __hash_table_find_entry_hash(hash_table_hash_t hash, hash_table_opaque_buckets *buckets);

// __hash_table_get_entry_hash, __hash_table_put_entry: 
// 'insert'/put the entry using the best location available
ht_ret_code __hash_table_put_entry(char *entryMem, size_t entrySize, hash_table_opaque_buckets *buckets);
#define __hash_table_get_entry_hash(ht, hash) ( __hash_table_find_entry_hash((hash), (ht_opaque_buckets *) &ht_buckets(ht)) )

// hash_table_exists, ht_exists: 
// check if there's already an entry with a specific hash
#define hash_table_exists(ht, hash) ( __hash_table_get_entry_hash( ht, hash ).it != NULL )
#define ht_exists hash_table_exists

// __hash_table_load_entry_reference:
// helper function to extract the value field from node pointer
#define __hash_table_load_entry_reference(ht, hash) \
    ( \
       *((void **) &(ht)->it) = &__hash_table_get_entry_hash( ht, hash ).it->value, \
       (ht)->it \
    )

// hash_table_get_entry_pointer, ht_get_ptr:
// find the entry by using a pointer with specific length
// most likely used for string key...
#define hash_table_get_entry_pointer(ht, ptr, len) \
    ( \
        common_ensure(ht_valid(ht)), \
        ht_entry_clear(ht), \
        ht_entry_key(ht) = (ptr), \
        ht_entry_hash(ht) = ht_hash(ht_entry_key(ht), len), \
        ht_exists( ht, ht_entry_hash(ht) ) ? __hash_table_load_entry_reference(ht, ht_entry_hash(ht)) : NULL \
    )
#define ht_get_ptr hash_table_get_entry_pointer

// hash_table_get_entry_string, ht_get_str:
// find the entry by using a NULL-terminated string
// !!! if the string is not NULL-terminated (e.g. tagged)
// then use ht_get_ptr instead
#define hash_table_get_entry_string(ht, str) ht_get_ptr(ht, str, common_strlen((str)))
#define ht_get_str hash_table_get_entry_string

// hash_table_get_entry_value, ht_get_val:
// find the entry by using a specific valued-copied key
// the key will be value-copied to interal temporary field (for type safety and locality)
// and hashed afterward
#define hash_table_get_entry_value(ht, key) \
    ( \
        common_ensure(ht_valid(ht)), \
        ht_entry_clear(ht), \
        ht_entry_key(ht) = (key), \
        ht_entry_hash(ht) = ht_hash(&ht_entry_key(ht), ht_key_sizeof(ht)), \
        ht_exists( ht, ht_entry_hash(ht) ) ? __hash_table_load_entry_reference(ht, ht_entry_hash(ht)) : NULL \
    )

#define ht_get_val hash_table_get_entry_value

// hash_table_get_entry_pointer_auto, ht_get_ptr_auto:
// find the entry by using a pointer with specific length
// if there are no such field, it will be assigned 'automatically'
#define hash_table_get_entry_pointer_auto(ht, ptr, len) \
    ( \
        common_ensure(ht_valid(ht)), \
        ht_entry_clear(ht), \
        ht_entry_key(ht) = (ptr), \
        ht_entry_hash(ht) = ht_hash(ht_entry_key(ht), len), \
        ( \
            !ht_exists(ht, ht_entry_hash(ht)) ? ( \
                __hash_table_put_entry((char *) &ht_entry(ht), ht_entry_sizeof(ht), (ht_opaque_buckets *) &ht_buckets(ht)) \
            ) : OK \
        ), \
        __hash_table_load_entry_reference(ht, ht_entry_hash(ht)) \
    )
#define ht_get_ptr_auto hash_table_get_entry_pointer_auto

// hash_table_get_entry_string_auto, ht_get_str_auto:
// find the entry by using a NULL-terminated string
// if there are no such field, it will be assigned 'automatically'
// !!! if the string is not NULL-terminated (e.g. tagged)
// then use ht_get_ptr_auto instead
#define hash_table_get_entry_string_auto(ht, str) ht_get_ptr_auto(ht, str, common_strlen((str)))
#define ht_get_str_auto hash_table_get_entry_string_auto

// hash_table_get_entry_value_auto, ht_get_val_auto:
// find the entry by using a specific valued-copied key
// the key will be value-copied to interal temporary field (for type safety and locality)
// and hashed afterward
// if there are no such field, it will be assigned 'automatically'
#define hash_table_get_entry_value_auto(ht, key) \
    ( \
        common_ensure(ht_valid(ht)), \
        ht_entry_clear(ht), \
        ht_entry_key(ht) = (key), \
        ht_entry_hash(ht) = ht_hash(&ht_entry_key(ht), ht_key_sizeof(ht)), \
        ( \
            !ht_exists(ht, ht_entry_hash(ht)) ? ( \
                __hash_table_put_entry((char *) &ht_entry(ht), ht_entry_sizeof(ht), (ht_opaque_buckets *) &ht_buckets(ht)) \
            ) : 0 \
        ), \
        __hash_table_load_entry_reference(ht, ht_entry_hash(ht)) \
    )
#define ht_get_val_auto hash_table_get_entry_value_auto

// hash_table_put_entry_pointer, ht_put_ptr: 
// 'insert'/put the entry using a pointer-value pair
// returns a HASH_EXISTS error if a field already exists
#define hash_table_put_entry_pointer(ht, ptr, v, len) \
    ( \
        common_ensure(ht_valid(ht)), \
        ht_entry_key(ht) = (ptr), \
        ht_entry_value(ht) = (v), \
        ht_entry_hash(ht) = ht_hash(ht_entry_key(ht), len), \
        __hash_table_put_entry((char *) &ht_entry(ht), ht_entry_sizeof(ht), (ht_opaque_buckets *) &ht_buckets(ht)) \
    )
#define ht_put_ptr hash_table_put_entry_pointer

// hash_table_put_entry_string, ht_put_str: 
// 'insert'/put the entry using the pointer with a NULL-terminated-string-value pair
// returns a HASH_EXISTS error if a field already exists
#define hash_table_put_entry_string(ht, str, v) ht_put_ptr(ht, str, v, common_strlen((str)))
#define ht_put_str hash_table_put_entry_string

// hash_table_put_entry_value, ht_put_val: 
// 'insert'/put the entry using a specfic key-value pair
// the key an value will be value-copied to interal temporary field (for type safety and locality)
// and the key will be hashed afterward
// returns a HASH_EXISTS error if a field already exists
#define hash_table_put_entry_value(ht, k, v) \
    ( \
       common_ensure(ht_valid(ht)), \
        ht_entry_key(ht) = (k), \
        ht_entry_value(ht) = (v), \
        ht_entry_hash(ht) = ht_hash(&ht_entry_key(ht), ht_key_sizeof(ht)), \
        __hash_table_put_entry((char *) &ht_entry(ht), ht_entry_sizeof(ht), (ht_opaque_buckets *) &ht_buckets(ht)) \
    )
#define ht_put_val hash_table_put_entry_value

// __hash_table_try_find_and_unlink: 
// find a node that contains the specific hash
// then try to remove its existence between links
// and that node will finally be destroyed
#define __hash_table_try_find_and_unlink(ht, hash) \
    do { \
        ht_it_ret ret; \
        common_ensure(ht_valid(ht)); \
        ret = __hash_table_get_entry_hash(ht, hash); \
        if (ret.it != NULL) { \
            if (ret.prev != NULL) { \
                ret.prev->next = ret.it->next; \
            } \
            if (ret.n == 0) { /* if the entry is a head */ \
                vec_get(&ht_buckets(ht), ret.arr) = ret.it->next; /* it->next is the new head */ \
            } \
            sll_destroy(ret.it); \
        } \
    } while(0) \

// hash_table_delete_entry_pointer, ht_del_ptr:
// find and delete an entry with a pointer and a specific length
#define hash_table_delete_entry_pointer(ht, ptr, len) \
    do { \
        common_ensure(ht_valid(ht)); \
        ht_entry_key(ht) = (ptr); \
        __hash_table_try_find_and_unlink(ht, ht_hash(ht_entry_key(ht), len)); \
    } while(0)
#define ht_del_ptr hash_table_delete_entry_pointer

// hash_table_delete_entry_string, ht_del_str:
// find and delete an entry with a NULL-terminated string
#define hash_table_delete_entry_string(ht, str) ht_del_ptr(ht, str, common_strlen((str)))
#define ht_del_str hash_table_delete_entry_string

// hash_table_delete_entry_string, ht_del_str:
// find and delete an entry with a specific key
#define hash_table_delete_entry_value(ht, k) \
    do { \
        common_ensure(ht_valid(ht)); \
        ht_entry_key(ht) = (k); \
        __hash_table_try_find_and_unlink(ht, ht_hash(&ht_entry_key(ht), ht_key_sizeof(ht))); \
    } while(0)
#define ht_del_val hash_table_delete_entry_value

// hash_table_for_each_entry, ht_for_each:
// iterate all the entries
// working in progress
#define hash_table_for_each_entry(ht, bucket, head, it) \
for ((bucket) = 0; (bucket) < vec_cap(&ht_buckets(ht)); (bucket)++) \
    for ((it) = (head) = vector_get(&ht_buckets(ht), (bucket)); (it) != NULL; (it) = (it)->next)
#define ht_for_each hash_table_for_each_entry

// hash_table_clone, ht_clone:
// clone the hash table to a new memory location
// right now it was just a simple shallow clone, sorry
// 
// !!! SIMPLE SHALLOW CLONE ONLY !!!
// !!! LINK ORDER IS NOT PRESERVED !!!
#define hash_table_clone(dest, src) \
    do { \
        sll_opaque *it, *head; \
        size_t i; \
        common_ensure(ht_entry_sizeof(dest) == ht_entry_sizeof(src)); /* weakly ensure the fields are compatible */ \
        common_ensure ((dest) != NULL && ht_valid(src)); \
        vec_clone(&ht_buckets(dest), &ht_buckets(src)); /* preserve the same layout of buckets */ \
        vec_clear_mem(&ht_buckets(dest)); /* but emptied the heads */ \
        ht_for_each(src, i, head, it) { \
            __hash_table_put_entry((char *)&it->value, ht_entry_sizeof(src), &ht_buckets(dest)); \
        } \
    } while(0)
#define ht_clone hash_table_clone
