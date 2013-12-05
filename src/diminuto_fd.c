/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2010 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "com/diag/diminuto/diminuto_fd.h"
#include "com/diag/diminuto/diminuto_log.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
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

void ** diminuto_fd_allocate(size_t count)
{
	void ** map;

	map = (void **)calloc(count + 1, sizeof(void *));
	map[0] = (void *)count;

	return map;
}
