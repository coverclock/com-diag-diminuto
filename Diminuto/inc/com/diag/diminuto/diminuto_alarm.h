/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_ALARM_
#define _H_COM_DIAG_DIMINUTO_ALARM_

/**
 * @file
 *
 * Copyright 2009-2018 Digital Aggregates Corporation, Colorado, USA.
 * Licensed under the terms in LICENSE.txt.
 *
 * The Alarm feature provides a convenient API to using SIGALRM.
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
 * Return the number of SIGALRM signals received since the previous call.
 * @return the number of SIGALRM signals received since the previous call.
 */
extern int diminuto_alarm_check(void);

/**
 * Install a SIGALRM signal handler in the caller.
 * @param restart is true if interrupt system calls should be restarted.
 * @return 0 for success, <0 otherwise.
 */
extern int diminuto_alarm_install(int restart);

#endif
