/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2008 Digital Aggregates Corporation, Arvada CO 80001-0587 USA<BR>
 * Licensed under the GNU GPL V2<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "diminuto_log.h"
#include <stdio.h>
#include <syslog.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
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
