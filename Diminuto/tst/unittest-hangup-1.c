/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2008-2022 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is a unit test of the Hangup feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is a unit test of the Hangup feature.
 *
 * Model: parent forks child, waits for child to create lock file, signals
 * child to exit with SIGHUP, waits for child to remove lock file.
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_hangup.h"
#include "com/diag/diminuto/diminuto_reaper.h"
#include "com/diag/diminuto/diminuto_delay.h"
#include "com/diag/diminuto/diminuto_lock.h"
#include "com/diag/diminuto/diminuto_time.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_frequency.h"
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

extern int diminuto_hangup_debug;

static const char LOCKNAME[] = "/tmp/%s.pid";

int main(int argc, char ** argv)
{
    int rc;
    pid_t pid;
    pid_t id;

    SETLOGMASK();

    diminuto_hangup_debug = !0;

    (void)unlink(LOCKNAME);

    TEST();

    CHECKPOINT("PARENT BEGIN\n");

    rc = diminuto_reaper_install(!0);
    ASSERT(rc == 0);

    ASSERT(!diminuto_reaper_check());
    ASSERT(!diminuto_reaper_check());

    pid = fork();
    ASSERT(pid >= 0);

    if (pid > 0) {

        CHECKPOINT("PARENT child=%d\n", pid);

        while ((id = diminuto_lock_check(LOCKNAME)) < 0) {
            diminuto_yield();
        }

        EXPECT(id == pid);

        rc = diminuto_hangup_signal(pid);
        ASSERT(rc == 0);

        while ((id = diminuto_lock_check(LOCKNAME)) > 0) {
            diminuto_yield();
        }

        diminuto_delay(diminuto_frequency(), 0);

        ASSERT(diminuto_reaper_wait((int *)0) == pid);

        ASSERT(diminuto_reaper_check());
        ASSERT(!diminuto_reaper_check());

        CHECKPOINT("PARENT END\n");

    } else {

        CHECKPOINT("CHILD BEGIN\n");

        rc = diminuto_hangup_install(!0);
        ASSERT(rc == 0);

        rc = diminuto_lock_lock(LOCKNAME);
        ASSERT(rc == 0);

        while (!diminuto_hangup_check()) {
            diminuto_yield();
        }

        rc = diminuto_lock_unlock(LOCKNAME);
        ASSERT(rc == 0);

        CHECKPOINT("CHILD END\n");

    }

    STATUS();

    EXIT();
}
