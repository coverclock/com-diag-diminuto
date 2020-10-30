/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_PIPE_
#define _H_COM_DIAG_DIMINUTO_PIPE_

/**
 * @file
 * @copyright Copyright 2018 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief Provides convenience functions for using SIGPIPE.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 *
 * The Pipe feature provides a convenient API for using SIGPIPE.
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
