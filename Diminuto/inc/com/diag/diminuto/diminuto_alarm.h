/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_ALARM_
#define _H_COM_DIAG_DIMINUTO_ALARM_

/**
 * @file
 *
 * Copyright 2009-2018 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 */

#include "com/diag/diminuto/diminuto_types.h"

/**
 * Send a process a SIGALRM signal.
 * @param pid is the process identifier, or 0 for process group, or -1 for all
 * processes for whom the sender has permissions to send a signal.
 * @return 0 or <0 if an error occurred.
 */
extern int diminuto_alarm_signal(pid_t pid);

/**
 * Return true if the caller received a SIGALRM, false otherwise.
 * @return true if the caller received a SIGALRM, false otherwise.
 */
extern int diminuto_alarm_check(void);

/**
 * Install a SIGALRM signal handler in the caller.
 * @param restart is true if interrupt system calls should be restarted.
 * @return 0 for success, <0 otherwise.
 */
extern int diminuto_alarm_install(int restart);

#endif
