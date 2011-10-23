/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2008 Digital Aggregates Corporation, Arvada CO 80001-0587 USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "com/diag/diminuto/diminuto_core.h"
#include "com/diag/diminuto/diminuto_daemon.h"
#include "com/diag/diminuto/diminuto_hangup.h"
#include "com/diag/diminuto/diminuto_delay.h"
#include "com/diag/diminuto/diminuto_lock.h"
#include "com/diag/diminuto/diminuto_log.h"
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>

static const char * file = "/tmp/unittest-hangup.pid";

int main(int argc, char ** argv)
{
	int rc;

	diminuto_core_enable();

	if (argc > 1) {

		diminuto_hangup_signal(diminuto_lock_check(file));

	} else {

		rc = diminuto_daemon(file);
		if (rc < 0) { return 2; }

		rc = diminuto_hangup_install(0);
		if (rc < 0) { return 3; }

		while (!diminuto_hangup_check()) {
			diminuto_delay(1000000, 1);
		}

        rc = diminuto_lock_unlock(file);
		if (rc < 0) { return 4; }

	}

    return 0;
}
