/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_PIPE_
#define _H_COM_DIAG_DIMINUTO_PIPE_

/**
 * @file
 *
 * Copyright 2018 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 */

#include "com/diag/diminuto/diminuto_types.h"

/**
 * Send a process a SIGPIPE signal.
 * @param pid is the process identifier, or 0 for process group, or -1 for all
 * processes for whom the sender has permissions to send a signal.
 * @return 0 or <0 if an error occurred.
 */
extern int diminuto_pipe_signal(pid_t pid);

/**
 * Return the number of SIGPIPE signals received since the previous call.
 * @return the number of SIGPIPE signals received since the previous call.
 */
extern int diminuto_pipe_check(void);

/**
 * Install a SIGPIPE signal handler in the caller.
 * @param restart is true if interrupt system calls should be restarted.
 * @return 0 for success, <0 otherwise.
 */
extern int diminuto_pipe_install(int restart);

#endif
