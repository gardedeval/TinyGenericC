#pragma once

typedef enum {
    EXPR,
    EXPR_PRIMARY,
    EXPR_UNARY_OP,
    EXPR_BINARY_OP,
} ast_type_t;

typedef enum {
    PLUS,
    MINUS,
    MULTIPLY,
    DIVIDE,
} ast_operator_t;

typedef enum {
    NUMBER
} ast_primary_t;

typedef struct {
    ast_type_t type;
} ast_t;