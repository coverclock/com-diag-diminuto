/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2008-2009 Digital Aggregates Corporation, Arvada CO 80001-0587 USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "diminuto_hangup.h"
#include "diminuto_log.h"
#include <signal.h>
#include <errno.h>

static int signaled = 0;

static void diminuto_handler(int signum)
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
    sigset_t set;
    sigset_t was;

    sigemptyset(&set);
    sigaddset(&set, SIGHUP);
    sigprocmask(SIG_BLOCK, &set, &was);

    result = signaled;
    signaled = 0;

    sigprocmask(SIG_SETMASK, &was, (sigset_t *)0);

    return result;
}

int diminuto_hangup_install(void)
{
    if (signal(SIGHUP, diminuto_handler) == SIG_ERR) {
        diminuto_perror("diminuto_hangup_install: signal");
        return -1;
    }

    return 0;
}
