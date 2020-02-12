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

#include <signal.h>
#include <xlib/xlog.h>

static __attribute__ ((__unused__))
void _xassert_log_base(
    bool print_loc,
    const char *expr,
    const char *file,
    int line,
    const char *func)
{
    _xlog(
        XLOG_CRIT,
        print_loc,
        file,
        line,
        func,
        "Assert: failed expression (%s)",
        expr);
}

static __attribute__ ((__unused__))
void _xassert_log_extra(
    const char *expr,
    const char *file,
    int line,
    const char *func,
    const char *extra)
{
    _xassert_log_base(true, expr, file, line, func);
    _xlog(XLOG_CRIT, false, file, line, func, extra);
}

static __attribute__ ((__unused__))
void _xassert_log_fmt(
    const char *expr,
    const char *file,
    int line,
    const char *func,
    const char *fmt,
    ...)
{
    va_list args;

    _xassert_log_base(true, expr, file, line, func);

    va_start(args, fmt);
    _xlog_va(XLOG_CRIT, false, file, line, func, fmt, args);
    va_end(args);
}

#ifdef __cplusplus
#include <sstream>

template <class X, class Y>
static __attribute__ ((__unused__))
void _xassert_log_formatted_msg_cpp(
    const X &x,
    const Y &y,
    const char *expr,
    const char *file,
    int line,
    const char *func)
{
    std::stringstream ss;

    ss << "LHS: " << x << "\nRHS: " << y;
    _xassert_log_extra(expr, file, line, func, ss.str().c_str());
}
#endif /* __cplusplus */

#include <float.h>
#include <math.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#ifdef __GNUC__
#define _XASSERT_LIKELY(x) __builtin_expect(!!(x), 1)
#define _XASSERT_UNLIKELY(x) __builtin_expect(!!(x), 0)
#else
#define _XASSERT_LIKELY(x) (x)
#define _XASSERT_UNLIKELY(x) (x)
#endif

#define _XASSERT_SKELETON(expr, log_code) \
    do { \
        int _res = expr; \
        if (_XASSERT_LIKELY(_res)) { \
            /* Empty, but catches accidental assignment (i.e. a=b) in expr. */ \
        } \
        else { \
            log_code; \
            kill(0, SIGABRT); \
            __builtin_unreachable(); \
        } \
    } while (0);

#define XASSERT(expr) \
    _XASSERT_SKELETON(expr, \
        _xassert_log_base( \
           true, \
           #expr, \
           __FILE__, \
           __LINE__, \
           __func__));

#ifndef __cplusplus
/*
 * C++ doesn't get XASSERT_FMT, which relies on prinf-style formatting and
 * generic macros.  This is because C++ can use templates, which do a better job
 * than generic macros anyway.
 */
#define _XASSERT_FMT(expr, fmt, ...) \
    _XASSERT_SKELETON(expr, \
        _xassert_log_fmt( \
            #expr, \
            __FILE__, \
            __LINE__, \
            __func__, \
            "LHS: " fmt "\nRHS: " fmt, \
            __VA_ARGS__));

#define _XASSERT_OP_FMT(op, fmt, x, y) _XASSERT_FMT((x) op (y), fmt, x, y)

#define XASSERT_LT_FMT(fmt, x, y) _XASSERT_OP_FMT(<, fmt, x, y)
#define XASSERT_LTE_FMT(fmt, x, y) _XASSERT_OP_FMT(<=, fmt, x, y)
#define XASSERT_EQ_FMT(fmt, x, y) _XASSERT_OP_FMT(==, fmt, x, y)
#define XASSERT_NEQ_FMT(fmt, x, y) _XASSERT_OP_FMT(!=, fmt, x, y)
#define XASSERT_GT_FMT(fmt, x, y) _XASSERT_OP_FMT(>, fmt, x, y)
#define XASSERT_GTE_FMT(fmt, x, y) _XASSERT_OP_FMT(>=, fmt, x, y)
#endif

