/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_DAEMON_
#define _H_COM_DIAG_DIMINUTO_DAEMON_

/**
 * @file
 *
 * Copyright 2008-2013 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

/**
 * Daemonize the application so that it cannot be affected by the invoking
 * terminal, parent, session, or process group.
 * Upon success, the parent process automatically exits with a 0 exit code,
 * and the function returns 0 to the daemon child process. Upon failure, the
 * daemon child process (if it exists) exists with a 1 exit code, and the
 * function returns -1 to the parent process. Setting errno is typically not
 * possible since the error is most likely to have occurred in the failed
 * child process. All errors, whether they occurred in the parent or the
 * child, are however unconditionally logged to the system log. This function
 * uses signals SIGUSR1, SIGALRM, and SIGCHLD.
 * @param name is the name used for the system log.
 * @param file if non-null is the path and name of a lock/pid file to be used.
 * @param timeout is the number of seconds to wait for the child to respond.
 * @param fail if true forces the child to fail (useful for testing).
 * @return 0 to the child upon success, -1 to the parent otherwise.
 */
extern int diminuto_daemon_generic(const char * name, const char * file, unsigned int timeout, int fail);

/**
 * Daemonize the application so that it cannot be affected by the invoking
 * terminal, parent, session, or process group.
 * Upon success, the parent process automatically exits with a 0 exit code,
 * and the function returns 0 to the daemon child process. Upon failure, the
 * daemon child process (if it exists) exists with a 1 exit code, and the
 * function returns -1 to the parent process. Setting errno is typically not
 * possible since the error is most likely to have occurred in the failed
 * child process. All errors, whether they occurred in the parent or the
 * child, are however unconditionally logged to the system log. This function
 * uses signals SIGUSR1, SIGALRM, and SIGCHLD.
 * @param name is the name used for the system log and the lock file.
 * @param file if non-null is the path and name of a lock/pid file to be used.
 * @return 0 to the child upon success, -1 to the parent otherwise.
 */
extern int diminuto_daemon(const char * name, const char * file);

#endif
