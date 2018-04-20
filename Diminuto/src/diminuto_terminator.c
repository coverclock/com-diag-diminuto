/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2015-2018 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in LICENSE.txt<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 * EXPERIMENTAL * UNTESTED
 */

#include "com/diag/diminuto/diminuto_terminator.h"
#include "com/diag/diminuto/diminuto_uninterruptiblesection.h"
#include "com/diag/diminuto/diminuto_criticalsection.h"
#include "com/diag/diminuto/diminuto_log.h"
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>

int diminuto_terminator_debug = 0; /* Not part of the public API. */

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
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
    if (signum != SIGTERM) {
        /* Do nothing. */
    } else if (signaled < (~(((int)1) << ((sizeof(signaled) * 8) - 1)))) {
       	signaled += 1;
    } else {
        /* Do nothing. */
    }
}

int diminuto_terminator_check(void)
{
    int mysignaled;

    DIMINUTO_CRITICAL_SECTION_BEGIN(&mutex);
    	DIMINUTO_UNINTERRUPTIBLE_SECTION_BEGIN(SIGTERM);

        	mysignaled = signaled;
        	signaled = 0;

        DIMINUTO_UNINTERRUPTIBLE_SECTION_END;
    DIMINUTO_CRITICAL_SECTION_END;

    if (!mysignaled) {
        /* Do nothing. */
    } else if (diminuto_terminator_debug) {
        DIMINUTO_LOG_DEBUG("diminuto_terminator_check: SIGTERM");
    } else {
        /* Do nothing. */
    }

    return mysignaled;
}

int diminuto_terminator_install(int restart)
{
    int rc = 0;
    struct sigaction terminator = { 0 };

    terminator.sa_handler = diminuto_terminator_handler;
    terminator.sa_flags = restart ? SA_RESTART : 0;

    if (sigaction(SIGTERM, &terminator, (struct sigaction *)0) < 0) {
        diminuto_perror("diminuto_terminator_install: sigaction");
        rc = -1;
    } else if (diminuto_terminator_debug) {
        DIMINUTO_LOG_DEBUG("diminuto_terminator_install: SIGTERM");
    } else {
        /* Do nothing. */
    }

    signaled = 0;

    return rc;
}
