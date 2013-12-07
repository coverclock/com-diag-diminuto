/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2010-2013 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "com/diag/diminuto/diminuto_fd.h"
#include "com/diag/diminuto/diminuto_countof.h"
#include "com/diag/diminuto/diminuto_log.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

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
        diminuto_perror(device ? device : "close");
    } else {
        fd = -1;
    }

    return fd;
}

ssize_t diminuto_fd_read(int fd, void * buffer, size_t min, size_t max)
{
    ssize_t total = 0;
    char * bp;
    ssize_t current;
    size_t slack;

    bp = (char *)buffer;
    slack = max;

    while (slack > 0) {
        if ((current = read(fd, bp, slack)) == 0) {
            break;
        } else if (current < 0) {
            if ((errno != EINTR) && (errno != EAGAIN)) {
                diminuto_perror("diminuto_fd_read: read");
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

ssize_t diminuto_fd_write(int fd, const void * buffer, size_t min, size_t max)
{
    ssize_t total = 0;
    const char * bp;
    ssize_t current;
    size_t slack;

    bp = (char *)buffer;
    slack = max;

    while (slack > 0) {
        if ((current = write(fd, bp, slack)) == 0) {
            break;
        } else if (current < 0) {
            if ((errno != EINTR) && (errno != EAGAIN)) {
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

size_t diminuto_fd_count(void)
{
	long count;

	count = sysconf(_SC_OPEN_MAX);

	return (count >= 0) ? count : 0;
}

diminuto_fd_map_t * diminuto_fd_map_alloc(size_t count)
{
	diminuto_fd_map_t * mapp;
	size_t size;

	size = count * sizeof(mapp->data);
	mapp = (diminuto_fd_map_t *)malloc(sizeof(*mapp) - sizeof(mapp->data) + size);
	mapp->count = count;
	memset(&mapp->data, 0, size);

	return mapp;
}

void ** diminuto_fd_map_ref(diminuto_fd_map_t * mapp, int fd)
{
	return ((0 <= fd) && (fd < mapp->count)) ? &mapp->data[fd] : (void **)0;
}
