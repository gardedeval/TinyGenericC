#pragma once

#ifdef __cplusplus
#error "C++ is not supported in this library"
#endif

#ifdef _MSC_VER
#define MSVC
#elif defined(__clang__)
#define CLANG
#elif defined(__GNUC__)
#define GCC
#endif

#ifdef MSVC
#define PACKME_BEGIN __pragma( pack(push, 1) )
#define PACKME_END __pragma( pack(pop) )
#elif defined(CLANG) || defined(GCC)
#define PACKME_BEGIN 
#define PACKME_END __attribute__((packed))
#endif

#ifndef common_assert
#if defined(DEBUG) || defined(_DEBUG)
#include <assert.h>
#define common_assert assert
#else
#define common_assert(expr) ((void) 0)
#endif
#endif


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

#if defined(DEBUG) || defined(_DEBUG)
#define common_ensure_message(expr, msg) ( !(expr) ? common_fprintf(stderr, (msg "\n")), common_abort(0) : (void)0 )
#else
#define common_ensure_message(expr, msg) ( !(expr) ? common_abort() : (void)0 )
#endif

#define common_ensure(expr) common_ensure_message(expr, "Conditional violation, program terminated")
#define common_ensure_nomsg(expr) ( !(expr) ? common_abort() : (void) 0 )

#define common_ptr_rv_to_lv(ptr) *((void **) &(ptr))
#define ptr_rtol common_ptr_rv_to_lv

#endif
