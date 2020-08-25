/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2015 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in LICENSE.txt<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_reaper.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_delay.h"
#include "com/diag/diminuto/diminuto_frequency.h"
#include "com/diag/diminuto/diminuto_countof.h"
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>

extern int diminuto_reaper_debug;

int main(int argc, char ** argv)
{
    pid_t pids[10];
    pid_t pid;
    int status;
    int ii;
    int kk;

    SETLOGMASK();

    TEST();

    diminuto_reaper_debug = !0;

    ASSERT(diminuto_reaper_install(!0) == 0);

    ASSERT(!diminuto_reaper_check());

    for (ii = 0; ii < countof(pids); ++ii) {
        if ((pids[ii] = fork()) == 0) {
            DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "ii=%d pid=%d\n", ii, getpid());
            diminuto_delay((diminuto_frequency() / 10) * ii, 0);
            DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "ii=%d pid=%d exit=%d\n", ii, getpid(), ii);
            exit(ii);
        }
    }

    diminuto_delay(diminuto_frequency() * 5, 0);
    ASSERT(diminuto_reaper_check());

    while ((pid = diminuto_reaper_reap(&status)) > 0) {
        for (ii = 0; ii < countof(pids); ++ii) {
            if (pid == pids[ii]) {
                break;
            }
        }
        ASSERT(ii < countof(pids));
        DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "ii=%d pid=%d exit=%d\n", ii, pid, status);
        ASSERT(WIFEXITED(status));
        ASSERT(WEXITSTATUS(status) == ii);
    }
    ASSERT(pid == 0);

    if ((pid = fork()) == 0) {
        DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "ii=%d pid=%d\n", ii, getpid());
        while (!0) {
            diminuto_yield();
        }
    }

    diminuto_delay(diminuto_frequency(), 0);
    kill(pid, SIGINT);
    ASSERT(diminuto_reaper_wait((int *)0) == pid);
    ASSERT(diminuto_reaper_check());

    pid = getpid();
    ASSERT(diminuto_reaper_signal(pid) == 0);
    ASSERT(diminuto_reaper_wait((int *)0) == 0);

    EXIT();
}
