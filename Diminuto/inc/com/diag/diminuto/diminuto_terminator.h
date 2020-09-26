/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_TERMINATOR_
#define _H_COM_DIAG_DIMINUTO_TERMINATOR_

/**
 * @file
 *
 * Copyright 2015-2018 Digital Aggregates Corporation, Colorado, USA.
 * Licensed under the terms in LICENSE.txt.

 * The Terminator feature provides a convenient API for using SIGTERM.
 * It propagates the SIGTERM signal to appropriate processes (children or
 * process group) when it receives one.
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
 * Return the number of SIGTERM signals received since the previous call.
 * @return the number of SIGTERM signals received since the previous call.
 */
extern int diminuto_terminator_check(void);

/**
 * Install a SIGTERM signal handler in the caller.
 * @param restart is true if interrupt system calls should be restarted.
 * @return 0 for success, <0 otherwise.
 */
extern int diminuto_terminator_install(int restart);

#endif
