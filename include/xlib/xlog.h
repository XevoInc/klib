/*
 * @file      xlog.h
 * @brief     Generic logging and assertion functionality.
 * @author    Martin Kelly <mkelly@xevo.com>
 * @copyright Copyright (C) 2017 Xevo Inc. All Rights Reserved.
 *
 * Note that the assertion code is adapted from the code in Ellis, originally
 * written by Martin Kelly and James Corey. The original file is here:
 *
 * https://github.com/project-ellis/ellis/blob/master/include/ellis/core/system.hpp
 *
 */

#ifndef XLIB_XLOG_H_
#define XLIB_XLOG_H_

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>

/* Forward declarations of logging functionality needed for assertions. */
typedef enum {
    XLOG_EMERG = 0,
    XLOG_ALERT = 1,
    XLOG_CRIT = 2,
    XLOG_ERR = 3,
    XLOG_WARNING = 4,
    XLOG_NOTICE = 5,
    XLOG_INFO = 6,
    XLOG_DEBUG = 7
} XlogPriority;

/*
 * If we have syslog, let's enforce that our log levels match those of syslog.
 */
#if defined(_POSIX_C_SOURCE) && (_POSIX_C_SOURCE >= 200112L)
#include <syslog.h>

#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 201112L)
#include <assert.h>
#define _XLIB_SYSLOG_MSG "xlib log levels don't match those of syslog!"
static_assert(XLOG_EMERG == LOG_EMERG, _XLIB_SYSLOG_MSG);
static_assert(XLOG_ALERT == LOG_ALERT, _XLIB_SYSLOG_MSG);
static_assert(XLOG_CRIT == LOG_CRIT, _XLIB_SYSLOG_MSG);
static_assert(XLOG_ERR == LOG_ERR, _XLIB_SYSLOG_MSG);
static_assert(XLOG_WARNING == LOG_WARNING, _XLIB_SYSLOG_MSG);
static_assert(XLOG_NOTICE == LOG_NOTICE, _XLIB_SYSLOG_MSG);
static_assert(XLOG_INFO == LOG_INFO, _XLIB_SYSLOG_MSG);
static_assert(XLOG_DEBUG == LOG_DEBUG, _XLIB_SYSLOG_MSG);
#undef _XLIB_SYSLOG_MSG
#endif

#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*XlogFunc)(XlogPriority priority, const char *fmt, va_list args);

/**
 * Set the global log priority.
 *
 * @param priority a log priority
 */
void xlog_set_log_priority(XlogPriority priority);

/**
 * Set the global log function.
 *
 * @param a log function
 */
void xlog_set_log_func(XlogFunc func);

/**
 * Check if logging is enabled. Useful for skipping code that builds up logging
 * information and thus incurs non-negligible overhead.
 *
 * @param priority a log priority
 */
bool xlog_enabled(XlogPriority priority);

/**
 * Log a message. If the globally-configured priority is lower than the priority
 * given, nothing will be logged.
 *
 * @param priority a log priority
 * @param fmt a printf-style formatting message
 * @param ... additional formatting arguments
 */
void xlog(XlogPriority priority, const char *fmt, ...);

/**
 * Log a message. This is the same as xlog but takes a va_list.
 *
 * @param priority a log priority
 * @param fmt a printf-style formatting message
 * @param args a var-args va_list
 */
void xlog_va(XlogPriority priority, const char *fmt, va_list args);

/**
 * Log a message without formatting. This is a convenience routine equivalent to
 * xlog(priority, "%s", msg).
 *
 * @param priority a log priority
 * @param msg a message that does not need formatting
 */
void xlog_nofmt(XlogPriority priority, const char *msg);

#ifdef __cplusplus
}
#endif

#endif /* XLIB_XLOG_H_ */
