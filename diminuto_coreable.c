/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2008-2010 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "diminuto_core.h"
#include "diminuto_log.h"
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <sys/resource.h>
#include <sys/types.h>

int diminuto_core_enable()
{
	int rc = -1;
	struct rlimit limit;

    do {

    	if (getrlimit(RLIMIT_CORE, &limit) < 0) {
		    diminuto_perror("diminuto_core_enable: getrlimit");
            break;
        }

		limit.rlim_cur = limit.rlim_max;

		if (setrlimit(RLIMIT_CORE, &limit) < 0) {
			diminuto_perror("diminuto_core_enable: setrlimit");
            break;
		}

        rc = 0;

    } while (0);

	return rc;
}
