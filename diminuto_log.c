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
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>
#include <sys/types.h>

static const char * levels[] = {
    "EMRG",
    "ALRT",
    "CRIT",
    "EROR",
    "WARN",
    "NOTC",
    "INFO",
    "DBUG"
};

void diminuto_perror(const char * s)
{
    pid_t ppid;
    int myerrno;
    const char * e;

    myerrno = errno;
    ppid = getppid();
    errno = myerrno;

    if (ppid != 1) {
        perror(s);
    } else {
        e = strerror(errno);
        syslog(LOG_DAEMON | LOG_ERR, "%s: %s", s, e);
    }
}

void diminuto_log(unsigned int priority, const char * format, ...)
{
    pid_t ppid;
    pid_t pid;
    const char * level = "UNKN";
    va_list list;

    va_start(list, format);

    ppid = getppid();
    pid = getpid();

    if (ppid != 1) {
        if (priority < (sizeof(levels)/sizeof(levels[0]))) {
            level = levels[priority];
        }
        fprintf(stderr, "[%d] %s ", pid, level);
        vfprintf(stderr, format, list);
    } else {
        vsyslog(level, format, list);
    }

    va_end(list);
}
