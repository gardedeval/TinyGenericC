#pragma once

#include <type.h>

#define ast_expr_extend_gen_(line) ast_expr_##line
#define ast_expr_extend_gen(line) ast_expr_extend_gen_(line)
#define ast_expr_extend() union ast_expr_extend_gen(__LINE__) { ast_type_t type; ast_t super; } base

typedef struct { ast_expr_extend(); } ast_expr_t;

typedef struct {
    ast_expr_extend();

    ast_primary_t prim;
    size_t len;
    void *mem;
} ast_expr_primary_t;

typedef struct {
    ast_expr_extend();

    ast_operator_t op;
    ast_expr_t *expr;
} ast_expr_unary_op_t;

typedef struct {
    ast_expr_extend();

    ast_expr_t *l;
    ast_operator_t op;
    ast_expr_t *r;
} ast_expr_binary_op_t;