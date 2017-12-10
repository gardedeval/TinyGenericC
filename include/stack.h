#pragma once

#include "vector.h"

#define STACK_VERSION "0.0.0"

#define stack vector
#define stk stack

#define stack_sizeof vector_sizeof
#define stk_sizeof stack_sizeof

#define stack_type_sizeof vector_type_sizeof
#define stk_type_sizeof stack_type_sizeof

#define stack_index vector_index
#define stk_idx stack_index

#define stack_capacity vector_capacity
#define stk_cap stack_capacity

#define stack_memory vector_memory
#define stk_mem stack_memory

#define stack_used_memory vector_used_memory
#define stk_used_mem stack_used_memory

#define stack_valid vector_valid
#define stk_valid stack_valid

#define stack_clear_memory vector_clear_memory
#define stk_clear_mem stack_clear_memory

#define stack_make vector_make
#define stk_make stack_make

#define stack_destroy vector_destroy
#define stk_destroy stack_destroy

#define stack_clone vector_clone
#define stk_clone stack_clone

#define stack_push vector_push
#define stk_push stack_push

#define stack_pop vector_pop
#define stk_pop stack_pop

#define stack_top vector_top
#define stk_top stack_top

#define stack_bottom vector_bottom
#define stk_bottom stack_bottom
