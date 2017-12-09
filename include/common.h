#pragma once

#ifdef __cplusplus
#error "C++ is not supported in this library"
#endif

#include "platform_check.h"
#include "platform_shim.h"
#include "syntax_hack.h"
#include "static_assert.h"

#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>

#ifndef common_memcpy
#define common_memcpy memcpy
#endif

#ifndef common_memset
#define common_memset memset
#endif

#ifndef common_memmove
#define common_memmove memmove
#endif

#ifndef common_memcmp
#define common_memcmp memcmp
#endif

#ifndef common_calloc
#define common_calloc calloc
#endif

#ifndef common_malloc
#define common_malloc malloc
#endif

#ifndef common_realloc
#define common_realloc realloc
#endif

#ifndef common_free
#define common_free free
#endif

#ifndef common_strcmp
#define common_strcmp strcmp
#endif

#ifndef common_strlen
#define common_strlen strlen
#endif

#ifndef common_strdup
#ifdef MSVC
#define common_strdup _strdup
#else
#define common_strdup strdup
#endif

#ifndef common_qsort
#define common_qsort qsort
#endif

#ifndef common_fprintf
#define common_fprintf fprintf
#endif

#ifndef common_exit
#define common_exit exit
#endif

#ifndef common_abort
#define common_abort abort
#endif

#if M_DEBUG_V == 1
#define common_ensure_message(expr, msg) ( !(expr) ? (common_fprintf(stderr, (msg "\n")), common_abort(), 0) : 1 )
#else
#define common_ensure_message(expr, msg) ( !(expr) ? common_abort(), ((int) 0) : (int)1 )
#endif

#define common_ensure(expr) common_ensure_message(expr, "Conditional violation, program terminated")
#define common_ensure_nomsg(expr) ( !(expr) ? common_abort() : (int) 1 )

#define common_ptr_rv_to_lv(ptr) *typecast(void **, &(ptr))

#ifndef ptr_rtol
#define ptr_rtol common_ptr_rv_to_lv
#endif

#ifndef type_assert_by_size
#define type_assert_by_size(a, b) STATIC_ASSERT((a) == (b), "underlying size unmatch")
#define type_assert_by_size_expr(a, b) STATIC_ASSERT_EXPR((a) == (b), "underlying size unmatch")
#endif

#endif
