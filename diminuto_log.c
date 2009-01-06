/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2009 Digital Aggregates Corporation, Arvada CO 80001-0587 USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "diminuto_log.h"
#include <stdio.h>
#include <syslog.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>
#include <sys/types.h>

void diminuto_perror(const char * s)
{
    pid_t ppid;
    int myerrno;

    myerrno = errno;
    ppid = getppid();
    errno = myerrno;
    if (ppid != 1) {
        perror(s);
    } else {
        const char * e;
        e = strerror(errno);
        syslog(LOG_DAEMON | LOG_ERR, "%s: %s", s, e);
    }
}

void diminuto_log(int level, const char * format, ...)
{
    pid_t ppid;
    va_list list;

    va_start(list, format);
    ppid = getppid();
    if (ppid != 1) {
        vfprintf(stderr, format, list);
    } else {
        vsyslog(level, format, list);
    }
    va_end(list);
}
