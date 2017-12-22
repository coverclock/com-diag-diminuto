/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2008-2010 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 */

#include "com/diag/diminuto/diminuto_hangup.h"
#include "com/diag/diminuto/diminuto_uninterruptiblesection.h"
#include "com/diag/diminuto/diminuto_log.h"
#include <string.h>
#include <signal.h>
#include <errno.h>

static int signaled = 0;

static void diminuto_hangup_handler(int signum)
{
	if (signum == SIGHUP) {
		signaled = !0;
	}
}

pid_t diminuto_hangup_signal(pid_t pid)
{
    if (pid < 0) {
        errno = EINVAL;
        diminuto_perror("diminuto_hangup_signal: pid");
    } else if (kill(pid, SIGHUP) < 0) {
        diminuto_perror("diminuto_hangup_kill: kill");
        pid = -1;
    }

    return pid;
}

int diminuto_hangup_check(void)
{
    int result;

    DIMINUTO_UNINTERRUPTIBLE_SECTION_BEGIN(SIGHUP);

    	result = signaled;
    	signaled = 0;

    DIMINUTO_UNINTERRUPTIBLE_SECTION_END;

    return result;
}

int diminuto_hangup_install(int restart)
{
    struct sigaction hangup;

    memset(&hangup, 0, sizeof(hangup));
    hangup.sa_handler = diminuto_hangup_handler;
    hangup.sa_flags = restart ? SA_RESTART : 0;

    if (sigaction(SIGHUP, &hangup, (struct sigaction *)0) < 0) {
        diminuto_perror("diminuto_hangup_install: sigaction");
        return -1;
    }

    return 0;
}
