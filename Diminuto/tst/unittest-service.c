/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2008-2022 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is a unit test of the Service feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is a unit test of the Service feature.
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
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include "../src/diminuto_daemon.h"

int main(int argc, char ** argv)
{
    int rc;
    pid_t pid0;
    pid_t pid2;
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

    rc = diminuto_service();
    ASSERT(rc >= 0);

    if (rc > 0) {
        ASSERT(getpid() == pid0);
        EXIT();
    }

    pid2 = getpid();
    ASSERT(pid2 > 1);
    ASSERT(pid2 != pid0);

    diminuto_delay(diminuto_frequency() / 100, 0);

    /*
     * This can fail if the child gets to getppid(2) before the parent gets to
     * EXIT() and the child is inherited by init(1). Hence the delay above.
     */

    ppid = getppid();
    ASSERT(ppid == 1);

    spid = getsid(pid2);
    ASSERT(spid > 0);

    CHECKPOINT("SERVICE pid=%d ppid=%d spid=%d\n", pid2, ppid, spid);

    EXIT();
}
