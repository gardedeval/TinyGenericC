#pragma once

#include "platform_check.h"

#ifdef MSVC
#define PACKME_BEGIN __pragma( pack(push, 1) )
#define PACKME_END __pragma( pack(pop) )
#elif defined(CLANG) || defined(GCC)
#define PACKME_BEGIN 
#define PACKME_END __attribute__((packed))
#else
#error "Unsupported compiler"
#endif

#ifndef common_assert
#if M_DEBUG_V == 1
#include <assert.h>
#define common_assert assert
#else
#define common_assert(expr) ((void) 0)
#endif
#endif

// reference: https://github.com/cxong/tinydir/blob/master/tinydir.h#L118
#ifdef MSVC // supports __inline and __forceinline
#define INLINE static __inline
#elif !defined(C99) // could be older GCC compilers
#define INLINE static __inline__
#else // pray for inline support then
#define INLINE static inline
#endif

/* https://graphics.stanford.edu/~seander/bithacks.html#RoundUpPowerOf2 */
INLINE
size_t size_expand_to_2n(size_t n) {
    n--;
    n |= n >> 1; // size_t = 0.25, 2-bit
    n |= n >> 2; // size_t = 0.5, 4-bit
    n |= n >> 4; // size_t = 1, 8-bit
    n |= n >> 8; // size_t = 2, 16-bit
    n |= n >> 16; // size_t = 4, 32-bit
                  // n |= n >> 32; // size_t = 8, 64-bit
    n |= n >> 4 * sizeof(size_t); // if 32-bit then 4*4 = 16 same as above, otherwise if 64-bit then 4*8 = 32

    /*
    // not loop unrolled? why?
    for (size_t i = 1; i < sizeof(size_t) * CHAR_BIT; i *= 2) {
    n |= n >> i;
    }
    */
    return ++n;
}