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
#include "diminuto_core.h"
#include "diminuto_daemon.h"
#include "diminuto_delay.h"
#include "diminuto_lock.h"
#include "diminuto_log.h"
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>

static const char * file = "/tmp/unittest-daemonize.pid";

int main(int argc, char ** argv)
{
    int rc;
    pid_t pid1;
    pid_t pid2;
    pid_t pid3;
    pid_t pid4;
    pid_t pid5;
    pid_t pid6;
    pid_t pid7;
    pid_t ppid;

    diminuto_log_emit("unittest-daemonize: begin %d\n", getpid());

    diminuto_core_enable();

    pid1 = getpid();
    ASSERT(pid1 >= 0);

    diminuto_emit("unittest-daemonize: parent %d\n", pid1);

    rc = diminuto_lock_lock(file);
    ASSERT(rc == 0);

    rc = diminuto_daemon(file);
    ASSERT(rc < 0);

    rc = diminuto_lock_unlock(file);
    ASSERT(rc == 0);

    pid7 = getpid();
    ASSERT(pid7 == pid1);

    rc = diminuto_daemon(file);
    ASSERT(rc == 0);

    pid2 = getpid();
    ASSERT(pid2 > 0);
    ASSERT(pid1 != pid2);

    diminuto_log_emit("unittest-daemonize: child %d\n", pid2);

    ppid = getppid();
    ASSERT(ppid > 0);
    ASSERT(ppid == 1);

    diminuto_log_emit("unittest-daemonize: adopted %d\n", ppid);

    rc = diminuto_lock_lock(file);
    ASSERT(rc < 0);

    pid3 = diminuto_lock_check(file);
    ASSERT(pid3 > 0);
    ASSERT(pid2 == pid3);

    rc = diminuto_lock_unlock(file);
    ASSERT(rc == 0);

    rc = diminuto_lock_unlock(file);
    ASSERT(rc < 0);

    diminuto_emit("unittest-daemonize: end %d\n", getpid());

    EXIT();
}
