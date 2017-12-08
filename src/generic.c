#include "common.h"
#include "hash_table.h"
#include "json.h"
#include "tagged_mem.h"

const uint32_t fnv1a_prime = 16777619;
const uint32_t fnv1a_offset_basis = 2166136261;

// fnv1a, 32-bit variant.
hash_table_hash_t __hash_table_hash(char *mem, size_t len) {
    size_t i;
    uint32_t hash;

    if (mem == NULL) {
        return 0;
    }

    hash = fnv1a_offset_basis;
    for (i = 0; i < len; i++) {
        hash ^= mem[i];
        hash *= fnv1a_prime;
    }
    return (hash_table_hash_t) hash;
}

void __hash_table_destroy(hash_table_opaque_buckets *buckets) {
    size_t i;
    sll_opaque *it;

    if (buckets == NULL) return;
    
    for (i = 0; i < vec_cap(buckets); i++) { // iterate the buckets
        // iterate the bucket heads
        it = vec_get(buckets, i);
        if (it != NULL) { // if it is a valid list head
            sll_destroy_all(it); // kill them all one by one
        }
    }
}

sll_opaque *__hash_table_find_best_free_slot(size_t idx, hash_table_hash_t hash, hash_table_opaque_buckets *buckets, ht_ret_code *err) {
    size_t n, cap;
    uint32_t chainSize;
    sll_opaque *it, *prev;
    cap = vec_cap(buckets);
    const size_t k = HASH_SEPARATE_CHAIN_MAXLINKS * cap; // pseudo-size

    if (buckets == NULL || err == NULL) return NULL;

    n = 0;
    for (; idx < cap; idx++) { // iterate the buckets
        chainSize = 0;
        it = vec_get(buckets, idx);

        if (it == NULL) { // empty head
            *err = EMPTY_NODE;
            return (sll_opaque *) vec_get_ref(buckets, idx); // just assign the slot at the vector array head
        }

        // doubly linked list emulation
        for (prev = it; ; n++, ++chainSize, prev = it, it = it->next) {
            if (prev != NULL) { 
                if (*(uint32_t *)prev->value == hash) {
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

ht_it_ret __hash_table_find_entry_hash(hash_table_hash_t hash, hash_table_opaque_buckets *buckets) {
    size_t idx, cap, i;
    sll_opaque *prev, *it;
    ht_it_ret ret; // node context

    common_memset(&ret, 0, sizeof(ht_it_ret)); // empty the context

    if (buckets == NULL) return ret;

    cap = vec_cap(buckets);
    idx = hash % cap;

    for (; idx < cap; idx++) { // probe start
        i = 0;
        prev = NULL; // doubly linked list simulation
        for (it = vec_get(buckets, idx); it != NULL; i++, prev = it, it = it->next) {
            if (*(uint32_t *)it->value == hash) {
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

// __hash_table_put_entry and __hash_table_resize can be mutually recursive
// so we must define them as a global function

static void __hash_table_resize(size_t entrySize, hash_table_opaque_buckets *buckets) {
    /* algorithm:
    * 1. clone the old buckets where the entries are merged into one linear array
    * 2. relink all the bucket content one by one
    *      -> recursive call to __hash_table_put_entry
    * 3. put the current entry again
    */
    sll_opaque *it, *it1;
    hash_table_opaque_buckets snapshot; // the previous view of the buckets, before expansion
    size_t i;

    if (buckets == NULL) return;

    vec_clone(&snapshot, buckets); // clone the buckets for a snapshot

    vec_expand(buckets); // we're lucky here, we have them pointers-sized
    vec_clear_mem(buckets); // erase all old bucket heads

    for (i = 0; i < vec_cap(&snapshot); i++) { // iterate the snapshot
        it = it1 = vec_get(&snapshot, i);
        if (it != NULL) { // if it is not an empty head
            for (; it1 != NULL; it1 = it1->next) { // loop the list
                __hash_table_put_entry((char *)&it1->value, entrySize, buckets); // linear value-copied rehash
            }
            sll_destroy_all(it); // destroy all the nodes
        }
    }

    vec_destroy(&snapshot);
}

ht_ret_code __hash_table_put_entry(char *entryMem, size_t entrySize, hash_table_opaque_buckets *buckets) {
    hash_table_hash_t hash;
    size_t idx;
    sll_opaque *slot, **slotRef;
    ht_ret_code err;

    if (entryMem == NULL || buckets == NULL) return EMPTY_NODE;

    hash = *(uint32_t *)entryMem;
    idx = hash % vec_cap(buckets);
    slot = __hash_table_find_best_free_slot(idx, hash, buckets, &err);

    if (err == HASH_EXISTS) return HASH_EXISTS; // what

    if (err == LINKS_FULL) {
        __hash_table_resize(entrySize, buckets); // enlarge the bucket and rehash the entries
        return __hash_table_put_entry(entryMem, entrySize, buckets); // re-install the entry
    }

    slotRef = err == EMPTY_NODE ? (sll_opaque **)slot : &slot->next;

    if (slotRef != NULL) {
        *slotRef = sll_opaque_make(entrySize); // create the node
        common_memcpy(&(*slotRef)->value, entryMem, entrySize); // shovel up memory from known entry address
    }

    return OK;
}


json_context *json_make(json_context *j) {
    if (j == NULL) return NULL;
    common_memset((j), 0, json_sizeof(j));
    json_type(j) = JSON_NULL;
    return j;
}

json_context *json_make_boolean(json_context *j, uint8_t val) {
    if (j == NULL) return NULL;
    json_make(j);
    json_type(j) = JSON_BOOLEAN;
    json_boolean(j) = val;
    return j;
}

json_context *json_make_number(json_context *j, double_t val) {
    if (j == NULL) return NULL;
    json_make(j);
    json_type(j) = JSON_NUMBER;
    json_number(j) = val;
    return j;
}

json_context *json_make_string(json_context *j, char *str, size_t len) {
    if (j == NULL) return NULL;
    json_make(j);
    json_type(j) = JSON_STRING;
    json_string(j).len = len;
    json_string(j).mem = common_memcpy(common_malloc(len), str, len);
    return j;
}

json_context *json_make_array(json_context *j) {
    if (j == NULL) return NULL;
    json_make(j);
    json_type(j) = JSON_ARRAY;
    json_array(j) = common_malloc(vec_sizeof(json_array(j)));
    vec_make(json_array(j));
    return j;
}

json_context *json_make_object(json_context *j) {
    if (j == NULL) return NULL;
    json_make(j);
    json_type(j) = JSON_OBJECT;
    json_object(j) = common_malloc(ht_sizeof(json_object(j)));
    ht_make(json_object(j));
    return j;
}

void json_array_push(json_context *arr, json_context *j) {
    if (arr == NULL || j == NULL || json_type(arr) != JSON_ARRAY) return;

    vec_push(json_array(arr), j);
}

json_context *json_array_get(json_context *arr, size_t n) {
    if (arr == NULL || json_type(arr) != JSON_ARRAY) return NULL;

    return vec_get(json_array(arr), n);
}

json_context *json_array_pop(json_context *arr) {
    if (arr == NULL || json_type(arr) != JSON_ARRAY) return NULL;

    return vec_pop(json_array(arr));
}

void json_object_put(json_context *obj, const char *key, json_context *j) {
    char *managedKey;
    if (obj == NULL || key == NULL || j == NULL || json_type(obj) != JSON_OBJECT) {
        return;
    }
    if (json_object_get(obj, key) != NULL) return;

    managedKey = common_strdup(key);
    common_ensure_message(managedKey != NULL, "Out of memory");

    ht_put_str(json_object(obj), managedKey, j);
}

json_context *json_object_get(json_context *obj, const char *key) {
    ht_entry_t(const char *, json_context *) *pair;

    if (obj == NULL || key == NULL || json_type(obj) != JSON_OBJECT) {
        return NULL;
    }

    pair = ht_get_str(json_object(obj), key);
    return pair != NULL ? pair->value : NULL;
}

void json_object_delete(json_context *obj, const char *key) {
    json_context *j;

    if (obj == NULL || key == NULL || json_type(obj) != JSON_OBJECT) {
        return;
    }

    j = json_object_get(obj, key);

    if (j != NULL) {
        json_destroy(j);
        ht_del_str(json_object(obj), key);
    }
}

// TODO: rewrite to utilize vector_view
json_serialize_ret json_serialize(json_context *j, char *buf, int len, size_t *written) {
    if (j == NULL || buf == NULL) return SERIAL_INVALID;
    if (len < 0) return SERIAL_NOMEM;

    switch (json_type(j)) {
        case JSON_NULL:
        {
            // null

            const char *str = "null";
            size_t strLen = common_strlen(str);
            if ((int)strLen > len) return SERIAL_NOMEM;
            if (written != NULL) *written = strLen;
            common_memcpy(buf, str, strLen);
        }
        break;
        case JSON_BOOLEAN:
        {
            const char *str = json_boolean(j) ? "true" : "false";
            size_t strLen = common_strlen(str);
            if ((int)strLen > len) return SERIAL_NOMEM;
            if (written != NULL) *written = strLen;
            common_memcpy(buf, str, strLen);
        }
        break;
        case JSON_NUMBER:
        {
            // number
            size_t ret = snprintf(buf, len, "%g", json_number(j));
            if (ret < 1) return SERIAL_INVALID;
            if (written != NULL) *written = ret;
        }
        break;
        case JSON_STRING:
        {
            // " ... "

            const char *str = json_string(j).mem;
            size_t strLen = json_string(j).len;
            // first quote + string length + last quote
            if ((1 + (int)strLen + 1) > len) return SERIAL_NOMEM;
            if (written != NULL) *written = 1 + strLen + 1;

            size_t i = 0;
            buf[i++] = '"';
            common_memcpy(buf + i, str, strLen);
            i += strLen - 1;
            buf[++i] = '"';
        }
        break;
        case JSON_ARRAY:
        {
            // [ ... ]

            int bufLen = len;
            size_t i = 0;
            buf[--bufLen, i++] = '[';

            size_t it;
            json_context **value;
            vec_for_each(json_array(j), it, value) {
                if (value != NULL) {
                    size_t written0;
                    json_serialize_ret ret = json_serialize(*value, buf + i, bufLen, &written0);
                    if (ret == SERIAL_OK) {
                        i += written0 - 1;
                        bufLen -= written0;
                    } else return ret;

                    // not the last element
                    if ((it + 1) < vec_index(json_array(j))) {
                        if ((i + 2) < (size_t) len) {
                            buf[bufLen--, ++i] = ','; // append a comma
                            buf[bufLen--, ++i] = ' '; // and a space
                        } else return SERIAL_NOMEM;
                    }
                    bufLen--, ++i;
                } else return SERIAL_INVALID;
            }


            if (i < (size_t) len) {
                buf[i++] = ']';
                if (written != NULL) *written = i;
            } else return SERIAL_NOMEM;
        }
        break;
        case JSON_OBJECT:
        {
            // { ... }
           

            size_t n = 0; // total entries
            size_t bucket;
            ht_link_entry_t(const char *, json_context *) *head, *it;
            ht_for_each(json_object(j), bucket, head, it) n++;

            size_t i = 0;
            size_t bufLen = len;
            buf[--bufLen, i++] = '{';

            size_t e = 0;
            ht_for_each(json_object(j), bucket, head, it) {
                const char *str = it->value.key;
                size_t strLen = strlen(str);

                if (str == NULL) return SERIAL_INVALID;

                // first quote + string length + last quote
                if ((1 + (int) strLen + 1) > len) return SERIAL_NOMEM;

                buf[bufLen--, i++] = '"';

                common_memcpy(buf + i, str, strLen);
                i += strLen - 1;
                bufLen -= strLen;

                buf[bufLen--, ++i] = '"';
                buf[bufLen--, ++i] = ':';
                buf[bufLen--, ++i] = ' ';
                i++;

                size_t written0;
                json_serialize_ret ret = json_serialize(it->value.value, buf + i, bufLen, &written0);
                if (ret == SERIAL_OK) {
                    i += written0 - 1;
                    bufLen -= written0;
                } else return ret;

                // not the last element
                if ((e + 1) < n) {
                    if ((i + 2) < (size_t) len) {
                        buf[bufLen--, ++i] = ',';
                        buf[bufLen--, ++i] = ' ';
                    } else return SERIAL_NOMEM;
                }
                bufLen--, ++i;
                e++;
            }

            if (i < (size_t) len) {
                buf[i++] = '}';
                if (written) *written = i;
            } else return SERIAL_NOMEM;
        }
        break;

        default: return SERIAL_UNKNOWN;
    }

    return SERIAL_OK;
}

void json_destroy(json_context *j) {
    if (j == NULL) return;
    switch (json_type(j)) {
        case JSON_STRING:
        {
            common_free((void *)json_string(j).mem);
        }
        break;
        case JSON_ARRAY:
        {
            size_t it;
            json_context **value;
            vec_for_each(json_array(j), it, value) {
                if (value != NULL) {
                    json_destroy(*value);
                }

            }
            vec_destroy(json_array(j));
            common_free(json_array(j));
        }
        break;
        case JSON_OBJECT:
        {
            ht_link_entry_t(const char *, json_context *) *head, *it;
            size_t i;

            ht_for_each(json_object(j), i, head, it) {
                common_free((void *)it->value.key);
                json_destroy(it->value.value);
            }
            ht_destroy(json_object(j));
            common_free(json_object(j));
        }
        break;
        default: break;
    }
    common_memset(j, 0, json_sizeof(j));
}

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

size_t __vector_expand_to_nearest_2n(size_t n) {
    n--;
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
    if (sizeof(size_t) == sizeof(uint64_t)) {
        n |= n >> 32;
    }

    n++;
    return n;
}