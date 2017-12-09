#pragma once

#include "common.h"
#include "vector.h"
#include "hash_table.h"

typedef enum {
    SERIAL_OK,
    SERIAL_INVALID,
    SERIAL_NOMEM,
    SERIAL_UNKNOWN,
} json_serialize_ret;

typedef enum {
    DESERIAL_OK,
    DESERIAL_INVALID,
    DESERIAL_UNKNOWN,
} json_deserialize_ret;

typedef enum json_type {
    JSON_NULL,
    JSON_BOOLEAN,
    JSON_NUMBER,
    JSON_STRING,
    JSON_ARRAY,
    JSON_OBJECT,
} json_type;

typedef struct json_context {
    json_type type;
    union {
        uint8_t boolean;
        double number;
        struct {
            size_t len;
            const char *mem;
        } string;
        vec(struct json_context *) *array;
        ht_t(const char *, struct json_context *) *object;
    } pinned;
} json_context, json;

#define json_type(j) ( (j)->type )

#define json_boolean(j) ( (j)->pinned.boolean )
#define json_number(j) ( (j)->pinned.number )
#define json_string(j) ( (j)->pinned.string )
#define json_array(j) ( (j)->pinned.array )
#define json_object(j) ( (j)->pinned.object )

#define json_sizeof(j) ( sizeof( *(j) ) )


INLINE
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

INLINE 
json_context *json_make(json_context *j) {
    if (j == NULL) return NULL;
    common_memset(j, 0, json_sizeof(j));
    json_type(j) = JSON_NULL;
    return j;
}

INLINE
json_context *json_make_boolean(json_context *j, uint8_t val) {
    if (j == NULL) return NULL;
    json_make(j);
    json_type(j) = JSON_BOOLEAN;
    json_boolean(j) = val;
    return j;
}

INLINE
json_context *json_make_number(json_context *j, double_t val) {
    if (j == NULL) return NULL;
    json_make(j);
    json_type(j) = JSON_NUMBER;
    json_number(j) = val;
    return j;
}

INLINE
json_context *json_make_string(json_context *j, char *str, size_t len) {
    if (j == NULL) return NULL;
    json_make(j);
    json_type(j) = JSON_STRING;
    json_string(j).len = len;
    json_string(j).mem = common_memcpy(common_malloc(len), str, len);
    return j;
}

INLINE
json_context *json_make_array(json_context *j) {
    if (j == NULL) return NULL;
    json_make(j);
    json_type(j) = JSON_ARRAY;
    json_array(j) = common_malloc(vec_sizeof(json_array(j)));
    vec_make(json_array(j));
    return j;
}

INLINE
json_context *json_make_object(json_context *j) {
    if (j == NULL) return NULL;
    json_make(j);
    json_type(j) = JSON_OBJECT;
    json_object(j) = common_malloc(ht_sizeof(json_object(j)));
    ht_make(json_object(j));
    return j;
}

INLINE
void json_array_push(json_context *arr, json_context *j) {
    if (arr == NULL || j == NULL || json_type(arr) != JSON_ARRAY) return;

    vec_push(json_array(arr), j);
}

INLINE
json_context *json_array_get(json_context *arr, size_t n) {
    if (arr == NULL || json_type(arr) != JSON_ARRAY) return NULL;

    return vec_get(json_array(arr), n);
}

INLINE
json_context *json_array_pop(json_context *arr) {
    if (arr == NULL || json_type(arr) != JSON_ARRAY) return NULL;

    return vec_pop(json_array(arr));
}

INLINE
json_context *json_object_get(json_context *obj, const char *key) {
    if (obj == NULL || key == NULL || json_type(obj) != JSON_OBJECT) {
        return NULL;
    }

    ht_entry_t(const char *, json_context *) *pair;

    ptr_rtol(pair) = ht_get_str(json_object(obj), key);
    return pair != NULL ? pair->value : NULL;
}

INLINE
void json_object_put(json_context *obj, const char *key, json_context *j) {
    if (obj == NULL || key == NULL || j == NULL || json_type(obj) != JSON_OBJECT) {
        return;
    }
    if (json_object_get(obj, key) != NULL) return;

    char *managedKey = common_strdup(key);
    common_ensure_message(managedKey != NULL, "Out of memory");

    ht_put_str(json_object(obj), managedKey, j);
}

