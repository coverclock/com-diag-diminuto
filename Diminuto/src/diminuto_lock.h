/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_LOCK_PRIVATE_
#define _H_COM_DIAG_DIMINUTO_LOCK_PRIVATE_

/**
 * @file
 *
 * Copyright 2018 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in LICENSE.txt<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 */

#include <unistd.h>

/**
 * Atomically create a lock file with the specified path and name (e.g.
 * "/var/run/foo.pid") using an exclusive open such that if the file
 * already exists the function fails, and containing the text string of the
 * provided value. This only works reliably if the file is in the local
 * file system.
 * @param file is the path and name of the lock file.
 * @param pid is the value to write into the lock file.
 * @return 0 if successful, <0 with errno set otherwise.
 */
extern int diminuto_lock_lock_generic(const char * file, pid_t pid);

/**
 * Atomically write the text string of the provided value into the specified
 * file.
 * @param file is the path and name of the lock file.
 * @param pid is the value to write into the lock file.
 * @return 0 if successful, <0 with errno set otherwise.
 */
extern int diminuto_lock_postlock_generic(const char * file, pid_t pid);

#endif
