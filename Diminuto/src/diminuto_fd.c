/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2010-2023 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is the implementation of the File Descriptor (FD) feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is the implementation of the File Descriptor (FD) feature.
 */

#include "com/diag/diminuto/diminuto_fd.h"
#include "com/diag/diminuto/diminuto_countof.h"
#include "com/diag/diminuto/diminuto_environment.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_memory.h"
#include "com/diag/diminuto/diminuto_platform.h"
#include <errno.h>
#include <fcntl.h>
#if !defined(COM_DIAG_DIMINUTO_PLATFORM_DARWIN)
#   include <malloc.h>
#endif
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <unistd.h>
#include <sys/ioctl.h> /* FIONREAD, TIOCINQ */
#include <sys/resource.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <sys/types.h>
#if defined(COM_DIAG_DIMINUTO_PLATFORM_CYGWIN)
#   include <asm/socket.h> /* FIONREAD */
#else
#endif

#if !defined(TIOCINQ)
#   warning TIOCINQ not defined on this platform!
#   define TIOCINQ FIONREAD
#endif

#if !defined(O_DIRECT)
#   warning O_DIRECT not defined on this platform!
#   define O_DIRECT 0
#endif

int diminuto_fd_acquire(int fd, const char * device, int flags, mode_t mode)
{
    if (fd >= 0) {
        /* Do nothing: already open. */
    } else if ((fd = open(device, flags, mode)) < 0) {
        diminuto_perror(device);
    } else {
        /* Do nothing: success. */
    }

    return fd;
}

int diminuto_fd_relinquish(int fd, const char * device)
{
    if (fd < 0) {
        /* Do nothing: not open. */
    } else if (close(fd) < 0) {
        diminuto_perror(device ? device : "diminuto_fd_relinquish: close");
    } else {
        fd = -1;
    }

    return fd;
}

ssize_t diminuto_fd_read_generic(int fd, void * buffer, size_t min, size_t max)
{
    ssize_t total = 0;
    char * bp = (char *)0;
    ssize_t current = -1;
    size_t slack = 0;

    bp = (char *)buffer;
    slack = max;

    while (slack > 0) {
        if ((current = read(fd, bp, slack)) == 0) {
            break;
        } else if (current < 0) {
            if (errno == EINTR) {
                /* Do nothing. */
            } else if (errno == EAGAIN) {
                /* Do nothing. */
            } else if (errno == EWOULDBLOCK) {
                /* Do nothing. */
            } else {
                diminuto_perror("diminuto_fd_read_generic: read");
            }
            if (total == 0) {
                total = current;
            }
            break;
        } else {
            bp += current;
            slack -= current;
            total += current;
            if (total >= (ssize_t)min) {
                break;
            }
        }
    }

    return total;
}

ssize_t diminuto_fd_write_generic(int fd, const void * buffer, size_t min, size_t max)
{
    ssize_t total = 0;
    const char * bp = (const char *)0;
    ssize_t current = -1;
    size_t slack = 0;

    bp = (char *)buffer;
    slack = max;

    while (slack > 0) {
        if ((current = write(fd, bp, slack)) == 0) {
            break;
        } else if (current < 0) {
            if (errno == EINTR) {
                /* Do nothing. */
            } else if (errno == EAGAIN) {
                /* Do nothing. */
            } else if (errno == EWOULDBLOCK) {
                /* Do nothing. */
            } else {
                diminuto_perror("diminuto_fd_write: write");
            }
            if (total == 0) {
                total = current;
            }
            break;
        } else {
            bp += current;
            slack -= current;
            total += current;
            if (total >= (ssize_t)min) {
                break;
            }
        }
    }

    return total;
}

ssize_t diminuto_fd_readable_generic(int fd, int suppress)
{
    ssize_t result = -1;
    int request = -1;
    int count = -1;

    /*
     * On some platforms, TIOCINQ and FIONREAD are the same ioctl.
     * Others, not. Some platforms don't define TIOCINQ at all.
     */

    request = isatty(fd) ? TIOCINQ : FIONREAD;

    if (ioctl(fd, request, &count) >= 0) {
        result = count;
    } else if (!suppress) {
        diminuto_perror("diminuto_fd_readable: ioctl");
    } else {
        /* Do nothing. */
    }

    return result;
}

ssize_t diminuto_fd_count(void)
{
    return FD_SETSIZE;
}

ssize_t diminuto_fd_limit(void)
{
    ssize_t result = -1;
    struct rlimit limit;

    if (getrlimit(RLIMIT_NOFILE, &limit) >= 0) {
        result = limit.rlim_cur;
    } else {
        diminuto_perror("diminuto_fd_limit: getrlimit");
    }

    return result;
}

ssize_t diminuto_fd_maximum(void)
{
    ssize_t result = -1;

    DIMINUTO_ENVIRONMENT_READER_BEGIN;
        result = sysconf(_SC_OPEN_MAX);
    DIMINUTO_ENVIRONMENT_READER_END;
    if (result < 0) {
        diminuto_perror("diminuto_fd_maximum: sysconf");
    }

    return result;
}

diminuto_fd_map_t * diminuto_fd_map_alloc(size_t count)
{
    diminuto_fd_map_t * mapp = (diminuto_fd_map_t *)0;
    size_t size = 0;

    size = count * sizeof(mapp->data[0]);
    mapp = (diminuto_fd_map_t *)malloc(sizeof(*mapp) + size);
    mapp->count = count;
    memset(&mapp->data, 0, size);

    return mapp;
}

void ** diminuto_fd_map_ref(diminuto_fd_map_t * mapp, int fd)
{
    return ((0 <= fd) && (fd < mapp->count)) ? &(mapp->data[fd]) : (void **)0;
}

void * diminuto_fd_direct_alloc(size_t size)
{
    ssize_t alignment = -1;
    void * pointer = (void *)0;

    if ((alignment = getpagesize()) <= 0) {
        errno = EINVAL;
        diminuto_perror("diminuto_fd_direct_alloc: getpagesize");
    } else if ((pointer = diminuto_memory_aligned(alignment, size)) == 0) {
        /* Do nothing. */
    } else {
        /* Do nothing. */
    }

    return pointer;
}

int diminuto_fd_direct_acquire(int fd, const char * device, int flags, mode_t mode)
{
    return diminuto_fd_acquire(fd, device, flags | O_DIRECT | O_SYNC, mode);
}

diminuto_fd_type_t diminuto_fd_type(int fd)
{
    diminuto_fd_type_t type = DIMINUTO_FS_TYPE_UNKNOWN;
    struct stat status = { 0 };

    if (isatty(fd)) {
        type = DIMINUTO_FS_TYPE_TTY;
    } else if (fstat(fd, &status) < 0) {
        diminuto_perror("diminuto_fd_type: fstat");
    } else {
        type = diminuto_fs_mode2type(status.st_mode);
        if (type == DIMINUTO_FS_TYPE_UNKNOWN) {
            errno = EINVAL;
            diminuto_perror("diminuto_fd_type: stat.st_mode");
        }
    }

    return type;
}
