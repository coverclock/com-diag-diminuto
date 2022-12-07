/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2008-2018 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is the implementation of the Hangup feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is the implementation of the Hangup feature.
 */

#include "com/diag/diminuto/diminuto_hangup.h"
#include "com/diag/diminuto/diminuto_uninterruptiblesection.h"
#include "com/diag/diminuto/diminuto_criticalsection.h"
#include "com/diag/diminuto/diminuto_log.h"
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <pthread.h>

int diminuto_hangup_debug = 0; /* Not part of the public API. */

static volatile int signaled = 0;
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

pid_t diminuto_hangup_signal(pid_t pid)
{
    int rc = 0;

    if (kill(pid, SIGHUP) < 0) {
        diminuto_perror("diminuto_hangup_kill: kill");
        rc = -1;
    } else if (diminuto_hangup_debug) {
        DIMINUTO_LOG_DEBUG("diminuto_hangup_signal: SIGHUP");
    } else {
        /* Do nothing. */
    }

    return rc;
}

static void diminuto_hangup_handler(int signum)
{
    int mysignaled = -1;
    static const int MAXIMUM = ~(((int)1) << ((sizeof(signaled) * 8) - 1));

    if (signum == SIGHUP) {
        mysignaled = signaled;
        if (mysignaled < MAXIMUM) {
            mysignaled += 1;
            signaled = mysignaled;
        }
    }

}

int diminuto_hangup_check(void)
{
    int mysignaled = -1;

    DIMINUTO_CRITICAL_SECTION_BEGIN(&mutex);
        DIMINUTO_UNINTERRUPTIBLE_SECTION_BEGIN(SIGHUP);

            mysignaled = signaled;
            signaled = 0;

        DIMINUTO_UNINTERRUPTIBLE_SECTION_END;
    DIMINUTO_CRITICAL_SECTION_END;

    if (!mysignaled) {
        /* Do nothing. */
    } else if (diminuto_hangup_debug) {
        DIMINUTO_LOG_DEBUG("diminuto_hangup_check: SIGHUP");
    } else {
        /* Do nothing. */
    }

    return mysignaled;
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
    } else if (diminuto_hangup_debug) {
        DIMINUTO_LOG_DEBUG("diminuto_hangup_install: SIGHUP");
    } else {
        /* Do nothing. */
    }

    return 0;
}
