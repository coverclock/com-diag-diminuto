/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2018-2020 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is a unit test of the Interrupter feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is a unit test of the Interrupter feature.
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_interrupter.h"
#include "com/diag/diminuto/diminuto_reaper.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_delay.h"
#include "com/diag/diminuto/diminuto_frequency.h"
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>

extern int diminuto_interrupter_debug;

int main(int argc, char ** argv)
{
    pid_t pid;

    SETLOGMASK();

    TEST();

    CHECKPOINT("BEGIN parent=%d\n", getpid());

    diminuto_interrupter_debug = !0;

    ASSERT(diminuto_interrupter_install(!0) == 0);
    ASSERT(!diminuto_interrupter_check());

    ASSERT(diminuto_reaper_install(!0) == 0);
    ASSERT(!diminuto_reaper_check());

    pid = fork();
    if (pid == 0) {
        CHECKPOINT("BEGIN child=%d\n", getpid());
        while (!diminuto_interrupter_check()) {
            diminuto_yield();
        }
        CHECKPOINT("END child=%d\n", getpid());
        exit(0);
    }

    diminuto_delay(diminuto_frequency(), 0);

    ASSERT(!diminuto_reaper_check());

    CHECKPOINT("SIGNAL parent=%d child=%d\n", getpid(), pid);

    ASSERT(diminuto_interrupter_signal(pid) == 0);

    diminuto_delay(diminuto_frequency(), 0);

    ASSERT(diminuto_reaper_reap((int *)0) == pid);
    ASSERT(diminuto_reaper_check());
    ASSERT(!diminuto_reaper_check());

    CHECKPOINT("END parent=%d\n", getpid());

    STATUS();

    EXIT();
}
