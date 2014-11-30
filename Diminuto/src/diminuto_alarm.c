/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2009-2014 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "com/diag/diminuto/diminuto_alarm.h"
#include "com/diag/diminuto/diminuto_uninterruptiblesection.h"
#include "com/diag/diminuto/diminuto_log.h"
#include <string.h>
#include <signal.h>
#include <errno.h>

int diminuto_alarm_debug = 0; /* Not part of the public API. */

static int signaled = 0;

static void diminuto_alarm_handler(int signum)
{
    if (signum == SIGALRM) {
        signaled = !0;
    } else {
        /* Do nothing. */
    }
}

pid_t diminuto_alarm_signal(pid_t pid)
{
    if (pid < 0) {
        errno = EINVAL;
        diminuto_perror("diminuto_alarm_signal: pid");
    } else if (kill(pid, SIGALRM) < 0) {
        diminuto_perror("diminuto_alarm_signal: kill");
        pid = -1;
    } else if (diminuto_alarm_debug) {
        DIMINUTO_LOG_DEBUG("diminuto_alarm_signal: SIGALRM");
    } else {
        /* Do nothing. */
    }

    return pid;
}

int diminuto_alarm_check(void)
{
    int mysignaled;

    DIMINUTO_UNINTERRUPTIBLE_SECTION_BEGIN(SIGALRM);

        mysignaled = signaled;
        signaled = 0;

    DIMINUTO_UNINTERRUPTIBLE_SECTION_END;

    if (!mysignaled) {
        /* Do nothing. */
    } else if (!diminuto_alarm_debug) {
        /* Do nothing. */
    } else {
        DIMINUTO_LOG_DEBUG("diminuto_alarm_check: SIGALRM");
    }

    return mysignaled;
}

int diminuto_alarm_install(int restart)
{
    struct sigaction alarm;

    memset(&alarm, 0, sizeof(alarm));
    alarm.sa_handler = diminuto_alarm_handler;
    alarm.sa_flags = restart ? SA_RESTART : 0;

    if (sigaction(SIGALRM, &alarm, (struct sigaction *)0) < 0) {
        diminuto_perror("diminuto_alarm_install: sigaction");
        return -1;
    }

    if (diminuto_alarm_debug) {
        DIMINUTO_LOG_DEBUG("diminuto_alarm_install: SIGALRM");
    }

    return 0;
}
