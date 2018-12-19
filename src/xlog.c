#include <xlib/xassert.h>
#include <xlib/xlog.h>

void xlog_default_func(XlogPriority priority, const char *fmt, va_list args)
{
    FILE *dst;

    if (priority <= XLOG_WARNING) {
        dst = stderr;
    }
    else {
        dst = stdout;
    }

    vfprintf(dst, fmt, args);
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
void xlog_va(XlogPriority priority, const char *fmt, va_list args)
{
    if (!xlog_enabled(priority)) {
        return;
    }

    s_log_func(priority, fmt, args);
}

void xlog(XlogPriority priority, const char *fmt, ...)
{
    va_list args;

    if (!xlog_enabled(priority)) {
        return;
    }

    va_start(args, fmt);
    s_log_func(priority, fmt, args);
    va_end(args);
}

static
void _xlog_nofmt(XlogPriority priority, ...)
{
    va_list args;

    va_start(args, priority);
    s_log_func(priority, "%s", args);
    va_end(args);
}

void xlog_nofmt(XlogPriority priority, const char *msg)
{
    if (!xlog_enabled(priority)) {
        return;
    }

    _xlog_nofmt(priority, msg);
}
