/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_DAEMON_
#define _H_COM_DIAG_DIMINUTO_DAEMON_

/**
 * @file
 *
 * Copyright 2008-2017 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

/**
 * Daemonize the application so that it cannot be affected by the invoking
 * terminal, parent, session, or process group. Not all errors can be indicated
 * to the parent or to the child (depending on the process context in which they
 * occur), but all errors are logged to the system log.
 * @param name is the name used when opening the system log.
 * @return 0 to child upon success, -1 to parent if otherwise.
 */
extern int diminuto_daemon(const char * name);

/**
 * Run the specified shell command in the background detached from the caller.
 * @param command is the shell command string.
 * @return 0 upon success, -1 otherwise.
 */
extern int diminuto_system(const char * command);

#endif
