/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_TERMINATOR_
#define _H_COM_DIAG_DIMINUTO_TERMINATOR_

/**
 * @file
 *
 * Copyright 2015 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 *
 * The terminator feature propagates a SIGTERM signal to appropriate processes
 * (children or process group) when it receives one.
 */

#include "com/diag/diminuto/diminuto_types.h"

/**
 * Send a process a SIGTERM signal.
 * @param pid is the process identifier, or 0 for process group, or -1 for all
 * processes for whom the sender has permissions to send a signal.
 * @return 0 or <0 if an error occurred.
 */
extern int diminuto_terminator_signal(pid_t pid);

/**
 * Return true if the caller received a SIGTERM, false otherwise.
 * @return true if the caller received a SIGTERM, false otherwise.
 */
extern int diminuto_terminator_check(void);

/**
 * Install a SIGTERM signal handler in the caller.
 * @param restart is true if interrupt system calls should be restarted.
 * @return 0 for success, <0 otherwise.
 */
extern int diminuto_terminator_install(int restart);

#endif
