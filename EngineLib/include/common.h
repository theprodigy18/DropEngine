#pragma once

#include <stdlib.h>
#include <stdbool.h>
#ifdef DEBUG
#include <stdio.h>
#include <stdarg.h>
#include "log.h"
#endif // DEBUG

#ifdef __cplusplus
#define EXTERN_C_BEGIN \
    extern "C"         \
    {
#define EXTERN_C_END }
#else
#define EXTERN_C_BEGIN
#define EXTERN_C_END
#endif // __cplusplus

typedef unsigned char      u8;
typedef unsigned short     u16;
typedef unsigned int       u32;
typedef unsigned long long u64;

typedef signed char      i8;
typedef signed short     i16;
typedef signed int       i32;
typedef signed long long i64;

typedef float  f32;
typedef double f64;

#define BIT(x) (1 << x)

#define ALLOC_SINGLE(T) (T*) malloc(sizeof(T))
#define ALLOC_ARRAY(T, count) (T*) malloc(count * sizeof(T))
#define ZERO_MEM_SINGLE(ptr) memset(ptr, 0, sizeof(*ptr))
#define ZERO_MEM_ARRAY(ptr, count) memset(ptr, 0, count * sizeof(*ptr))
#define ALLOC_SINGLE_ZEROED(T) (T*) calloc(1, sizeof(T))
#define ALLOC_ARRAY_ZEROED(T, count) (T*) calloc(count, sizeof(T))
#define FREE(ptr) free(ptr)

#ifdef DEBUG
#ifdef _MSC_VER
#define DBG_BREAK() __debugbreak()
#elif defined(__clang__) || defined(__GNUC__)
#define DBG_BREAK() __builtin_trap()
#else
#define DBG_BERAK() (*(volatile int*) 0 = 0)
#endif // _MSVC_VER

#define LOG_TRACE(...) _Log("TRACE: ", __FILE__, __LINE__, TEXT_COLOR_GREEN, __VA_ARGS__) // Log Trace.
#define LOG_WARN(...) _Log("WARN:  ", __FILE__, __LINE__, TEXT_COLOR_YELLOW, __VA_ARGS__) // Log Warning.
#define LOG_ERROR(...) _Log("ERROR: ", __FILE__, __LINE__, TEXT_COLOR_RED, __VA_ARGS__)   // Log Error.

#define ASSERT(x) \
    if (!(x)) DBG_BREAK()
#define ASSERT_MSG(x, ...)      \
    if (!(x))                   \
    {                           \
        LOG_ERROR(__VA_ARGS__); \
        DBG_BREAK();            \
    }

#define DEBUG_OP(x) x

#else
#define DBG_BREAK()

#define LOG_TRACE(...)
#define LOG_WARN(...)
#define LOG_ERROR(...)

#define ASSERT(x)
#define ASSERT_MSG(x, ...)

#define DEBUG_OP(x)
#endif // D_DEBUG
