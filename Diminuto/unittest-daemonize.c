/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2008 Digital Aggregates Corporation, Arvada CO 80001-0587 USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "diminuto_unittest.h"
#include "diminuto_coreable.h"
#include "diminuto_daemonize.h"
#include "diminuto_delay.h"
#include "diminuto_lock.h"
#include "diminuto_log.h"
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <syslog.h>

static const char * file = "/tmp/unittest-daemonize.pid";

int main(int argc, char ** argv)
{
    int rc;
    pid_t pid1;
    pid_t pid2;
    pid_t pid3;
    pid_t pid4;

    diminuto_coreable();

    pid1 = getpid();
    ASSERT(pid1 >= 0);

    pid2 = diminuto_locked(file);
    ASSERT(pid2 < 0);

    rc = diminuto_lock(file);
    ASSERT(rc == 0);

    pid2 = diminuto_locked(file);
    ASSERT(pid2 > 0);
    ASSERT(pid1 == pid2);

    diminuto_log(DIMINUTO_LOG_PRIORITY_NOTICE, "unittest-daemonize: parent %d\n", pid2);

    rc = diminuto_lock(file);
    ASSERT(rc < 0);

    rc = diminuto_unlock(file);
    ASSERT(rc == 0);

    pid2 = diminuto_locked(file);
    ASSERT(pid2 < 0);

    rc = diminuto_unlock(file);
    ASSERT(rc < 0);

    rc = diminuto_daemonize(file);
    ASSERT(rc == 0);

    pid3 = getpid();
    ASSERT(pid3 > 0);

    rc = diminuto_lock(file);
    ASSERT(rc == 0);

    pid4 = diminuto_locked(file);
    ASSERT(pid4 > 0);
    ASSERT(pid3 == pid4);

    /*
     * This delay is necessary for the parent to get the SIGUSR1 signal
     * from the daemon child; otherwise the daemon child exits before the
     * parent gets back into the run state and sends a SIGCHLD signal to
     * the parent instead. This wouldn't be necessary in a normal daemon
     * child which keeps running. It also gives the tester a chance to
     * verify the presence and contents of the lock file.
     */

    diminuto_delay(10 * 1000000, 0);

    rc = diminuto_unlock(file);
    ASSERT(rc == 0);

    diminuto_log(DIMINUTO_LOG_PRIORITY_NOTICE, "unittest-daemonize: child %d\n", pid4);

    return errors > 255 ? 255 : errors;
}
