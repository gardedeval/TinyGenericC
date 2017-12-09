#pragma once

// copied from https://stackoverflow.com/a/2115886
#if defined(__STDC__) 
#define C89
#if defined(__STDC_VERSION__)
#define C90
#if (__STDC_VERSION__ >= 199409L)
#define C94
#endif
#if (__STDC_VERSION__ >= 199901L)
#define C99
#endif
#endif
#endif

#if defined(_MSC_VER) && !defined(MSVC)
#define MSVC
#define C89 // MSVC have C89 from the beginning
#if _MSC_VER >= 1800 && !defined(C99) // >= VS2013
// CAVEAT: MSVC is still being incompliant to C99
#define C99 // but let's assume we have it since nobody is going to use it in production
#endif
#elif defined(__clang__) && !defined(CLANG)
#define CLANG
#elif defined(__GNUC__) && !defined(GCC)
#define GCC
#endif

#ifndef M_DEBUG_V
#define M_DEBUG_V 0
#endif

#ifndef M_RELEASE_V
#define M_RELEASE_V 0
#endif

#if defined(DEBUG) || defined(_DEBUG)
#undef M_DEBUG_V
#define M_DEBUG_V 1
#endif

#if defined(RELEASE) || defined(NDEBUG) || defined(_NDEBUG)
#undef M_RELEASE_V
#define M_RELEASE_V 1
#endif

#if M_DEBUG_V == M_RELEASE_V
#warning "Both debug and release mode exists, but they should be dichotomic"
#warning "Assuming debug mode"
#undef M_DEBUG_V
#undef M_RELEASE_V
#define M_DEBUG_V 1
#define M_RELEASE_V 0
#endif

