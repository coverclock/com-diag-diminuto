/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2008 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

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
#include <sys/syscall.h>>
#include <linux/fs.h>

int diminuto_lock_lock(const char * file)
{
	int rc = -1;
	int fd = -1;
    FILE * fp = (FILE *)0;
    pid_t pid = -1;
    char * path = (char *)0;
    static const char SUFFIX[] = ".tmp";

    do {

    	path = (char *)malloc(strlen(file) + sizeof(SUFFIX));
    	if (path == (char *)0) {
    		diminuto_perror("diminuto_lock_lock: malloc");
    		break;
    	}

    	strcpy(path, file);
    	strcat(path, SUFFIX);

        if ((fd = open(path, O_WRONLY | O_CREAT | O_EXCL, 0644)) >= 0) {
        	/* Do nothing. */
        } else if (errno == EEXIST) {
        	break;
        } else {
            diminuto_perror("diminuto_lock_lock: open");
            break;
        }

        if ((pid = getpid()) < 0) {
            diminuto_perror("diminuto_lock_lock: getpid");
            break;
        }

        if ((fp = fdopen(fd, "w")) == (FILE *)0) {
            diminuto_perror("diminuto_lock_lock: fdopen");
            break;
        }

        if (fprintf(fp, "%d\n", pid) < 0) {
           diminuto_perror("diminuto_lock_lock: fprintf");
           break;
        }

        if (fclose(fp) == EOF) {
        	diminuto_perror("diminuto_lock_lock: fclose");
        	break;
        }

        fp = (FILE *)0;
        fd = -1;

        /* renameat2(AT_FDCWD, path, AT_FDCWD, file, RENAME_NOREPLACE) */

        if (syscall(SYS_renameat2, AT_FDCWD, path, AT_FDCWD, file, RENAME_NOREPLACE) >= 0) {
        	/* Do nothing. */
        } else if (errno == EEXIST) {
        	break;
        } else {
        	diminuto_perror("diminuto_lock_lock: rename");
        	break;
        }

        rc = 0;

    } while (0);

    if (path != (char *)0) {
    	(void)unlink(path);
    	free(path);
    }

    if (fp == (FILE *)0) {
        /* Do nothing. */
    } else if (fclose(fp) == EOF) {
        diminuto_perror("diminuto_lock_lock: fclose");
    } else {
        fp = (FILE *)0;
        fd = -1;
    }

    if (fd < 0) {
        /* Do nothing. */
    } else if (close(fd) < 0) {
        diminuto_perror("diminuto_lock_lock: close");
    } else {
        fd = -1;
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
    int fd = -1;
    FILE * fp = (FILE *)0;
    int rc = -1;

    do {

        if ((fd = open(file, O_RDONLY, 0)) >= 0) {
        	/* Do nothing. */
        } else if (errno == ENOENT) {
        	break;
        } else {
            diminuto_perror("diminuto_lock_check: open");
            break;
        }

        if ((fp = fdopen(fd, "r")) == (FILE *)0) {
            diminuto_perror("diminuto_lock_check: fdopen");
            break;
        }

        if (fscanf(fp, "%d", &pid) != 1) {
            errno = EINVAL;
            diminuto_perror("diminuto_lock_check: fscanf");
            break;
        }

    } while (0);

    if (fp == (FILE *)0) {
        /* Do nothing. */
    } else if (fclose(fp) == EOF) {
        diminuto_perror("diminuto_lock_check: fclose");
    } else {
        fp = (FILE *)0;
        fd = -1;
    }

    if (fd < 0) {
        /* Do nothing. */
    } else if (close(fd) < 0) {
        diminuto_perror("diminuto_lock_check: close");
    } else {
        fd = -1;
    }

    return pid;
}
