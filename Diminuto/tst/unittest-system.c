/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2008-2017 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 *
 * Be aware that a bunch of the possibly diagnostic output goes into the
 * system log. Where this is exactly and what it is called will differ from
 * system to system. On my Ubunth 14.04 system it is "/var/log/syslog".
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_core.h"
#include "com/diag/diminuto/diminuto_daemon.h"
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include "../src/diminuto_daemon.h"

static const char COMMAND[] = "ps -l";

int main(int argc, char ** argv)
{
    int rc;
    pid_t pid0;
    pid_t ppid;
    pid_t spid;
    int test;

    diminuto_core_enable();

    test = (argc < 2) ? 0 : strtoul(argv[1], (char **)0, 0);
    diminuto_daemon_test((diminuto_daemon_test_t)test);
    if (test != 0) { CHECKPOINT("TEST test=%d\n", test); }

    pid0 = getpid();
    ASSERT(pid0 > 1);

    ppid = getppid();
    ASSERT(ppid > 1);

    spid = getsid(pid0);
    ASSERT(spid >= 0);

    CHECKPOINT("PARENT pid=%d ppid=%d spid=%d\n", pid0, ppid, spid);

    rc = diminuto_system(COMMAND);
    ASSERT(rc == 0);

    ASSERT(getpid() == pid0);

    EXIT();
}
