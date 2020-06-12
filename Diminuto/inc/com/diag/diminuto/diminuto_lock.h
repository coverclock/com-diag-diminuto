/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_LOCK_
#define _H_COM_DIAG_DIMINUTO_LOCK_

/**
 * @file
 *
 * Copyright 2008-2018 Digital Aggregates Corporation, Colorado, USA.
 * Licensed under the terms in LICENSE.txt.
 *
 * The Lock feature provides a mechanism to implement old-school atomic
 * locks using the file system.
 */

#include "com/diag/diminuto/diminuto_types.h"
#include "com/diag/diminuto/diminuto_platform.h"

/*
 * This is just a suggestion, but it is useful for unit testing. In
 * practice, you should probably use a dedicated directory. On most
 * server systems this would be under something like "/var/run/lock/".
 * But non-root processes like unit tests can write to the directories below.
 * You must use an absolute path because when a daemon tries to instantiate
 * the lock file its current directory may have already been changed to "/".
 */
#if defined(COM_DIAG_DIMINUTO_LOCK_ROOT)
    /* Do nothing. */
#elif defined(COM_DIAG_DIMINUTO_PLATFORM_BIONIC)
#	define COM_DIAG_DIMINUTO_LOCK_ROOT "/data/local/tmp/"
#else
#	define COM_DIAG_DIMINUTO_LOCK_ROOT "/tmp/"
#endif

/**
 * Atomically create a lock file with the specified path and name (e.g.
 * "/var/run/foo.pid") using an exclusive open such that if the file
 * already exists the function fails. The file will be empty.
 * This function can be used, for example, by a parent process to determine if
 * the child is already running; the child can populate the file with its own
 * PID using diminuto_lock_postlock() as a mechanism to inform the parent that
 * it is running.
 * @param file is the path and name of the lock file.
 * @return 0 if successful, <0 with errno set otherwise.
 */
extern int diminuto_lock_prelock(const char * file);

/**
 * Atomically write the PID text string of the current process into the
 * specified file.
 * @param file is the path and name of the lock file.
 * @return 0 if successful, <0 with errno set otherwise.
 */
extern int diminuto_lock_postlock(const char * file);

/**
 * Atomically create a lock file with the specified path and name (e.g.
 * "/var/run/foo.pid") using an exclusive open such that if the file
 * already exists the function fails, and containing the PID text
 * string of the calling process. This only works reliably if the file is
 * in the local file system.
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
 * @return a PID if locked, 0 if prelocked, <0 with errno set otherwise.
 */
extern pid_t diminuto_lock_check(const char * file);

/**
 * Atomically create a file with the specified path (e.g. "/var/run/foo.pid")
 * containing the PID text string of the calling process. This has no lock
 * semantics, but is useful for creating a file containing a PID.
 * @param file is the path and name of the lock file.
 * @return 0 if successful, <0 with errno set otherwise.
 */
extern int diminuto_lock_file(const char * file);

#endif
