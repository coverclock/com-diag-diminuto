/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_DAEMONIZE_
#define _H_COM_DIAG_DIMINUTO_DAEMONIZE_

/**
 * @file
 *
 * Copyright 2008 Digital Aggregates Corporation, Arvada CO 80001-0587 USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

/**
 * Daemonize the application so that it cannot be affected by the invoking
 * terminal, parent, session, or process group. If an error occurs, the
 * state of the application is not well defined since it will be left
 * somewhere in between being a normal process and being a daemon.
 * @param file if non-null is the path and name of a lock file used to
 * call diminuto_lock().
 * @return 0 to the daemon child if successful, <0 with errno set if an
 * error occurred; the parent exits with a 0 if the daemonization was
 * successful, >0 otherwise.
 * @see diminuto_lock().
 */
extern int diminuto_daemonize(const char * file);

#endif
