/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_LOG_
#define _H_COM_DIAG_DIMINUTO_LOG_

/**
 * @file
 *
 * Copyright 2009-2010 Digital Aggregates Corporation, Arvada CO 80001-0587 USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 *
 * This header file can legitimately be included more than once in a
 * single translation unit.
 */

#if defined(__KERNEL__)

#include <linux/kernel.h>

#define DIMINUTO_LOG_EMERGENGY      KERN_EMERG
#define DIMINUTO_LOG_ALERT          KERN_ALERT
#define DIMINUTO_LOG_CRITICAL       KERN_CRIT
#define DIMINUTO_LOG_ERROR          KERN_ERR
#define DIMINUTO_LOG_WARNING        KERN_WARNING
#define DIMINUTO_LOG_NOTICE         KERN_NOTICE
#define DIMINUTO_LOG_INFO           KERN_INFO
#define DIMINUTO_LOG_DEBUG          KERN_DEBUG

#else

#include <syslog.h>
#include <stdarg.h>

#define DIMINUTO_LOG_EMERGENGY      LOG_EMERG
#define DIMINUTO_LOG_ALERT          LOG_ALERT
#define DIMINUTO_LOG_CRITICAL       LOG_CRIT
#define DIMINUTO_LOG_ERROR          LOG_ERR
#define DIMINUTO_LOG_WARNING        LOG_WARNING
#define DIMINUTO_LOG_NOTICE         LOG_NOTICE
#define DIMINUTO_LOG_INFO           LOG_INFO
#define DIMINUTO_LOG_DEBUG          LOG_DEBUG

/**
 * If the parent PID of the calling process is not 1, emulate the stdio
 * function perror(); if it is, still emulate perror() but instead of writing
 * the prefixed error string, log it to the system log.
 * @param s points to a nul-terminated string prepended to the error string.
 * @see perror(3).
 */
extern void diminuto_perror(const char * s);

/**
 * If the parent PID of the calling process is not 1, format and print
 * the argument list to stderr; if it is, format and log the argument
 * list to syslog.
 * @param priority is the log priority level.
 * @param format points to a printf-style format string.
 * @param ap is a variable length argument list.
 */
extern void diminuto_log3(int priority, const char * format, va_list ap);

/**
 * If the parent PID of the calling process is not 1, format and print
 * the argument list to stderr; if it is, format and log the argument
 * list to syslog.
 * @param priority is the log priority level.
 * @param format points to a printf-style format string.
 */
extern void diminuto_log(int priority, const char * format, ...);

#endif

#endif

/*
 * This macro provides a logging capability that can be conditionally
 * compiled in or out of a translation unit.
 */

#if defined(DIMINUTO_LOG)
#undef DIMINUTO_LOG
#endif

#if defined(DIMINUTO_LOG_DISABLE)

#define DIMINUTO_LOG(_PRIORITY_, _FORMAT_, ...) ((void)0)

#elif defined(__KERNEL__)

#define DIMINUTO_LOG(_PRIORITY_, _FORMAT_, ...) printk(_PRIORITY_ _FORMAT_, ...)

#else

#define DIMINUTO_LOG(_PRIORITY_, _FORMAT_, ...) diminuto_log(_PRIORITY_, _FORMAT_, __VA_ARGS__)

#endif
