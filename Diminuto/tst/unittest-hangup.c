/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2008 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

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
#include <unistd.h>
#include <sys/types.h>

static const char LOGNAME[] = "unittest-hangup";
static const char LOCKFILE[] = "/tmp/unittest-hangup.pid";

int main(int argc, char ** argv)
{
	int rc;
	diminuto_ticks_t hertz;

	diminuto_core_enable();

	hertz = diminuto_frequency();

	if (argc > 1) {

		diminuto_hangup_signal(diminuto_lock_check(LOCKFILE));

	} else {

		rc = diminuto_daemon(LOGNAME, LOCKFILE);
		if (rc < 0) { return 2; }

		rc = diminuto_hangup_install(0);
		if (rc < 0) { return 3; }

		while (!diminuto_hangup_check()) {
			diminuto_delay(hertz, 1);
		}

        rc = diminuto_lock_unlock(LOCKFILE);
		if (rc < 0) { return 4; }

	}

    return 0;
}
