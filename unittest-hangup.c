/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2008 Digital Aggregates Corporation, Arvada CO 80001-0587 USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "diminuto_coreable.h"
#include "diminuto_daemonize.h"
#include "diminuto_hangup.h"
#include "diminuto_delay.h"
#include "diminuto_lock.h"
#include "diminuto_log.h"
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>

static const char * file = "/tmp/unittest-hangup.pid";

int main(int argc, char ** argv)
{
	int rc;

	diminuto_coreable();

	if (argc > 1) {

		diminuto_hangup(file);

	} else {

		rc = diminuto_daemonize(file);
		if (rc < 0) { return 2; }

		rc = diminuto_hangupable();
		if (rc < 0) { return 3; }

		while (!diminuto_hungup()) {
			diminuto_delay(1000000, 1);
		}

        rc = diminuto_unlock(file);
		if (rc < 0) { return 4; }

	}

    return 0;
}
