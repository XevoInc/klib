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

#ifndef XASSERT_H_
#define XASSERT_H_

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
    char:                        "LHS: %c\n" \
                                 "RHS: %c", \
    signed char:                 "LHS: %hhd\n" \
                                 "RHS: %hhd", \
    unsigned char:               "LHS: %hhu\n" \
                                 "RHS: %hhu", \
    signed short:                "LHS: %hd\n" \
                                 "RHS: %hd", \
    unsigned short:              "LHS: %hu\n" \
                                 "RHS: %hu", \
    signed int:                  "LHS: %d\n" \
                                 "RHS: %d", \
    unsigned int:                "LHS: %u\n" \
                                 "RHS: %u", \
    long int:                    "LHS: %ld\n" \
                                 "RHS: %ld", \
    unsigned long int:           "LHS: %lu\n" \
                                 "RHS: %lu", \
    _Atomic long unsigned int *: "RHS: %lu" \
                                 "LHS: %lu", \
    long long int:               "LHS: %lld\n" \
                                 "RHS: %lld", \
    unsigned long long int:      "LHS: %llu\n" \
                                 "RHS: %llu", \
    float:                       "LHS: %f\n" \
                                 "RHS: %f", \
    double:                      "LHS: %f\n" \
                                 "RHS: %f", \
    long double:                 "LHS: %f\n" \
                                 "RHS: %f", \
    char *:                      "LHS: %s\n" \
                                 "RHS: %s", \
    void *:                      "LHS: %p\n" \
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
           _XASSERT_STR_BASE #expr _XASSERT_STR_LOC_DETAILS, \
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

#endif /* XASSERT_H_ */
