/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2008 Digital Aggregates Corporation, Arvada CO 80001-0587 USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "diminuto_lock.h"
#include "diminuto_log.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <syslog.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>

int diminuto_lock(const char * file)
{
    int fd;
    pid_t pid;
    FILE * fp;
    int rc;
    int result = 0;

    do {

        if ((fd = open(file, O_WRONLY | O_CREAT | O_EXCL, 0644)) < 0) {
            result = -30;
            diminuto_perror("diminuto_lock: open");
            break;
        }

        if ((pid = getpid()) < 0) {
            result = -31;
            diminuto_perror("diminuto_lock: getpid");
            break;
        }

        if ((fp = fdopen(fd, "w")) == (FILE *)0) {
            result = -32;
            diminuto_perror("diminuto_lock: fdopen");
            break;
        }

        if (fprintf(fp, "%d", pid) < 0) {
            result = -33;
            diminuto_perror("diminuto_lock: fprintf");
            break;
        }

        if (fclose(fp) < 0) {
            result = -34;
            diminuto_perror("diminuto_lock: fclose");
            break;
        }

        fd = -1;

    } while (0);

    if (fd < 0) {
        /* Do nothing. */
    } else if (close(fd) < 0) {
        diminuto_perror("diminuto_lock: close");
    }

    return result;
}

int diminuto_unlock(const char * file)
{
    int rc;
    int result = 0;

    if (unlink(file) < 0) {
        result = -35;
        diminuto_perror("diminuto_unlock: unlink");
    }

    return result;
}

pid_t diminuto_locked(const char * file)
{
    int fd;
    FILE * fp;
    pid_t pid;
    int rc;

    do {

        if ((fd = open(file, O_RDONLY, 0)) < 0) {
            pid = -36;
            diminuto_perror("diminuto_lock: open");
            break;
        }

        if ((fp = fdopen(fd, "r")) == (FILE *)0) {
            pid = -37;
            diminuto_perror("diminuto_locked: fdopen");
            break;
        }

        if (fscanf(fp, "%d", &pid) != 1) {
            pid = -38;
            errno = EINVAL;
            diminuto_perror("diminuto_locked: fscanf");
            break;
        }

        if (fclose(fp) < 0) {
            diminuto_perror("diminuto_locked: fclose");
            break;
        }

        fd = -1;

    } while (0);

    if (fd < 0) {
        /* Do nothing. */
    } else if (close(fd) < 0) {
        diminuto_perror("diminuto_locked: close");
    }

    return pid;
}
