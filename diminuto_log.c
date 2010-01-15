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

diminuto_log_mask_t diminuto_log_mask = DIMINUTO_LOG_MASK_DEFAULT;

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

void diminuto_log3(int priority, const char * format, va_list ap)
{
    const char * level = "UNKN";

    if (getppid() != 1) {
        if (priority < (sizeof(levels) / sizeof(levels[0]))) {
            level = levels[priority];
        }
        fprintf(stderr, "[%d] %s ", getpid(), level);
        vfprintf(stderr, format, ap);
    } else {
        vsyslog(priority, format, ap);
    }
}

void diminuto_log(int priority, const char * format, ...)
{
    va_list ap;
    va_start(ap, format);
    diminuto_log3(priority, format, ap);
    va_end(ap);
}

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
        diminuto_log(LOG_DAEMON | LOG_ERR, "%s: %s", s, strerror(errno));
    }
}
