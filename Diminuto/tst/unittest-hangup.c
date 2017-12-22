/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2008 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_core.h"
#include "com/diag/diminuto/diminuto_daemon.h"
#include "com/diag/diminuto/diminuto_hangup.h"
#include "com/diag/diminuto/diminuto_delay.h"
#include "com/diag/diminuto/diminuto_lock.h"
#include "com/diag/diminuto/diminuto_time.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_frequency.h"
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

static const char LOCKNAME[] = "/tmp/unittest-hangup.pid";

int main(int argc, char ** argv)
{
	int rc;
	diminuto_ticks_t hertz;
	pid_t pid;
	char buffer[256];

	diminuto_core_enable();

	hertz = diminuto_frequency();

	if (argc == 1) {

		CHECKPOINT("unittest-hangup PARENT BEGIN\n");

		snprintf(buffer, sizeof(buffer), "%s %s", argv[0], "child");

		rc = diminuto_system(buffer);
		ASSERT(rc == 0);

		while ((pid = diminuto_lock_check(LOCKNAME)) < 0) {
			diminuto_delay(hertz, !0);
		}

		rc = diminuto_hangup_signal(pid);
		ASSERT(rc == 0);

		CHECKPOINT("unittest-hangup PARENT END\n");

	} else {

		CHECKPOINT("unittest-hangup CHILD BEGIN\n");

		rc = diminuto_hangup_install(0);
		ASSERT(rc == 0);

		rc = diminuto_lock_lock(LOCKNAME);
		ASSERT(rc == 0);

		while (!diminuto_hangup_check()) {
			diminuto_delay(hertz, !0);
		}

		rc = diminuto_lock_unlock(LOCKNAME);
		ASSERT(rc == 0);

		CHECKPOINT("unittest-hangup CHILD END\n");

	}

    return 0;
}
