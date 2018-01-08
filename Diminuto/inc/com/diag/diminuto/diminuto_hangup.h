/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_HANGUP_
#define _H_COM_DIAG_DIMINUTO_HANGUP_

/**
 * @file
 *
 * Copyright 2008-2018 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 *
 * N.B. The use of the same signal handler within multiple threads is
 * problematic anyway, and this code is not guaranteed to be thread safe.
 */

#include "com/diag/diminuto/diminuto_types.h"

/**
 * Send a process a SIGHUP signal.
 * @param pid is the process identifier.
 * @return pid or <0 if an error occurred.
 */
extern pid_t diminuto_hangup_signal(pid_t pid);

/**
 * Return true if the caller received a SIGHUP, false otherwise.
 * @return true if the caller received a SIGHUP, false otherwise.
 */
extern int diminuto_hangup_check(void);

/**
 * Install a SIGHUP signal handler in the caller.
 * @param restart is true if interrupt system calls should be restarted.
 * @return 0 for success, <0 otherwise.
 */
extern int diminuto_hangup_install(int restart);

#endif
