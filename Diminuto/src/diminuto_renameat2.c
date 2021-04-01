/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2020 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is the implementation of the renameat2(2) feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is the implementation of the renameat2(2) feature.
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

#include "../src/diminuto_renameat2.h"
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <linux/fs.h>
#include <sys/syscall.h>
#include <gnu/libc-version.h>

#if defined(COM_DIAG_DIMINUTO_RENAMEAT2_GLIBC)

	/* Do nothing. */

#elif	defined(COM_DIAG_DIMINUTO_RENAMEAT2_SYSCALL)

#   warning renameat2(2) not available on this platform so using syscall(2) SYS_renameat2 instead!

int renameat2(int olddirfd, const char * oldpath, int newdirfd, const char * newpath, unsigned int flags) {
    return syscall(SYS_renameat2, olddirfd, oldpath, newdirfd, newpath, flags);
}

#elif defined(COM_DIAG_DIMINUTO_RENAMEAT2_STUB)

#   warning renameat2(2) or SYS_renameat2 not available on this platform so stubbing out!

int renameat2(int olddirfd, const char * oldpath, int newdirfd, const char * newpath, unsigned int flags) {
    errno = ENOSYS;
    return -1;
}

#else

#   warning renameat2(2) availability is unknown on this platform!

#endif
