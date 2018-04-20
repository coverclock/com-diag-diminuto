/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2008-2017 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in LICENSE.txt<BR>
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
#include "com/diag/diminuto/diminuto_frequency.h"
#include "com/diag/diminuto/diminuto_delay.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include "../src/diminuto_daemon.h"

extern char * get_current_dir_name(void); /* Supposed to be in <unistd.h>? */

static const char LOGNAME[] = "unittest-daemon";

int main(int argc, char ** argv)
{
    int rc;
    pid_t pid0;
    pid_t pid1;
    pid_t ppid;
    pid_t spid;
    int fds;
    int fd;
    char ch;
    ssize_t ss;
    size_t us;
    char * path;
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

    rc = diminuto_daemon(LOGNAME);
    ASSERT(rc == 0);

    pid1 = getpid();
    ASSERT(pid1 >= 0);
    ASSERT(pid1 != pid0);

    diminuto_delay(diminuto_frequency() / 100, 0);

    /*
     * This can fail in the unlikely event that the child gets to getppid(2)
     * before the parent gets to _exit(2) and the child is inherited by init(1).
     * I've never see it happen here, but it was happening in unittest-service.
     */

    ppid = getppid();
    ASSERT(ppid == 1);

    spid = getsid(pid1);
    ASSERT(spid == pid1);

    CHECKPOINT("DAEMON pid=%d ppid=%d spid=%d\n", pid1, ppid, spid);

    ASSERT(umask(0) == 0);

    path = get_current_dir_name();
    ASSERT(path != (char *)0);
    ASSERT(strcmp(path, "/") == 0);
    free(path);

    ASSERT(STDIN_FILENO == 0);
    ASSERT(STDOUT_FILENO == 1);
    ASSERT(STDERR_FILENO == 2);

    ss = read(STDIN_FILENO, &ch, sizeof(ch));
    ASSERT(ss == 0);

    ss = write(STDOUT_FILENO, &ch, sizeof(ch));
    ASSERT(ss == 1);

    ss = write(STDERR_FILENO, &ch, sizeof(ch));
    ASSERT(ss == 1);

    ASSERT(stdin != (FILE *)0);
    ASSERT(stdout != (FILE *)0);
    ASSERT(stderr != (FILE *)0);

    ASSERT(fileno(stdin) == STDIN_FILENO);
    ASSERT(fileno(stdout) == STDOUT_FILENO);
    ASSERT(fileno(stderr) == STDERR_FILENO);

    us = fread(&ch, sizeof(ch), 1, stdin);
    ASSERT(us == 0);

    us = fwrite(&ch, sizeof(ch), 1, stdout);
    ASSERT(us == 1);

    us = fwrite(&ch, sizeof(ch), 1, stderr);
    ASSERT(us == 1);

    EXIT();
}
