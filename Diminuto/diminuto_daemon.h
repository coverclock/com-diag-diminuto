/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_DAEMON_
#define _H_COM_DIAG_DIMINUTO_DAEMON_

/**
 * @file
 *
 * Copyright 2008-2010 Digital Aggregates Corporation, Arvada CO 80001-0587 USA<BR>
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
 * @param file if non-null is the path and name of a lock file used to
 * call diminuto_lock_lock().
 * @return 0 to the child upon success, -1 to the parent otherwise.
 * @see diminuto_lock_lock().
 */
extern int diminuto_daemon(const char * file);

#endif
