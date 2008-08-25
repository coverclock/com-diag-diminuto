/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2008 Digital Aggregates Corporation, Arvada CO 80001-0587 USA<BR>
 * Licensed under the GNU GPL V2<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 *
 * @see http://www-theorie.physik.unizh.ch/~dpotter/howto/daemonize
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

pid_t diminuto_lock(const char * file)
{
    int fd;
    pid_t pid;
    FILE * fp;
    int rc;
    int result = 0;

    do {

        fd = open(file, O_WRONLY | O_CREAT | O_EXCL, 0444);
        if (fd < 0) {
            result = -30;
            diminuto_perror("diminuto_lock: open");
            break;
        }

        pid = getpid();
        if (pid < 0) {
            result = -31;
            diminuto_perror("diminuto_lock: getpid");
            break;
        }

        fp = fdopen(fd, "w");
        if (fp == (FILE *)0) {
            result = -32;
            diminuto_perror("diminuto_lock: fdopen");
            break;
        }

        rc = fprintf(fp, "%d", pid);
        if (rc < 0) {
            result = -33;
            diminuto_perror("diminuto_lock: fprintf");
            break;
        }

        rc = fclose(fp);
        if (rc < 0) {
            result = -34;
            diminuto_perror("diminuto_lock: fclose");
            break;
        }

    } while (0);

    return result;
}

int diminuto_unlock(const char * file)
{
    int rc;
    int result = 0;

    rc = unlink(file);
    if (rc < 0) {
        diminuto_perror("diminuto_unlock: unlink");
        result = -35;
    }

    return result;
}
