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

json_context *json_make(json_context *j);
#define json_make_null json_make
json_context *json_make_boolean(json_context *j, uint8_t val);
json_context *json_make_number(json_context *j, double_t val);
json_context *json_make_string(json_context *j, char *str, size_t len);
#define json_make_null_term_string(j, str) json_make_string(j, str, common_strlen((str)))
json_context *json_make_array(json_context *j);
json_context *json_make_object(json_context *j);

void json_array_push(json_context *arr, json_context *j);
json_context *json_array_get(json_context *arr, size_t n);
json_context *json_array_pop(json_context *arr);

void json_object_put(json_context *obj, const char *key, json_context *j);
json_context *json_object_get(json_context *obj, const char *key);
void json_object_delete(json_context *obj, const char *key);
#define json_object_del json_object_delete

json_serialize_ret json_serialize(json_context *j, char *buf, int len, size_t *written);
json_deserialize_ret json_deserialize(json_context *j, char *buf, int len);


void json_destroy(json_context *j);