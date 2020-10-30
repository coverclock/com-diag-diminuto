/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_DAEMON_
#define _H_COM_DIAG_DIMINUTO_DAEMON_

/**
 * @file
 * @copyright Copyright 2008-2017 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief Provides a mechanism to daemonize the caller.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 *
 * The Daemon feature provides an API to daemonize a process, handling
 * all the details of standard I/O redirection.
 */

#include <sys/types.h>

/**
 * Daemonize the application so that it cannot be affected by the invoking
 * terminal, parent, session, or process group. Not all errors can be indicated
 * to the parent or to the child (depending on the process context in which they
 * occur), but all errors are logged to the system log. The parent always exits.
 * @param name is the name used when opening the system log.
 * @return 0 to child and exit(0) parent upon success, exit(1) otherwise.
 */
extern int diminuto_daemon(const char * name);

/**
 * Daemonize the application but allow the child to inherit all the file
 * descriptors from the parent (who itself may be a daemon).
 * @return 0 to child and >0 to parent upon success, <0 to parent otherwise.
 */
extern int diminuto_service(void);

/**
 * Run the specified shell command in the background detached from the caller.
 * @param command is the shell command string.
 * @return 0 to parent upon success, <0 to parent otherwise.
 */
extern int diminuto_system(const char * command);

#endif
