/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2018-2022 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is a unit test of the Hangup feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is a unit test of the Hangup feature.
 *
 * Model: parent creates lock file, parent forks child, parent delays for SIGHUP
 * signal from child indicating successful initialization, child populates lock
 * file and signals parent with SIGHUP, parent checks lock file, parent waits
 * for child to exit, parent verifies child status is zero.
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
#include <sys/wait.h>
#include <unistd.h>

extern int diminuto_hangup_debug;

static const char LOCKNAME[] = "/tmp/%s.pid";

int main(int argc, char ** argv)
{
    int rc;
    pid_t pid;
    pid_t id;
    int status;

    SETLOGMASK();

    diminuto_hangup_debug = !0;

    (void)unlink(LOCKNAME);

    TEST();

    CHECKPOINT("PARENT BEGIN\n");

    rc = diminuto_reaper_install(!0);
    ASSERT(rc == 0);

    ASSERT(!diminuto_reaper_check());

    rc = diminuto_hangup_install(!0);
    ASSERT(rc == 0);

    ASSERT(!diminuto_hangup_check());

    rc = diminuto_lock_prelock(LOCKNAME);
    ASSERT(rc == 0);

    ASSERT(diminuto_lock_check(LOCKNAME) == 0);

    pid = fork();
    ASSERT(pid >= 0);

    if (pid > 0) {

        CHECKPOINT("PARENT child=%d\n", pid);

        diminuto_delay(diminuto_frequency() * 60 * 60 * 24, !0);

        ASSERT(diminuto_hangup_check());
        ASSERT(!diminuto_hangup_check());

        id = diminuto_lock_check(LOCKNAME);
        EXPECT(id == pid);

        CHECKPOINT("PARENT READY\n");

        id = diminuto_reaper_wait(&status);
        ASSERT(id == pid);
        ASSERT(status == 0);

        ASSERT(diminuto_reaper_check());
        ASSERT(!diminuto_reaper_check());

        id = diminuto_lock_check(LOCKNAME);
        EXPECT(id < 0);

        CHECKPOINT("PARENT END\n");

    } else {

        CHECKPOINT("CHILD BEGIN\n");

        rc = diminuto_lock_postlock(LOCKNAME);
        ASSERT(rc == 0);

        CHECKPOINT("CHILD READY\n");

        pid = getppid();
        ASSERT(pid >= 0);

        rc = diminuto_hangup_signal(pid);
        ASSERT(rc == 0);

        diminuto_delay(diminuto_frequency(), 0);

        rc = diminuto_lock_unlock(LOCKNAME);
        ASSERT(rc == 0);

        rc = diminuto_lock_unlock(LOCKNAME);
        ASSERT(rc < 0);

        CHECKPOINT("CHILD END\n");

    }

    STATUS();

    EXIT();
}
