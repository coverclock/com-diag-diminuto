/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_RENAMEAT2_PRIVATE_
#define _H_COM_DIAG_DIMINUTO_RENAMEAT2_PRIVATE_

/**
 * @file
 *
 * Copyright 2018 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 *
 * This describes the private API for the renameat2(2) feature. It is not
 * accessible to applications outside of Diminuto. This is a system call
 * provided by the Linux kernel starting in 3.15. It is not (yet) supported by
 * glibc. I am of the opinion that there is not any other way to accomplish its
 * function reliably using another combination of system calls.
 */

#include <stdio.h>
#include <fcntl.h>
#include <linux/fs.h>
#define _GNU_SOURCE
#include <unistd.h>
#include <sys/syscall.h>

#if defined(SYS_renameat2)

/**
 * Atomically change the name of a file providing it does not already exist.
 * @param olddirfd is a descriptor referring to the old directory.
 * @param oldpath points to the old file system path.
 * @param newdirfd is a descriptor referring to the new directory.
 * @param newpath points to the new file system path.
 * @param flags is a set of flags modifying behavior.
 * @return 0 if successful, <0 with errno set otherwise.
 */
static inline int renameat2(int olddirfd, const char * oldpath, int newdirfd, const char * newpath, unsigned int flags) {
	return syscall(SYS_renameat2, olddirfd, oldpath, newdirfd, newpath, flags);
}

#else

#warning SYS_renameat2 undefined!

/**
 * Atomically change the name of a file providing it does not  already exist.
 * @param olddirfd is a descriptor referring to the old directory.
 * @param oldpath points to the old file system path.
 * @param newdirfd is a descriptor referring to the new directory.
 * @param newpath points to the new file system path.
 * @param flags is a set of flags modifying behavior.
 * @return -1 for failure always.
 */
static inline int renameat2(int olddirfd, const char * oldpath, int newdirfd, const char * newpath, unsigned int flags) {
    return -1;
}

#if !defined(RENAME_NOREPLACE)
#define RENAME_NOREPLACE 0
#endif

#if !defined(RENAME_EXCHANGE)
#define RENAME_EXCHANGE 0
#endif

#if !defined(RENAME_WHITEOUT)
#define RENAME_WHITEOUT 0
#endif

#if !defined(AT_FDCWD)
#define AT_FDCWD
#endif

#endif

#endif
