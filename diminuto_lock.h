/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_LOCK_
#define _H_COM_DIAG_DIMINUTO_LOCK_

/**
 * @file
 *
 * Copyright 2008 Digital Aggregates Corporation, Arvada CO 80001-0587 USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "diminuto_types.h"

/**
 * Create a lock file with the specified path and name (e.g.
 * "/var/run/foo.pid") using an exclusive open such that if the file
 * already exists the function fails. The file will contain the PID
 * of the calling process. This only works reliabily if the file is
 * in the local file system.
 * @param file is the path and name of the lock file.
 * @return 0 if successful, <0 with errno set otherwise.
 */
extern int diminuto_lock_lock(const char * file);

/**
 * Remove a lock file with the specified path and name (e.g.
 * "/var/run/foo.pid".
 * @param file is the path and name of the lock file.
 * @return 0 if successful, <0 with errno set otherwise.
 */
extern int diminuto_lock_unlock(const char * file);

/**
 * Read the PID from a lock file with the specified path and name
 * (e.g. "/var/run/foo.pid") and return it.
 * @param file is the path and name of the lock file.
 * @return a PID if successful, <0 with errno set otherwise.
 */
extern pid_t diminuto_lock_check(const char * file);

#endif
