#include <xlib/xassert.h>
#include <xlib/xlog.h>

void xlog_default_func(
    XlogPriority priority,
    bool print_loc,
    const char *file,
    int line,
    const char *func,
    const char *fmt,
    va_list args)
{
    FILE *dst;

    if (priority <= XLOG_WARNING) {
        dst = stderr;
    }
    else {
        dst = stdout;
    }

    vfprintf(dst, fmt, args);
    if (print_loc) {
        fprintf(dst, " at %s:%d [%s]", file, line, func);
    }
    fputc('\n', dst);
}

static XlogPriority s_priority = XLOG_WARNING;
static XlogFunc s_log_func = xlog_default_func;

void xlog_set_log_priority(XlogPriority priority)
{
    XASSERT_GTE(priority, XLOG_EMERG);
    XASSERT_LTE(priority, XLOG_DEBUG);

    s_priority = priority;
}

void xlog_set_log_func(XlogFunc func)
{
    s_log_func = func;
}

bool xlog_enabled(XlogPriority priority)
{
    return priority <= s_priority;
}

/*
 * We could reduce duplication by having all these functiosn call into
 * xlog_va, but that would mean we have to call va_start even if later
 * logging is disabled. Since we want absolutely minimal overhead when
 * logging is disabled, we have a small bit of redundancy. Sigh.
 */
void _xlog_va(
    XlogPriority priority,
    bool print_loc,
    const char *file,
    int line,
    const char *func,
    const char *fmt,
    va_list args)
{
    if (!xlog_enabled(priority)) {
        return;
    }

    s_log_func(priority, print_loc, file, line, func, fmt, args);

}

void _xlog(
    XlogPriority priority,
    bool print_loc,
    const char *file,
    int line,
    const char *func,
    const char *fmt,
    ...)
{
    va_list args;

    if (!xlog_enabled(priority)) {
        return;
    }

    va_start(args, fmt);
    s_log_func(priority, print_loc, file, line, func, fmt, args);
    va_end(args);
}
