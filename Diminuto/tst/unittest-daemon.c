/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2008-2013 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_core.h"
#include "com/diag/diminuto/diminuto_daemon.h"
#include "com/diag/diminuto/diminuto_delay.h"
#include "com/diag/diminuto/diminuto_lock.h"
#include "com/diag/diminuto/diminuto_log.h"
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>

static const char LOGNAME[] = "unittest-daemon";
static const char LOCKFILE[] = "/tmp/unittest-daemon.pid";

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
    pid_t pid8;
    pid_t ppid;
    int fds;
    int fd;
    char ch;
    ssize_t ss;
    size_t us;

    diminuto_core_enable();

    pid1 = getpid();
    ASSERT(pid1 >= 0);

    CHECKPOINT("PARENT1 pid=%d\n", pid1);

    diminuto_lock_unlock(LOCKFILE);

    rc = diminuto_daemon_generic(LOGNAME, LOCKFILE, 10, !0);
    ASSERT(rc < 0);

    pid8 = getpid();
    ASSERT(pid8 == pid1);

    CHECKPOINT("PARENT2 pid=%d\n", pid8);

    diminuto_lock_unlock(LOCKFILE);

    rc = diminuto_lock_lock(LOCKFILE);
    ASSERT(rc == 0);

    rc = diminuto_daemon(LOGNAME, LOCKFILE);
    ASSERT(rc < 0);

    pid7 = getpid();
    ASSERT(pid7 == pid1);

    CHECKPOINT("PARENT3 pid=%d\n", pid7);

    rc = diminuto_lock_unlock(LOCKFILE);
    ASSERT(rc == 0);

    rc = diminuto_daemon(LOGNAME, LOCKFILE);
    ASSERT(rc == 0);

    pid2 = getpid();
    ASSERT(pid2 > 0);
    ASSERT(pid1 != pid2);

    ppid = getppid();
    ASSERT(ppid > 0);
    ASSERT(ppid == 1);

    CHECKPOINT("CHILD pid=%d ppid=%d\n", pid2, ppid);

    rc = diminuto_lock_lock(LOCKFILE);
    ASSERT(rc < 0);

    pid3 = diminuto_lock_check(LOCKFILE);
    ASSERT(pid3 > 0);
    ASSERT(pid2 == pid3);

    ASSERT(STDIN_FILENO == 0);
    ss = read(STDIN_FILENO, &ch, sizeof(ch));
    ASSERT(ss == 0);

    ASSERT(STDOUT_FILENO == 1);
    ss = write(STDOUT_FILENO, &ch, sizeof(ch));
    ASSERT(ss == 1);

    ASSERT(STDERR_FILENO == 2);
    ss = write(STDERR_FILENO, &ch, sizeof(ch));
    ASSERT(ss == 1);

    ASSERT(stdin != (FILE *)0);
    us = fread(&ch, sizeof(ch), 1, stdin);
    ASSERT(us == 0);

    ASSERT(stdout != (FILE *)0);
    us = fwrite(&ch, sizeof(ch), 1, stdout);
    ASSERT(us == 1);

    ASSERT(stderr != (FILE *)0);
    us = fwrite(&ch, sizeof(ch), 1, stderr);
    ASSERT(us == 1);

    /*
     * Can't test all fds because we don't know which one is the syslog
     * socket and the there is no call in the syslog API to tell us.
     */

    rc = diminuto_lock_unlock(LOCKFILE);
    ASSERT(rc == 0);

    rc = diminuto_lock_unlock(LOCKFILE);
    ASSERT(rc < 0);

    CHECKPOINT("DAEMON\n");

    EXIT();
}
