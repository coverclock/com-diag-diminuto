/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_REAPER_
#define _H_COM_DIAG_DIMINUTO_REAPER_

/**
 * @file
 * @copyright Copyright 2015-2020 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief Provides convenience functions for using SIGCHLD.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 *
 * The Reaper feature provides a SIGCHLD signal handler and functions to reap
 * any terminating children via a waitpid(2), and optionally logging their
 * termination signals or exit statuses. This prevents a forking application
* from being infested with zombie processes.
 */

#include "com/diag/diminuto/diminuto_types.h" /* For pid_t. */
#include <sys/wait.h> /* For WIFEXISTED etc. macros. */

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
 * Reap the status of a single child that has terminated. A specific PID
 * can be specified or any child can be reaped. If a child is reaped,
 * its status is returned. Can be used in a polling or blocking manner.
 * @param pids specifies the PID of a specific child or -1 for any child.
 * @param statusp points to a variable into which the status is returned.
 * @param flag is passed to the waitpid(2) e.g. WNOHANG to poll.
 * @return PID of the terminated child process, or 0 if none, or <0 if error.
 */
extern pid_t diminuto_reaper_reap_generic(pid_t pids, int * statusp, int flag);

/**
 * Return the process identifier, and the qualified exit status in a value
 * result parameter if the parameter is non-null, of a child process, or 0
 * if there is no terminated child process, or <0 if an error occurred.
 * If a PID is not returned, the value result parameter is left unchanged.
 * @param statusp points to a variable into which the status is returned.
 * @return the PID of the terminated child process, or 0, or <0.
 */
static inline pid_t diminuto_reaper_reap(int * statusp) {
    return diminuto_reaper_reap_generic(-1, statusp, WNOHANG);
}

/**
 * Wait and Return the process identifier, and the qualified exit status
 * in a value result parameter if the parameter is non-null, of a child
 * process, or <0 if an error occurred. If a PID is not returned, the
 * value result parameter is left unchanged.
 * @param statusp points to a variable into which the status is returned.
 * @return the PID of the terminated child process, or <0 if an error occurred.
 */
static inline pid_t diminuto_reaper_wait(int * statusp) {
    return diminuto_reaper_reap_generic(-1, statusp, 0);
}

/**
 * Install a SIGCHLD signal handler in the caller.
 * @param restart is true if interrupt system calls should be restarted.
 * @return 0 for success, <0 otherwise.
 */
extern int diminuto_reaper_install(int restart);

#endif
