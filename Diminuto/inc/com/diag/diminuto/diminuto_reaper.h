/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_REAPER_
#define _H_COM_DIAG_DIMINUTO_REAPER_

/**
 * @file
 *
 * Copyright 2015-2018 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in LICENSE.txt<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 *
 * The reaper feature provides a SIGCHLD signal handler that will reap any
 * terminating children via a waitpid(2), and logging their termination signals
 * or exit statuses. This prevents a forking application from being infested
 * with zombie processes (although most applications will likely choose to do
 * the waitpid(2) themselves).
 */

#include "com/diag/diminuto/diminuto_types.h"

/**
 * Send a process a SIGCHLD signal.
 * @param pid is the process identifier, or 0 for process group, or -1 for all
 * processes for whom the sender has permissions to send a signal.
 * @return 0 or <0 if an error occurred.
 */
extern int diminuto_reaper_signal(pid_t pid);

/**
 * Return the number of SIGCHLD signals received since the previous call.
 * @return the number of SIGCHLD signals received since the previous call.
 */
extern int diminuto_reaper_check(void);

/**
 * Install a SIGCHLD signal handler in the caller. The handler will
 * reap any pending child zombie processes.
 * @param restart is true if interrupt system calls should be restarted.
 * @return 0 for success, <0 otherwise.
 */
extern int diminuto_reaper_install(int restart);

#endif
