/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2015 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_reaper.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_delay.h"
#include "com/diag/diminuto/diminuto_frequency.h"
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>

int main(int argc, char ** argv)
{
    pid_t pid;
    int ii;
    extern int diminuto_reaper_debug;

    SETLOGMASK();

    diminuto_reaper_debug = !0;

    ASSERT(diminuto_reaper_install(!0) == 0);

    ASSERT(!diminuto_reaper_check());

    for (ii = 0; ii < 10; ++ii) {
        if ((pid = fork()) == 0) {
            DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "ii=%d pid=%d\n", ii, getpid());
            diminuto_delay((diminuto_frequency() / 10) * ii, 0);
            exit(ii);
        }
    }

    diminuto_delay(diminuto_frequency() * 5, 0);
    ASSERT(diminuto_reaper_check());

    if ((pid = fork()) == 0) {
        DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "ii=%d pid=%d\n", ii, getpid());
        while (!0) {
            diminuto_yield();
        }
    }

    diminuto_delay(diminuto_frequency(), 0);
    kill(pid, SIGINT);
    diminuto_delay(diminuto_frequency(), 0);
    ASSERT(diminuto_reaper_check());

    pid = getpid();
    ASSERT(diminuto_reaper_signal(pid) == pid);

    EXIT();
}
