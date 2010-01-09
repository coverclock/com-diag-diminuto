/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_LOG_
#define _H_COM_DIAG_DIMINUTO_LOG_

/**
 * @file
 *
 * Copyright 2009 Digital Aggregates Corporation, Arvada CO 80001-0587 USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include <syslog.h>

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
 */
extern void diminuto_log(unsigned int priority, const char * format, ...);

#if 0
#define	LOG_EMERG	0	/* system is unusable */
#define	LOG_ALERT	1	/* action must be taken immediately */
#define	LOG_CRIT	2	/* critical conditions */
#define	LOG_ERR		3	/* error conditions */
#define	LOG_WARNING	4	/* warning conditions */
#define	LOG_NOTICE	5	/* normal but significant condition */
#define	LOG_INFO	6	/* informational */
#define	LOG_DEBUG	7	/* debug-level messages */
#endif

#endif

#if defined(DIMINUTO_LOG)
#undef DIMINUTO_LOG
#endif

#if defined(DIMINUTO_DEBUG)
#define DIMINUTO_LOG(_ARGS_) diminuto_log _ARGS_
#else
#define DIMINUTO_LOG(_ARGS_) ((void)0)
#endif