/*
 * Here, we define XASSERT_* style macros if we have C11 Generic Macros.
 * Otherwise, users have to use either XASSERT or XASSERT_EQ_FMT style.
 */
#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 201112L)

#include <stdatomic.h>
#include <stdbool.h>

#define _XFMT(x) _Generic((x), \
    char:                                  "LHS: %c\n" \
                                           "RHS: %c", \
    const char:                            "LHS: %c\n" \
                                           "RHS: %c", \
    char *:                                "LHS: %c\n" \
                                           "RHS: %c", \
    const char *:                          "LHS: %c\n" \
                                           "RHS: %c", \
    _Atomic char:                          "LHS: %c\n" \
                                           "RHS: %c", \
    _Atomic char *:                        "LHS: %p\n" \
                                           "RHS: %p", \
    signed char:                           "LHS: %hhd\n" \
                                           "RHS: %hhd", \
    const signed char:                     "LHS: %hhd\n" \
                                           "RHS: %hhd", \
    signed char *:                         "LHS: %hhd\n" \
                                           "RHS: %hhd", \
    const signed char *:                   "LHS: %hhd\n" \
                                           "RHS: %hhd", \
    _Atomic signed char:                   "LHS: %hhd\n" \
                                           "RHS: %hhd", \
    _Atomic signed char *:                 "LHS: %p\n" \
                                           "RHS: %p", \
    unsigned char:                         "LHS: %hhu\n" \
                                           "RHS: %hhu", \
    const unsigned char:                   "LHS: %hhu\n" \
                                           "RHS: %hhu", \
    unsigned char *:                       "LHS: %hhu\n" \
                                           "RHS: %hhu", \
    const unsigned char *:                 "LHS: %hhu\n" \
                                           "RHS: %hhu", \
    _Atomic unsigned char:                 "LHS: %hhu\n" \
                                           "RHS: %hhu", \
    _Atomic unsigned char *:               "LHS: %p\n" \
                                           "RHS: %p", \
    signed short:                          "LHS: %hd\n" \
                                           "RHS: %hd", \
    const signed short:                    "LHS: %hd\n" \
                                           "RHS: %hd", \
    signed short *:                        "LHS: %hd\n" \
                                           "RHS: %hd", \
    const signed short *:                  "LHS: %hd\n" \
                                           "RHS: %hd", \
    _Atomic signed short:                  "LHS: %hd\n" \
                                           "RHS: %hd", \
    _Atomic signed short *:                "LHS: %p\n" \
                                           "RHS: %p", \
    unsigned short:                        "LHS: %hu\n" \
                                           "RHS: %hu", \
    const unsigned short:                  "LHS: %hu\n" \
                                           "RHS: %hu", \
    unsigned short *:                      "LHS: %hu\n" \
                                           "RHS: %hu", \
    const unsigned short *:                "LHS: %hu\n" \
                                           "RHS: %hu", \
    _Atomic unsigned short:                "LHS: %hu\n" \
                                           "RHS: %hu", \
    _Atomic unsigned short *:              "LHS: %p\n" \
                                           "RHS: %p", \
    signed int:                            "LHS: %d\n" \
                                           "RHS: %d", \
    const signed int:                      "LHS: %d\n" \
                                           "RHS: %d", \
    signed int *:                          "LHS: %d\n" \
                                           "RHS: %d", \
    const signed int *:                    "LHS: %d\n" \
                                           "RHS: %d", \
    _Atomic signed int:                    "LHS: %d\n" \
                                           "RHS: %d", \
    _Atomic signed int *:                  "LHS: %p\n" \
                                           "RHS: %p", \
    unsigned int:                          "LHS: %u\n" \
                                           "RHS: %u", \
    const unsigned int:                    "LHS: %u\n" \
                                           "RHS: %u", \
    unsigned int *:                        "LHS: %u\n" \
                                           "RHS: %u", \
    const unsigned int *:                  "LHS: %u\n" \
                                           "RHS: %u", \
    _Atomic unsigned int:                  "LHS: %u\n" \
                                           "RHS: %u", \
    _Atomic unsigned int *:                "LHS: %p\n" \
                                           "RHS: %p", \
    long int:                              "LHS: %ld\n" \
                                           "RHS: %ld", \
    const long int:                        "LHS: %ld\n" \
                                           "RHS: %ld", \
    long int *:                            "LHS: %ld\n" \
                                           "RHS: %ld", \
    const long int *:                      "LHS: %ld\n" \
                                           "RHS: %ld", \
    _Atomic long int:                      "LHS: %ld\n" \
                                           "RHS: %ld", \
    _Atomic long int *:                    "LHS: %p\n" \
                                           "RHS: %p", \
    unsigned long int:                     "LHS: %lu\n" \
                                           "RHS: %lu", \
    const unsigned long int:               "LHS: %lu\n" \
                                           "RHS: %lu", \
    unsigned long int *:                   "LHS: %lu\n" \
                                           "RHS: %lu", \
    const unsigned long int *:             "LHS: %lu\n" \
                                           "RHS: %lu", \
    _Atomic unsigned long int:             "LHS: %lu\n" \
                                           "RHS: %lu", \
    _Atomic unsigned long int *:           "LHS: %p\n" \
                                           "RHS: %p", \
    long long int:                         "LHS: %lld\n" \
                                           "RHS: %lld", \
    const long long int:                   "LHS: %lld\n" \
                                           "RHS: %lld", \
    long long int *:                       "LHS: %lld\n" \
                                           "RHS: %lld", \
    const long long int *:                 "LHS: %lld\n" \
                                           "RHS: %lld", \
    _Atomic long long int:                 "LHS: %lld\n" \
                                           "RHS: %lld", \
    _Atomic long long int *:               "LHS: %p\n" \
                                           "RHS: %p", \
    unsigned long long int:                "LHS: %llu\n" \
                                           "RHS: %llu", \
    const unsigned long long int:          "LHS: %llu\n" \
                                           "RHS: %llu", \
    unsigned long long int *:              "LHS: %llu\n" \
                                           "RHS: %llu", \
    const unsigned long long int *:        "LHS: %llu\n" \
                                           "RHS: %llu", \
    _Atomic unsigned long long int:        "LHS: %llu\n" \
                                           "RHS: %llu", \
    _Atomic unsigned long long int *:      "LHS: %p\n" \
                                           "RHS: %p", \
    float:                                 "LHS: %f\n" \
                                           "RHS: %f", \
    const float:                           "LHS: %f\n" \
                                           "RHS: %f", \
    float *:                               "LHS: %f\n" \
                                           "RHS: %f", \
    const float *:                         "LHS: %f\n" \
                                           "RHS: %f", \
    _Atomic float:                         "LHS: %f\n" \
                                           "RHS: %f", \
    _Atomic float *:                       "LHS: %p\n" \
                                           "RHS: %p", \
    double:                                "LHS: %f\n" \
                                           "RHS: %f", \
    const double:                          "LHS: %f\n" \
                                           "RHS: %f", \
    double *:                              "LHS: %f\n" \
                                           "RHS: %f", \
    const double *:                        "LHS: %f\n" \
                                           "RHS: %f", \
    _Atomic double:                        "LHS: %f\n" \
                                           "RHS: %f", \
    _Atomic double *:                      "LHS: %p\n" \
                                           "RHS: %p", \
    long double:                           "LHS: %f\n" \
                                           "RHS: %f", \
    const long double:                     "LHS: %f\n" \
                                           "RHS: %f", \
    long double *:                         "LHS: %f\n" \
                                           "RHS: %f", \
    const long double *:                   "LHS: %f\n" \
                                           "RHS: %f", \
    _Atomic long double:                   "LHS: %f\n" \
                                           "RHS: %f", \
    _Atomic long double *:                 "LHS: %p\n" \
                                           "RHS: %p", \
    const void *:                          "LHS: %p\n" \
                                           "RHS: %p", \
    void *:                                "LHS: %p\n" \
                                           "RHS: %p" \
    )

