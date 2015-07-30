/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2015 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "com/diag/diminuto/diminuto_terminator.h"
#include "com/diag/diminuto/diminuto_uninterruptiblesection.h"
#include "com/diag/diminuto/diminuto_log.h"
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>

int diminuto_terminator_debug = 0; /* Not part of the public API. */

static int signaled = 0;

int diminuto_terminator_signal(pid_t pid)
{
    int rc = 0;

    if (kill(pid, SIGTERM) < 0) {
        diminuto_perror("diminuto_terminator_signal: kill");
        rc = -1;
    } else if (diminuto_terminator_debug) {
        DIMINUTO_LOG_DEBUG("diminuto_terminator_signal: SIGTERM");
    } else {
        /* Do nothing. */
    }

    return rc;
}

static void diminuto_terminator_handler(int signum)
{
    if (signum == SIGTERM) {
        pid_t pid;
        pid = (getpid() == 1) ? -1 : 0;
        (void)diminuto_terminator_signal(pid);
    }
}

int diminuto_terminator_check(void)
{
    int mysignaled;

    DIMINUTO_UNINTERRUPTIBLE_SECTION_BEGIN(SIGTERM);

        mysignaled = signaled;
        signaled = 0;

    DIMINUTO_UNINTERRUPTIBLE_SECTION_END;

    if (!mysignaled) {
        /* Do nothing. */
    } else if (!diminuto_terminator_debug) {
        /* Do nothing. */
    } else {
        DIMINUTO_LOG_DEBUG("diminuto_terminator_check: SIGTERM");
    }

    return mysignaled;
}

int diminuto_terminator_install(int restart)
{
    struct sigaction terminator;

    memset(&terminator, 0, sizeof(terminator));
    terminator.sa_handler = diminuto_terminator_handler;
    terminator.sa_flags = restart ? SA_RESTART : 0;

    if (sigaction(SIGTERM, &terminator, (struct sigaction *)0) < 0) {
        diminuto_perror("diminuto_terminator_install: sigaction");
        return -1;
    }

    if (diminuto_terminator_debug) {
        DIMINUTO_LOG_DEBUG("diminuto_terminator_install: SIGTERM");
    }

    return 0;
}
