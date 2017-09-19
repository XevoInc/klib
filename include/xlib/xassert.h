/**
 * @file      xassert.h
 * @brief     Generic, macro-based assertions that dump both sides of the
 *            expression being asserted.
 * @author    Martin Kelly <mkelly@xevo.com>
 * @copyright Copyright (C) 2017 Xevo Inc. All Rights Reserved.
 *
 * Note that the assertion code is adapted from the code in Ellis, originally
 * written by Martin Kelly and James Corey. The original file is here:
 *
 * https://github.com/project-ellis/ellis/blob/master/include/ellis/core/system.hpp
 *
 */

#ifndef XLIB_XASSERT_H_
#define XLIB_XASSERT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#define _XASSERT_STR_BASE "Assert: failed expression ("
#define _XASSERT_STR_LOC_DETAILS ") at %s:%d [%s]"

#define _XASSERT_STATIC_STRLEN(s) (sizeof(char)*sizeof(s) - 1)
#define _XASSERT_MAX_EXPR_STR_LEN (100)
#define _XASSERT_MAX_ARG_STR_LEN (1000)
#define _XASSERT_MAX_FORMAT_STR_LEN  ( \
    _XASSERT_STATIC_STRLEN(_XASSERT_STR_BASE) + \
    _XASSERT_MAX_EXPR_STR_LEN + \
    _XASSERT_STATIC_STRLEN(s_loc_str) + \
    _XASSERT_MAX_ARG_STR_LEN \
    )

#define _XASSERT_SKELETON(expr, log_code) \
    do { \
      if (expr) { \
        /* Empty, but catches accidental assignment (i.e. a=b) in expr. */ \
      } \
      else { \
        log_code; \
        abort(); \
      } \
    } while (0);

#define _XASSERT_FMT(expr, fmt, ...) \
    _XASSERT_SKELETON(expr, \
        _xassert_log_msg( \
            #expr, \
            __FILE__, \
            __LINE__, \
            __func__, \
            "LHS: " fmt "\nRHS: " fmt, \
            __VA_ARGS__));

#define _XASSERT_OP_FMT(op, fmt, x, y) _XASSERT_FMT(x op y, fmt, x, y)

#define XASSERT_LT_FMT(fmt, x, y) _XASSERT_OP_FMT(<, fmt, x, y)
#define XASSERT_LTE_FMT(fmt, x, y) _XASSERT_OP_FMT(<=, fmt, x, y)
#define XASSERT_EQ_FMT(fmt, x, y) _XASSERT_OP_FMT(==, fmt, x, y)
#define XASSERT_NEQ_FMT(fmt, x, y) _XASSERT_OP_FMT(!=, fmt, x, y)
#define XASSERT_GT_FMT(fmt, x, y) _XASSERT_OP_FMT(>, fmt, x, y)
#define XASSERT_GTE_FMT(fmt, x, y) _XASSERT_OP_FMT(>=, fmt, x, y)

#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 201112L)

#include <stdatomic.h>
#include <stdbool.h>

#define _XFMT(x) _Generic((x), \
    char:                                  "LHS: %c\n" \
                                           "RHS: %c", \
    _Atomic char:                          "LHS: %c\n" \
                                           "RHS: %c", \
    _Atomic char *:                        "LHS: %p\n" \
                                           "RHS: %p", \
    signed char:                           "LHS: %hhd\n" \
                                           "RHS: %hhd", \
    _Atomic signed char:                   "LHS: %hhd\n" \
                                           "RHS: %hhd", \
    _Atomic signed char *:                 "LHS: %p\n" \
                                           "RHS: %p", \
    unsigned char:                         "LHS: %hhu\n" \
                                           "RHS: %hhu", \
    _Atomic unsigned char:                 "LHS: %hhu\n" \
                                           "RHS: %hhu", \
    _Atomic unsigned char *:               "LHS: %p\n" \
                                           "RHS: %p", \
    signed short:                          "LHS: %hd\n" \
                                           "RHS: %hd", \
    _Atomic signed short:                  "LHS: %hd\n" \
                                           "RHS: %hd", \
    _Atomic signed short *:                "LHS: %p\n" \
                                           "RHS: %p", \
    unsigned short:                        "LHS: %hu\n" \
                                           "RHS: %hu", \
    _Atomic unsigned short:                "LHS: %hu\n" \
                                           "RHS: %hu", \
    _Atomic unsigned short *:              "LHS: %p\n" \
                                           "RHS: %p", \
    signed int:                            "LHS: %d\n" \
                                           "RHS: %d", \
    _Atomic signed int:                    "LHS: %d\n" \
                                           "RHS: %d", \
    _Atomic signed int *:                  "LHS: %p\n" \
                                           "RHS: %p", \
    unsigned int:                          "LHS: %u\n" \
                                           "RHS: %u", \
    _Atomic unsigned int:                  "LHS: %u\n" \
                                           "RHS: %u", \
    _Atomic unsigned int *:                "LHS: %p\n" \
                                           "RHS: %p", \
    long int:                              "LHS: %ld\n" \
                                           "RHS: %ld", \
    _Atomic long int:                      "LHS: %ld\n" \
                                           "RHS: %ld", \
    _Atomic long int *:                    "LHS: %p\n" \
                                           "RHS: %p", \
    unsigned long int:                     "LHS: %lu\n" \
                                           "RHS: %lu", \
    _Atomic unsigned long int:             "LHS: %lu\n" \
                                           "RHS: %lu", \
    _Atomic unsigned long int *:           "LHS: %p\n" \
                                           "RHS: %p", \
    long long int:                         "LHS: %lld\n" \
                                           "RHS: %lld", \
    _Atomic long long int:                 "LHS: %lld\n" \
                                           "RHS: %lld", \
    _Atomic long long int *:               "LHS: %p\n" \
                                           "RHS: %p", \
    unsigned long long int:                "LHS: %llu\n" \
                                           "RHS: %llu", \
    _Atomic unsigned long long int:        "LHS: %llu\n" \
                                           "RHS *: %llu", \
    _Atomic unsigned long long int *:      "LHS: %p\n" \
                                           "RHS: %p", \
    float:                                 "LHS: %f\n" \
                                           "RHS: %f", \
    _Atomic float:                         "LHS: %f\n" \
                                           "RHS: %f", \
    _Atomic float *:                       "LHS: %p\n" \
                                           "RHS: %p", \
    double:                                "LHS: %f\n" \
                                           "RHS: %f", \
    _Atomic double:                        "LHS: %f\n" \
                                           "RHS: %f", \
    _Atomic double *:                      "LHS: %p\n" \
                                           "RHS: %p", \
    long double:                           "LHS: %f\n" \
                                           "RHS: %f", \
    _Atomic long double:                   "LHS: %f\n" \
                                           "RHS: %f", \
    _Atomic long double *:                 "LHS: %p\n" \
                                           "RHS: %p", \
    void *:                                "LHS: %p\n" \
                                           "RHS: %p" \
    )

