/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2015-2022 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is the implementation of the Terminator feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is the implementation of the Terminator feature.
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

static volatile int signaled = 0;
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

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
    int mysignaled = -1;
    static const int MAXIMUM = ~(((int)1) << ((sizeof(signaled) * 8) - 1));

    if (signum == SIGTERM) {
        mysignaled = signaled;
        if (mysignaled < MAXIMUM) {
            mysignaled += 1;
            signaled = mysignaled;
        }
    }

}

int diminuto_terminator_check(void)
{
    int mysignaled = -1;

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