#define _XASSERT_GENERIC(expr, x, y) \
    _XASSERT_SKELETON(expr, \
        _xassert_log_fmt(#expr, __FILE__, __LINE__, __func__, _XFMT(x), x, y));

#define _XASSERT_OP_GENERIC(op, x, y) _XASSERT_GENERIC((x) op (y), x, y)

#define XASSERT_LT(x, y) _XASSERT_OP_GENERIC(<, x, y)
#define XASSERT_LTE(x, y) _XASSERT_OP_GENERIC(<=, x, y)
#define XASSERT_EQ(x, y) _XASSERT_OP_GENERIC(==, x, y)
#define XASSERT_NEQ(x, y) _XASSERT_OP_GENERIC(!=, x, y)
#define XASSERT_GT(x, y) _XASSERT_OP_GENERIC(>, x, y)
#define XASSERT_GTE(x, y) _XASSERT_OP_GENERIC(>=, x, y)

#endif /* defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 201112L) */

#ifdef __cplusplus
/* For C++, we implement these using a template function. */
#define _XASSERT_CXX(expr, x, y) _XASSERT_SKELETON(expr, \
    _xassert_log_formatted_msg_cpp(\
        x, \
        y, \
        #expr, \
        __FILE__, \
        __LINE__, \
        __func__))

#define _XASSERT_OP_CXX(op, x, y) _XASSERT_CXX((x) op (y), x, y)

#define XASSERT_LT(x, y) _XASSERT_OP_CXX(<, x, y)
#define XASSERT_LTE(x, y) _XASSERT_OP_CXX(<=, x, y)
#define XASSERT_EQ(x, y) _XASSERT_OP_CXX(==, x, y)
#define XASSERT_NEQ(x, y) _XASSERT_OP_CXX(!=, x, y)
#define XASSERT_GT(x, y) _XASSERT_OP_CXX(>, x, y)
#define XASSERT_GTE(x, y) _XASSERT_OP_CXX(>=, x, y)

/* Needed for std::nullptr_t. */
#include <cstddef>
#define XASSERT_NULL(x) XASSERT((x) == nullptr)
#define XASSERT_NOT_NULL(x) XASSERT((x) != nullptr)

#else /* C, not C++ */

#define XASSERT_NULL(x) XASSERT((x) == NULL)
#define XASSERT_NOT_NULL(x) XASSERT((x) != NULL)

#endif /* __cplusplus */

#define XASSERT_FALSE(expr) XASSERT(!(expr))

#define XASSERT_ERROR \
    do { \
    XASSERT(0); \
    __builtin_unreachable(); \
    } while (0);

#define XASSERT_FLTEQ_THRESH(x, y, thresh) \
    _XASSERT_FMT(fabsf((x) - (y)) < (thresh), "%f", x, y)
#define XASSERT_DBLEQ_THRESH(x, y, thresh) \
    _XASSERT_FMT(fabs((x) - (y)) < (thresh), "%f", x, y)
#define XASSERT_LDBLEQ_THRESH(x, y, thresh) \
    _XASSERT_FMT(fabsl((x) - (y)) < (thresh), "%f", x, y)

#define XASSERT_FLTEQ(x, y) XASSERT_FLTEQ_THRESH(x, y, DBL_EPSILON)
#define XASSERT_DBLEQ(x, y) XASSERT_DBLEQ_THRESH(x, y, DBL_EPSILON)
#define XASSERT_LDBLEQ(x, y) XASSERT_LDBLEQ_THRESH(x, y, DBL_EPSILON)

#define XASSERT_STREQ(s, t) _XASSERT_FMT(strcmp(s, t) == 0, "%s", s, t)

#define _XASSERT_ERRCODE(x, y, strerror_func) \
    _XASSERT_FMT((x) == (y), "%d (%s)", x, strerror_func(x), y, strerror_func(y))

#endif /* XLIB_XASSERT_H_ */
