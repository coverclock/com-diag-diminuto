/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_DIMINUTO_LOG_
#define _H_DIMINUTO_LOG_

/**
 * @file
 *
 * Copyright 2008 Digital Aggregates Corporation, Arvada CO 80001-0587 USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

/**
 * If the parent PID of the calling process is not 1, emulate the stdio
 * function perror(); if it is, still emulate perror() but instead of writing
 * the prefixed error string, log it to the system log.
 * @param s points to a nul-terminated string prepended to the error string.
 * @see perror(3).
 */
extern void diminuto_perror(const char * s);

#endif
