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
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>

int diminuto_lock(const char * file)
{
    int fd = -1;
    FILE * fp = (FILE *)0;
    pid_t pid;
    int rc;
    int result = 0;

    do {

        if ((fd = open(file, O_WRONLY | O_CREAT | O_EXCL, 0644)) < 0) {
            result = -1;
            diminuto_perror("diminuto_lock: open");
            break;
        }

        if ((pid = getpid()) < 0) {
            result = -2;
            diminuto_perror("diminuto_lock: getpid");
            break;
        }

        if ((fp = fdopen(fd, "w")) == (FILE *)0) {
            result = -3;
            diminuto_perror("diminuto_lock: fdopen");
            break;
        }

        if (fprintf(fp, "%d", pid) < 0) {
            result = -4;
            diminuto_perror("diminuto_lock: fprintf");
            break;
        }

    } while (0);

    if (fp == (FILE *)0) {
        /* Do nothing. */
    } else if (fclose(fp) == EOF) {
        result = -5;
        diminuto_perror("diminuto_lock: fclose");
    } else {
        fp = (FILE *)0;
        fd = -1;
    }

    if (fd < 0) {
        /* Do nothing. */
    } else if (close(fd) < 0) {
        result = -6;
        diminuto_perror("diminuto_lock: close");
    } else {
        fd = -1;
    }

    return result;
}

int diminuto_unlock(const char * file)
{
    int rc;
    int result = 0;

    if (unlink(file) < 0) {
        result = -1;
        diminuto_perror("diminuto_unlock: unlink");
    }

    return result;
}

pid_t diminuto_locked(const char * file)
{
    int fd = -1;
    FILE * fp = (FILE *)0;
    pid_t pid;
    int rc;

    do {

        if ((fd = open(file, O_RDONLY, 0)) < 0) {
            pid = -1;
            diminuto_perror("diminuto_lock: open");
            break;
        }

        if ((fp = fdopen(fd, "r")) == (FILE *)0) {
            pid = -2;
            diminuto_perror("diminuto_locked: fdopen");
            break;
        }

        if (fscanf(fp, "%d", &pid) != 1) {
            pid = -3;
            errno = EINVAL;
            diminuto_perror("diminuto_locked: fscanf");
            break;
        }

    } while (0);

    if (fp == (FILE *)0) {
        /* Do nothing. */
    } else if (fclose(fp) == EOF) {
        pid = -4;
        diminuto_perror("diminuto_locked: fclose");
    } else {
        fp = (FILE *)0;
        fd = -1;
    }

    if (fd < 0) {
        /* Do nothing. */
    } else if (close(fd) < 0) {
        pid = -5;
        diminuto_perror("diminuto_locked: close");
    } else {
        fd = -1;
    }

    return pid;
}