INLINE
void json_object_delete(json_context *obj, const char *key) {
    if (obj == NULL || key == NULL || json_type(obj) != JSON_OBJECT) {
        return;
    }

    json_context *j = json_object_get(obj, key);

    if (j != NULL) {
        json_destroy(j);
        ht_del_str(json_object(obj), key);
    }
}

// TODO: rewrite to utilize vector_view
INLINE
json_serialize_ret json_serialize(json_context *j, char *buf, int len, size_t *written) {
    if (j == NULL || buf == NULL) return SERIAL_INVALID;
    if (len < 0) return SERIAL_NOMEM;

    switch (json_type(j)) {
        case JSON_NULL:
        {
            // null

            const char *str = "null";
            const size_t strLen = common_strlen(str);
            if ((int)strLen > len) return SERIAL_NOMEM;
            if (written != NULL) *written = strLen;
            common_memcpy(buf, str, strLen);
        }
        break;
        case JSON_BOOLEAN:
        {
            const char *str = json_boolean(j) ? "true" : "false";
            const size_t strLen = common_strlen(str);
            if ((int)strLen > len) return SERIAL_NOMEM;
            if (written != NULL) *written = strLen;
            common_memcpy(buf, str, strLen);
        }
        break;
        case JSON_NUMBER:
        {
            // number
            const size_t ret = snprintf(buf, len, "%g", json_number(j));
            if (ret < 1) return SERIAL_INVALID;
            if (written != NULL) *written = ret;
        }
        break;
        case JSON_STRING:
        {
            // " ... "

            const char *str = json_string(j).mem;
            const size_t strLen = json_string(j).len;
            // first quote + string length + last quote
            if (1 + (int)strLen + 1 > len) return SERIAL_NOMEM;
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
                        bufLen -= (int)written0;
                    } else return ret;

                    // not the last element
                    if (it + 1 < vec_index(json_array(j))) {
                        if (i + 2 < (size_t)len) {
                            buf[bufLen--, ++i] = ','; // append a comma
                            buf[bufLen--, ++i] = ' '; // and a space
                        } else return SERIAL_NOMEM;
                    }
                    bufLen--, ++i;
                } else return SERIAL_INVALID;
            }


            if (i < (size_t)len) {
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
            int bufLen = len;
            buf[--bufLen, i++] = '{';

            size_t e = 0;
            ht_for_each(json_object(j), bucket, head, it) {
                const char *str = it->value.key;
                const size_t strLen = strlen(str);

                if (str == NULL) return SERIAL_INVALID;

                // first quote + string length + last quote
                if (1 + (int)strLen + 1 > len) return SERIAL_NOMEM;

                buf[bufLen--, i++] = '"';

                common_memcpy(buf + i, str, (int)strLen);
                i += (int)strLen - 1;
                bufLen -= (int)strLen;

                buf[bufLen--, ++i] = '"';
                buf[bufLen--, ++i] = ':';
                buf[bufLen--, ++i] = ' ';
                i++;

                size_t written0;
                json_serialize_ret ret = json_serialize(it->value.value, buf + i, bufLen, &written0);
                if (ret == SERIAL_OK) {
                    i += written0 - 1;
                    bufLen -= (int)written0;
                } else return ret;

                // not the last element
                if (e + 1 < n) {
                    if (i + 2 < (size_t)len) {
                        buf[bufLen--, ++i] = ',';
                        buf[bufLen--, ++i] = ' ';
                    } else return SERIAL_NOMEM;
                }
                bufLen--, ++i;
                e++;
            }

            if (i < (size_t)len) {
                buf[i++] = '}';
                if (written) *written = i;
            } else return SERIAL_NOMEM;
        }
        break;

        default: return SERIAL_UNKNOWN;
    }

    return SERIAL_OK;
}


json_deserialize_ret json_deserialize(json_context *j, char *buf, int len);

#define json_make_null json_make
#define json_make_null_term_string(j, str) json_make_string(j, str, common_strlen((str)))

#define json_object_del json_object_delete

json_deserialize_ret json_deserialize(json_context *j, char *buf, int len);
