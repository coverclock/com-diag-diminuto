/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2009 Digital Aggregates Corporation, Arvada CO 80001-0587 USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "diminuto_alarm.h"
#include "diminuto_log.h"
#include <signal.h>
#include <errno.h>

static int signaled = 0;

static void diminuto_handler(int signum)
{
	if (signum == SIGALRM) {
		signaled = !0;
	}
}

pid_t diminuto_alarm_signal(pid_t pid)
{
    if (pid < 0) {
        errno = EINVAL;
        diminuto_perror("diminuto_alarm: pid");
    } else if (kill(pid, SIGALRM) < 0) {
        diminuto_perror("diminuto_alarm: kill");
        pid = -1;
    }

    return pid;
}

int diminuto_alarm_check(void)
{
    int mysignaled;
    sigset_t set;
    sigset_t was;

    sigemptyset(&set);
    sigaddset(&set, SIGALRM);
    sigprocmask(SIG_BLOCK, &set, &was);

    mysignaled = signaled;
    signaled = 0;

    sigprocmask(SIG_SETMASK, &was, (sigset_t *)0);

    return mysignaled;
}

int diminuto_alarm_install(void)
{
    if (signal(SIGALRM, diminuto_handler) == SIG_ERR) {
        diminuto_perror("diminuto_alarmable: signal");
        return -1;
    }

    return 0;
}