#define _XASSERT_GENERIC(expr, x, y) \
    _XASSERT_SKELETON(expr, \
        _xassert_log_msg(#expr, __FILE__, __LINE__, __func__, _XFMT(x), x, y));

#define _XASSERT_OP_GENERIC(op, x, y) _XASSERT_GENERIC(x op y, x, y)

#define XASSERT_LT(x, y) _XASSERT_OP_GENERIC(<, x, y)
#define XASSERT_LTE(x, y) _XASSERT_OP_GENERIC(<=, x, y)
#define XASSERT_EQ(x, y) _XASSERT_OP_GENERIC(==, x, y)
#define XASSERT_NEQ(x, y) _XASSERT_OP_GENERIC(!=, x, y)
#define XASSERT_GT(x, y) _XASSERT_OP_GENERIC(>, x, y)
#define XASSERT_GTE(x, y) _XASSERT_OP_GENERIC(>=, x, y)

#define XASSERT_NULL(x) XASSERT_EQ((void *) (x), NULL)
#define XASSERT_NOT_NULL(x) XASSERT_NEQ((void *) (x), NULL)

#define XASSERT_ERROR XASSERT(false)

#else

#define XASSERT_LT(fmt, x, y) XASSERT_LT_FMT(fmt, x, y)
#define XASSERT_LTE(fmt, x, y) XASSERT_LTE_FMT(fmt, x, y)
#define XASSERT_EQ(fmt, x, y) XASSERT_EQ_FMT(fmt, x, y)
#define XASSERT_NEQ(fmt, x, y) XASSERT_NEQ_FMT(fmt, x, y)
#define XASSERT_GT(fmt, x, y) XASSERT_GT_FMT(fmt, x, y)
#define XASSERT_GTE(fmt, x, y) XASSERT_GTE_FMT(fmt, x, y)

#define XASSERT_NULL(x) XASSERT_EQ(%p, x, NULL)
#define XASSERT_NOT_NULL(x) XASSERT_NEQ(%p, x, NULL)

#define XASSERT_ERROR XASSERT_TRUE(0)

#endif /* defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 201112L) */

#define XASSERT(expr) \
    _XASSERT_SKELETON(expr, \
        _xassert_log_msg( \
           #expr, \
           __FILE__, \
           __LINE__, \
           __func__, \
           ""));

#define XASSERT_DEFINE_ASSERTS(log_func) \
\
static const char s_loc_str[] = _XASSERT_STR_LOC_DETAILS "\n"; \
\
static inline __attribute__ ((__unused__)) \
void _xassert_log_msg( \
    const char *expr, \
    const char *file, \
    int line, \
    const char *func, \
    const char *fmt, \
    ...) \
{ \
    va_list args; \
    char msg[_XASSERT_MAX_FORMAT_STR_LEN]; \
    char *pos; \
    \
    /*
     * We check only the sprintfs for strings that are unknown at compile-time
     * because they have the potential to be variable length.
     */ \
    pos = msg; \
    pos += sprintf(pos, _XASSERT_STR_BASE); \
    \
    /*
     * Override the -Wformat-security warning here regarding untrusted input
     * because expr comes from a limited set of compile-time assert macros,
     * which are all safe.
     *
     * Note that we use _Pragma instead of #pragma because you can't put a #
     * directive inside a #define:
     *
     * https://gcc.gnu.org/onlinedocs/cpp/Pragmas.html
     */ \
    _Pragma("GCC diagnostic push") \
    _Pragma("GCC diagnostic ignored \"-Wformat-security\"") \
    pos += snprintf(pos, _XASSERT_MAX_EXPR_STR_LEN, expr); \
    _Pragma("GCC diagnostic pop") \
    \
    pos += sprintf(pos, s_loc_str, file, line, func); \
    \
    va_start(args, fmt); \
    _Pragma("GCC diagnostic push") \
    _Pragma("GCC diagnostic ignored \"-Wformat-security\"") \
    vsnprintf(pos, _XASSERT_MAX_ARG_STR_LEN, fmt, args); \
    _Pragma("GCC diagnostic pop") \
    va_end(args); \
    \
    log_func(msg); \
}

#define _XASSERT_ERRCODE(x, y, strerror_func) \
    _XASSERT_FMT(x == y, "%d (%s)", x, strerror_func(x), y, strerror_func(y))

#ifdef __cplusplus
}
#endif

#endif /* XLIB_XASSERT_H_ */