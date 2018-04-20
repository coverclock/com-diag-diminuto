/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_DAEMON_PRIVATE_
#define _H_COM_DIAG_DIMINUTO_DAEMON_PRIVATE_

/**
 * @file
 *
 * Copyright 2017 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in LICENSE.txt<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 *
 * This is the daemon feature private API. It is used for unit testing, and to
 * expose the toolkit used to implement diminuto_daemon(), diminuto_service(),
 * and diminuto_system() so it may be used to implement other API calls.
 */

#include <stdio.h>
#include <signal.h>
#include <sys/types.h>

typedef enum DiminutoDaemonTest {
	DIMINUTO_DAEMON_TEST_NONE = 0,
	DIMINUTO_DAEMON_TEST_GETPPID,
	DIMINUTO_DAEMON_TEST_INIT,
	DIMINUTO_DAEMON_TEST_FORK,
	DIMINUTO_DAEMON_TEST_WAITPID,
	DIMINUTO_DAEMON_TEST_REFORK,
} diminuto_daemon_test_t;

extern diminuto_daemon_test_t diminuto_daemon_testing;

/**
 * Place the toolkit in a test mode used to exercise failure paths that occur
 * when system calls rarely fail.
 * @param test is the new test mode.
 * @return the prior test mode.
 */
extern diminuto_daemon_test_t diminuto_daemon_test(diminuto_daemon_test_t test);

/**
 * Install a signal handler, logging appropriate error messages should something
 * go amiss.
 * @param signum is the signal number.
 * @param handler is the signal handler.
 * @return 0 for success, <0 otherwise.
 */
extern int diminuto_daemon_install(int signum, void (*handler)(int));

/**
 * Configure a signal to be ignored.
 * @param signum is the signal number.
 * @return 0 for success, <0 otherwise.
 */
static inline int diminuto_daemon_ignore(int signum)
{
	return diminuto_daemon_install(signum, SIG_IGN);
}

/**
 * Configure a signal to be have default behavior.
 * @param signum is the signal number.
 * @return 0 for success, <0 otherwise.
 */
static inline int diminuto_daemon_default(int signum)
{
	return diminuto_daemon_install(signum, SIG_DFL);
}

/**
 * Do any preparation necessary prior to becoming a daemon. Mostly this just
 * means flushing stdout and stderr before redirecting them elsewhere.
 */
extern void diminuto_daemon_prepare(void);

/**
 * Check to see if the caller is already a daemon, that is, its parent is the
 * init(1) process with process identifier 1.
 * @return >0 if a daemon, 0 if not a daemon, <0 if an error occurred.
 */
extern int diminuto_daemon_verify(void);

/**
 * Perform the first fork for daemonization and wait to reap the resulting child
 * as it soon exits.
 * @return the child process ID to the parent, 0 to the child, or <0 if an error occurred.
 */
extern pid_t diminuto_daemon_fork(void);

/**
 * Perform the second fork of daemonization.
 * @return the child process ID to the parent, 0 to the child, or <0 if an error occurred.
 */
extern pid_t diminuto_daemon_refork(void);

/**
 * Redirect a file descriptor number (e.g. 0) to path (e.g. "/dev/null"), and
 * further redirect an existing FILE (e.g. pointed to by stdin) to that
 * descriptor.
 * @param path is the path to which the descriptor is redirected.
 * @param number is the file descriptor number.
 * @param flags are the open(2) flags to be used for the descriptor.
 * @param filep points to a FILE pointer into which the new pointer is stored.
 * @param mode is the fopen(3) mode to be used for the FILE.
 */
extern void diminuto_daemon_redirect(const char * path, int number, int flags, FILE ** filep, const char * mode);

/**
 * Sanitize a process by fixing up problematic signals, setting its user mask,
 * setting its home directory to one guaranteed to exist, closing all of its
 * file descriptors, and redirecting the big three descriptors and FILEs. If
 * name is NULL, the system log is not reopened. If path is NULL, the big three
 * are not redirected.
 * @param name is the system log tag to be used for this process.
 * @param path is the path to which descriptors and FILEs will be redirected.
 */
extern void diminuto_daemon_sanitize(const char * name, const char * path);

#endif
