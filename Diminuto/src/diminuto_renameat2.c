/* vi: set ts=4 expandtab shiftwidth=4: */

/**
 * @file
 *
 * Copyright 2020 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in LICENSE.txt<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 *
 * The Linux-specific function call renameat2(2) is a bit of a mess.
 * It doesn't exist at all in older Linux kernels; it exists only as
 * a system call without glibc support in later Linux/Gnu distros; and
 * it is fully supported by both Linux and glibc in the latest Linux
 * distros. Also, it is the only way to accomplish an atomic exchange
 * of files (for example). So we try to accomodate whatever the host
 * platform provides, since I have test systems in all three
 * categories. (Some versions of Linux/GNU have the system call, and
 * the function prototype in stdio.h, but dont' have the function in
 * glibc; this means the functions can't be inlined without generating
 * a compile warning about a prototype mismatch.)
 */

#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <linux/fs.h>
#include <sys/syscall.h>
#include <gnu/libc-version.h>

#if defined(DIMINUTO_RENAMEAT2_GLIBC)

	/* Do nothing. */

#elif	defined(DIMINUTO_RENAMEAT2_SYSCALL)

int renameat2(int olddirfd, const char * oldpath, int newdirfd, const char * newpath, unsigned int flags) {
    return syscall(SYS_renameat2, olddirfd, oldpath, newdirfd, newpath, flags);
}

#elif defined(DIMINUTO_RENAMEAT2_STUB)

int renameat2(int olddirfd, const char * oldpath, int newdirfd, const char * newpath, unsigned int flags) {
    errno = ENOSYS;
    return -1;
}

#else

	/* Do nothing. */

#endif
