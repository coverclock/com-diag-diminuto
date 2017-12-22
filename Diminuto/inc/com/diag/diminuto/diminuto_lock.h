/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_LOCK_
#define _H_COM_DIAG_DIMINUTO_LOCK_

/**
 * @file
 *
 * Copyright 2008-2017 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 */

#include "com/diag/diminuto/diminuto_types.h"
#include "com/diag/diminuto/diminuto_platform.h"

/*
 * This is just a suggestion, but it is useful for unit testing.
 * See unittest-daemon.c for an example of how this is used. In
 * practice, you should probably use a dedicated directory. On most
 * server systems this would be under something like "/var/run/lock/".
 * But non-root processes like unit tests can write to the directories below.
 * You must use an absolute path because when the daemon tries to instantiate
 * the lock file it's current directory will have already been changed to "/".
 */
#if defined(COM_DIAG_DIMINUTO_LOCK_ROOT)
	/* Do nothing. */
#elif defined(COM_DIAG_DIMINUTO_PLATFORM_BIONIC)
#	define COM_DIAG_DIMINUTO_LOCK_ROOT "/data/local/tmp/"
#else
#	define COM_DIAG_DIMINUTO_LOCK_ROOT "/tmp/"
#endif

/**
 * Create a lock file with the specified path and name (e.g.
 * "/var/run/foo.pid") using an exclusive open such that if the file
 * already exists the function fails. The file will contain the PID
 * of the calling process. This only works reliabily if the file is
 * in the local file system. This function may create a temporary
 * file whose name is the file name appended with ".tmp", for example
 * "/var/run/foo.pid.tmp".
 * @param file is the path and name of the lock file.
 * @return 0 if successful, <0 with errno set otherwise.
 */
extern int diminuto_lock_lock(const char * file);

/**
 * Remove a lock file with the specified path and name (e.g.
 * "/var/run/foo.pid". It is an error to call this function with
 * a lock file that does not exist.
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
