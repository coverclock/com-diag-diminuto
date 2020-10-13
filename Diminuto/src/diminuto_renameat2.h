/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_RENAMEAT2_PRIVATE_
#define _H_COM_DIAG_DIMINUTO_RENAMEAT2_PRIVATE_

/**
 * @file
 *
 * Copyright 2018-2020 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in LICENSE.txt<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 *
 * This describes the private API for the renameat2(2) feature. It is not
 * accessible to applications outside of Diminuto. This is a system call
 * provided by the Linux kernel starting in 3.15. It is has besn supported
 * in glibc since 2.28. I have convinced myself that there is not any other
 * way to accomplish its function reliably using another combination of
 * system calls.
 */

#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <linux/fs.h>
#include <sys/syscall.h>
#include <gnu/libc-version.h>

#if defined(__USE_GNU) && defined(_GNU_SOURCE) && (((__GLIBC__*1000)+(__GLIBC_MINOR__))>2028)

#   define COM_DIAG_DIMINUTO_RENAMEAT2_GLIBC (1)

    /* renameat2(2) available on GNU 2.28 and later. */

#elif defined(SYS_renameat2)

#   define COM_DIAG_DIMINUTO_RENAMEAT2_SYSCALL (1)

/**
 * Atomically change the name of a file providing it does not already exist.
 * This stub calls the SYS_renameat2 system call directly.
 * @param olddirfd is a descriptor referring to the old directory.
 * @param oldpath points to the old file system path.
 * @param newdirfd is a descriptor referring to the new directory.
 * @param newpath points to the new file system path.
 * @param flags is a set of flags modifying behavior.
 * @return 0 if successful, <0 with errno set otherwise.
 */
extern int renameat2(int olddirfd, const char * oldpath, int newdirfd, const char * newpath, unsigned int flags);

#else

#   define COM_DIAG_DIMINUTO_RENAMEAT2_STUB (1)

/**
 * Atomically change the name of a file providing it does not already exist.
 * This stub returns failure always.
 * @param olddirfd is a descriptor referring to the old directory.
 * @param oldpath points to the old file system path.
 * @param newdirfd is a descriptor referring to the new directory.
 * @param newpath points to the new file system path.
 * @param flags is a set of flags modifying behavior.
 * @return -1 for failure always.
 */
extern int renameat2(int olddirfd, const char * oldpath, int newdirfd, const char * newpath, unsigned int flags);

#   if !defined(RENAME_NOREPLACE)
#       define RENAME_NOREPLACE 0
#   endif

#   if !defined(RENAME_EXCHANGE)
#       define RENAME_EXCHANGE 0
#   endif

#   if !defined(RENAME_WHITEOUT)
#       define RENAME_WHITEOUT 0
#   endif

#   if !defined(AT_FDCWD)
#       define AT_FDCWD 0
#   endif

#endif

#endif
