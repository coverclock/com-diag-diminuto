/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2008 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
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

static const char LOCKNAME[] = "/tmp/unittest-hangup.pid";

int main(int argc, char ** argv)
{
	int rc;
	pid_t pid;
    pid_t id;

    SETLOGMASK();

    diminuto_hangup_debug = !0;

	TEST();

    CHECKPOINT("unittest-hangup PARENT BEGIN\n");

    (void)unlink(LOCKNAME);

    rc = diminuto_reaper_install(!0);
    ASSERT(rc == 0);

    ASSERT(!diminuto_reaper_check());
    ASSERT(!diminuto_reaper_check());

    pid = fork();
    ASSERT(pid >= 0);

	if (pid > 0) {

        CHECKPOINT("unittest-hangup PARENT child=%d\n", pid);

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

        ASSERT(diminuto_reaper_check());
        ASSERT(!diminuto_reaper_check());

		CHECKPOINT("unittest-hangup PARENT END\n");

	} else {

		CHECKPOINT("unittest-hangup CHILD BEGIN\n");

		rc = diminuto_hangup_install(!0);
		ASSERT(rc == 0);

		rc = diminuto_lock_lock(LOCKNAME);
		ASSERT(rc == 0);

		while (!diminuto_hangup_check()) {
			diminuto_yield();;
		}

		rc = diminuto_lock_unlock(LOCKNAME);
		ASSERT(rc == 0);

		CHECKPOINT("unittest-hangup CHILD END\n");

	}

    EXIT();
}
