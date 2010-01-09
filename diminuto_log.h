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
 *
 * This header file can legitimately be included more than once in a
 * single translation unit.
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

#endif

/*
 * This macro provides a logging capability that can be conditionally
 * compiled in or out of a translation unit.
 */

#if defined(DIMINUTO_LOG)
#undef DIMINUTO_LOG
#endif

#if defined(DIMINUTO_DEBUG)
#define DIMINUTO_LOG(_ARGS_) diminuto_log _ARGS_
#else
#define DIMINUTO_LOG(_ARGS_) ((void)0)
#endif
