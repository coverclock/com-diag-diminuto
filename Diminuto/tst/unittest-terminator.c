/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2015 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_terminator.h"
#include "com/diag/diminuto/diminuto_reaper.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_delay.h"
#include "com/diag/diminuto/diminuto_frequency.h"
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>

extern int diminuto_terminator_debug;

int main(int argc, char ** argv)
{
    pid_t pid;

    SETLOGMASK();

    TEST();

    DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "BEGIN parent=%d\n", getpid());

    diminuto_terminator_debug = !0;

    ASSERT(diminuto_terminator_install(!0) == 0);
    ASSERT(!diminuto_terminator_check());

    ASSERT(diminuto_reaper_install(!0) == 0);
    ASSERT(!diminuto_reaper_check());

    pid = fork();
    if (pid == 0) {
        DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "BEGIN child=%d\n", getpid());
        while (!diminuto_terminator_check()) {
            diminuto_yield();
        }
        DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "END child=%d\n", getpid());
        exit(0);
    }

    diminuto_delay(diminuto_frequency(), 0);

    ASSERT(!diminuto_reaper_check());

    DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "SIGNAL parent=%d child=%d\n", getpid(), pid);

    ASSERT(diminuto_terminator_signal(pid) == 0);

    diminuto_delay(diminuto_frequency(), 0);

    ASSERT(diminuto_reaper_check());

    DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "END parent=%d\n", getpid());

    EXIT();
}
