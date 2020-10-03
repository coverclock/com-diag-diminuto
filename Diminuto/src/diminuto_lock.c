/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2008-2020 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in LICENSE.txt<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 *
 * See the unit tests unittest-lock.c, unittest-hangup-wan.c,
 * unittest-hangup-too.c, unittest-hangup-tree.c, and unittest-hangup-fore.c
 * for examples of patterns in how this feature might be used.
 */

#include "../src/diminuto_renameat2.h"
#include "../src/diminuto_lock.h"
#include "com/diag/diminuto/diminuto_lock.h"
#include "com/diag/diminuto/diminuto_log.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>

int diminuto_lock_f(const char * file, const char * suffix, pid_t pid, unsigned int type)
{
    int rc = -1;
    int fd = -1;
    FILE * fp = (FILE *)0;
    char * path = (char *)0;

    do {

        path = (char *)malloc(strlen(file) + strlen(suffix) + 1);
        if (path == (char *)0) {
            diminuto_perror("diminuto_lock_f: malloc");
            break;
        }

        strcpy(path, file);
        strcat(path, suffix);

        if ((fd = mkstemp(path)) >= 0) {
            /* Do nothing. */
        } else if (errno == EEXIST) {
            break;
        } else {
            diminuto_perror("diminuto_lock_f: mkstemp");
            break;
        }

        if ((fp = fdopen(fd, "w")) == (FILE *)0) {
            diminuto_perror("diminuto_lock_f: fdopen");
            break;
        }

        if (fprintf(fp, "%d\n", pid) < 0) {
           diminuto_perror("diminuto_lock_f: fprintf");
           break;
        }

        if (fclose(fp) == EOF) {
            diminuto_perror("diminuto_lock_f: fclose");
            break;
        }

        fp = (FILE *)0;
        fd = -1;

        /*
         * N.B. atomic.
         */

        if (renameat2(AT_FDCWD, path, AT_FDCWD, file, type) >= 0) {
            /* Do nothing. */
        } else if ((type == RENAME_NOREPLACE) && (errno == EEXIST)) {
            break;
        } else {
            diminuto_perror("diminuto_lock_f: renameat2");
            break;
        }

        rc = 0;

    } while (0);

    if (fp == (FILE *)0) {
        /* Do nothing. */
    } else if (fclose(fp) == 0) {
        fp = (FILE *)0;
        fd = -1;
    } else {
        diminuto_perror("diminuto_lock_f: fclose");
    }

    if (fd < 0) {
        /* Do nothing. */
    } else if (close(fd) == 0) {
        /* Do nothing. */
     } else {
        diminuto_perror("diminuto_lock_f: close");
    }

    if (path != (char *)0) {
        (void)unlink(path);
        free(path);
        path = (char *)0;
    }

    return rc;
}

int diminuto_lock_lock_generic(const char * file, pid_t pid)
{
    return diminuto_lock_f(file, "-lock-XXXXXX", pid, RENAME_NOREPLACE);
}

int diminuto_lock_lock(const char * file)
{
    int rc = -1;
    pid_t pid = -1;

    if ((pid = getpid()) < 0) {
        diminuto_perror("diminuto_lock_lock: getpid");
    } else {
        rc = diminuto_lock_lock_generic(file, pid);
    }

    return rc;
}

int diminuto_lock_prelock(const char * file)
{
    int rc = -1;
    int fd = -1;

    /*
     * N.B. atomic.
     */

    if ((fd = open(file, O_CREAT | O_EXCL | O_WRONLY, 0600)) >= 0) {
        rc = 0;
    } else if (errno == EEXIST) {
        /* Do nothing. */
    } else {
        diminuto_perror("diminuto_lock_prelock: open");
    }

    if (fd < 0) {
        /* Do nothing. */
    } else if (close(fd) == 0) {
        /* Do nothing. */
     } else {
        diminuto_perror("diminuto_lock_prelock: close");
    }

    return rc;
}

int diminuto_lock_postlock_generic(const char * file, pid_t pid)
{
    return diminuto_lock_f(file, "-post-XXXXXX", pid, RENAME_EXCHANGE);
}

int diminuto_lock_postlock(const char * file)
{
    int rc = -1;
    pid_t pid = -1;

    if ((pid = getpid()) < 0) {
        diminuto_perror("diminuto_lock_postlock: getpid");
    } else {
        rc = diminuto_lock_postlock_generic(file, pid);
    }

    return rc;
}

int diminuto_lock_unlock(const char * file)
{
    int rc = 0;

    if (unlink(file) >= 0) {
        /* Do nothing. */
    } else if (errno == ENOENT) {
        rc = -1;
    } else {
        diminuto_perror("diminuto_lock_unlock: unlink");
        rc = -1;
    }

    return rc;
}

pid_t diminuto_lock_check(const char * file)
{
    pid_t pid = -1;
    FILE * fp = (FILE *)0;
    int rc = -1;

    do {

        if ((fp = fopen(file, "r")) == (FILE *)0) {
            break;
        }

        rc = fscanf(fp, "%d\n", &pid);
        if (rc != 1) {
            pid = 0;
            break;
        }

    } while (0);

    if (fp == (FILE *)0) {
        /* Do nothing. */
    } else if (fclose(fp) == EOF) {
        diminuto_perror("diminuto_lock_postlock: fclose");
    } else {
        fp = (FILE *)0;
    }

    return pid;
}

int diminuto_lock_file_generic(const char * file, pid_t pid)
{
    return diminuto_lock_f(file, "-file-XXXXXX", pid, 0);
}

int diminuto_lock_file(const char * file)
{
    int rc = -1;
    pid_t pid = -1;

    if ((pid = getpid()) < 0) {
        diminuto_perror("diminuto_lock_file: getpid");
    } else {
        rc = diminuto_lock_file_generic(file, pid);
    }

    return rc;
}
