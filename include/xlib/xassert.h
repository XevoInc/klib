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
#include <string.h>

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

#ifdef __GNUC__
#define _XASSERT_LIKELY(x) __builtin_expect(!!(x), 1)
#define _XASSERT_UNLIKELY(x) __builtin_expect(!!(x), 0)
#else
#define _XASSERT_LIKELY(x) (x)
#define _XASSERT_UNLIKELY(x) (x)
#endif

#define _XASSERT_SKELETON(expr, log_code) \
    do { \
        int _res = (expr); \
        if (_XASSERT_LIKELY(_res)) { \
            /* Empty, but catches accidental assignment (i.e. a=b) in expr. */ \
        } \
        else { \
            log_code; \
            abort(); \
            __builtin_unreachable(); \
        } \
    } while (0);

#define _XASSERT_FMT(expr, fmt, ...) \
    _XASSERT_SKELETON(expr, \
        _xassert_log_msg( \
            #expr, \
            __FILE__, \
            __LINE__, \
            __func__, \
            "LHS: " fmt "\nRHS: " fmt "\n", \
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
                                           "RHS: %c\n", \
    const char:                            "LHS: %c\n" \
                                           "RHS: %c\n", \
    char *:                                "LHS: %c\n" \
                                           "RHS: %c\n", \
    const char *:                          "LHS: %c\n" \
                                           "RHS: %c\n", \
    _Atomic char:                          "LHS: %c\n" \
                                           "RHS: %c\n", \
    _Atomic char *:                        "LHS: %p\n" \
                                           "RHS: %p\n", \
    signed char:                           "LHS: %hhd\n" \
                                           "RHS: %hhd\n", \
    const signed char:                     "LHS: %hhd\n" \
                                           "RHS: %hhd\n", \
    signed char *:                         "LHS: %hhd\n" \
                                           "RHS: %hhd\n", \
    const signed char *:                   "LHS: %hhd\n" \
                                           "RHS: %hhd\n", \
    _Atomic signed char:                   "LHS: %hhd\n" \
                                           "RHS: %hhd\n", \
    _Atomic signed char *:                 "LHS: %p\n" \
                                           "RHS: %p\n", \
    unsigned char:                         "LHS: %hhu\n" \
                                           "RHS: %hhu\n", \
    const unsigned char:                   "LHS: %hhu\n" \
                                           "RHS: %hhu\n", \
    unsigned char *:                       "LHS: %hhu\n" \
                                           "RHS: %hhu\n", \
    const unsigned char *:                 "LHS: %hhu\n" \
                                           "RHS: %hhu\n", \
    _Atomic unsigned char:                 "LHS: %hhu\n" \
                                           "RHS: %hhu\n", \
    _Atomic unsigned char *:               "LHS: %p\n" \
                                           "RHS: %p\n", \
    signed short:                          "LHS: %hd\n" \
                                           "RHS: %hd\n", \
    const signed short:                    "LHS: %hd\n" \
                                           "RHS: %hd\n", \
    signed short *:                        "LHS: %hd\n" \
                                           "RHS: %hd\n", \
    const signed short *:                  "LHS: %hd\n" \
                                           "RHS: %hd\n", \
    _Atomic signed short:                  "LHS: %hd\n" \
                                           "RHS: %hd\n", \
    _Atomic signed short *:                "LHS: %p\n" \
                                           "RHS: %p\n", \
    unsigned short:                        "LHS: %hu\n" \
                                           "RHS: %hu\n", \
    const unsigned short:                  "LHS: %hu\n" \
                                           "RHS: %hu\n", \
    unsigned short *:                      "LHS: %hu\n" \
                                           "RHS: %hu\n", \
    const unsigned short *:                "LHS: %hu\n" \
                                           "RHS: %hu\n", \
    _Atomic unsigned short:                "LHS: %hu\n" \
                                           "RHS: %hu\n", \
    _Atomic unsigned short *:              "LHS: %p\n" \
                                           "RHS: %p\n", \
    signed int:                            "LHS: %d\n" \
                                           "RHS: %d\n", \
    const signed int:                      "LHS: %d\n" \
                                           "RHS: %d\n", \
    signed int *:                          "LHS: %d\n" \
                                           "RHS: %d\n", \
    const signed int *:                    "LHS: %d\n" \
                                           "RHS: %d\n", \
    _Atomic signed int:                    "LHS: %d\n" \
                                           "RHS: %d\n", \
    _Atomic signed int *:                  "LHS: %p\n" \
                                           "RHS: %p\n", \
    unsigned int:                          "LHS: %u\n" \
                                           "RHS: %u\n", \
    const unsigned int:                    "LHS: %u\n" \
                                           "RHS: %u\n", \
    unsigned int *:                        "LHS: %u\n" \
                                           "RHS: %u\n", \
    const unsigned int *:                  "LHS: %u\n" \
                                           "RHS: %u\n", \
    _Atomic unsigned int:                  "LHS: %u\n" \
                                           "RHS: %u\n", \
    _Atomic unsigned int *:                "LHS: %p\n" \
                                           "RHS: %p\n", \
    long int:                              "LHS: %ld\n" \
                                           "RHS: %ld\n", \
    const long int:                        "LHS: %ld\n" \
                                           "RHS: %ld\n", \
    long int *:                            "LHS: %ld\n" \
                                           "RHS: %ld\n", \
    const long int *:                      "LHS: %ld\n" \
                                           "RHS: %ld\n", \
    _Atomic long int:                      "LHS: %ld\n" \
                                           "RHS: %ld\n", \
    _Atomic long int *:                    "LHS: %p\n" \
                                           "RHS: %p\n", \
    unsigned long int:                     "LHS: %lu\n" \
                                           "RHS: %lu\n", \
    const unsigned long int:               "LHS: %lu\n" \
                                           "RHS: %lu\n", \
    unsigned long int *:                   "LHS: %lu\n" \
                                           "RHS: %lu\n", \
    const unsigned long int *:             "LHS: %lu\n" \
                                           "RHS: %lu\n", \
    _Atomic unsigned long int:             "LHS: %lu\n" \
                                           "RHS: %lu\n", \
    _Atomic unsigned long int *:           "LHS: %p\n" \
                                           "RHS: %p\n", \
    long long int:                         "LHS: %lld\n" \
                                           "RHS: %lld\n", \
    const long long int:                   "LHS: %lld\n" \
                                           "RHS: %lld\n", \
    long long int *:                       "LHS: %lld\n" \
                                           "RHS: %lld\n", \
    const long long int *:                 "LHS: %lld\n" \
                                           "RHS: %lld\n", \
    _Atomic long long int:                 "LHS: %lld\n" \
                                           "RHS: %lld\n", \
    _Atomic long long int *:               "LHS: %p\n" \
                                           "RHS: %p\n", \
    unsigned long long int:                "LHS: %llu\n" \
                                           "RHS: %llu\n", \
    const unsigned long long int:          "LHS: %llu\n" \
                                           "RHS: %llu\n", \
    unsigned long long int *:              "LHS: %llu\n" \
                                           "RHS: %llu\n", \
    const unsigned long long int *:        "LHS: %llu\n" \
                                           "RHS: %llu\n", \
    _Atomic unsigned long long int:        "LHS: %llu\n" \
                                           "RHS: %llu\n", \
    _Atomic unsigned long long int *:      "LHS: %p\n" \
                                           "RHS: %p\n", \
    float:                                 "LHS: %f\n" \
                                           "RHS: %f\n", \
    const float:                           "LHS: %f\n" \
                                           "RHS: %f\n", \
    float *:                               "LHS: %f\n" \
                                           "RHS: %f\n", \
    const float *:                         "LHS: %f\n" \
                                           "RHS: %f\n", \
    _Atomic float:                         "LHS: %f\n" \
                                           "RHS: %f\n", \
    _Atomic float *:                       "LHS: %p\n" \
                                           "RHS: %p\n", \
    double:                                "LHS: %f\n" \
                                           "RHS: %f\n", \
    const double:                          "LHS: %f\n" \
                                           "RHS: %f\n", \
    double *:                              "LHS: %f\n" \
                                           "RHS: %f\n", \
    const double *:                        "LHS: %f\n" \
                                           "RHS: %f\n", \
    _Atomic double:                        "LHS: %f\n" \
                                           "RHS: %f\n", \
    _Atomic double *:                      "LHS: %p\n" \
                                           "RHS: %p\n", \
    long double:                           "LHS: %f\n" \
                                           "RHS: %f\n", \
    const long double:                     "LHS: %f\n" \
                                           "RHS: %f\n", \
    long double *:                         "LHS: %f\n" \
                                           "RHS: %f\n", \
    const long double *:                   "LHS: %f\n" \
                                           "RHS: %f\n", \
    _Atomic long double:                   "LHS: %f\n" \
                                           "RHS: %f\n", \
    _Atomic long double *:                 "LHS: %p\n" \
                                           "RHS: %p\n", \
    void *:                                "LHS: %p\n" \
                                           "RHS: %p\n" \
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

#define XASSERT_NULL(x) XASSERT((x) == NULL)
#define XASSERT_NOT_NULL(x) XASSERT((x) != NULL)

#else

#define XASSERT_LT(fmt, x, y) XASSERT_LT_FMT(fmt, x, y)
#define XASSERT_LTE(fmt, x, y) XASSERT_LTE_FMT(fmt, x, y)
#define XASSERT_EQ(fmt, x, y) XASSERT_EQ_FMT(fmt, x, y)
#define XASSERT_NEQ(fmt, x, y) XASSERT_NEQ_FMT(fmt, x, y)
#define XASSERT_GT(fmt, x, y) XASSERT_GT_FMT(fmt, x, y)
#define XASSERT_GTE(fmt, x, y) XASSERT_GTE_FMT(fmt, x, y)

#define XASSERT_NULL(x) XASSERT_EQ(%p, x, NULL)
#define XASSERT_NOT_NULL(x) XASSERT_NEQ(%p, x, NULL)

#endif /* defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 201112L) */

#define XASSERT_STREQ(s, t) _XASSERT_FMT(strcmp(s, t) == 0, "%s", s, t)

#define XASSERT_ERROR \
    do { \
    XASSERT(0); \
    __builtin_unreachable(); \
    } while (0);

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
