/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2015-2022 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is the implementation of the Reaper feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is the implementation of the Reaper feature.
 */

#include "com/diag/diminuto/diminuto_reaper.h"
#include "com/diag/diminuto/diminuto_uninterruptiblesection.h"
#include "com/diag/diminuto/diminuto_criticalsection.h"
#include "com/diag/diminuto/diminuto_log.h"
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>

int diminuto_reaper_debug = 0; /* Not part of the public API. */

static volatile int signaled = 0;
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int diminuto_reaper_signal(pid_t pid)
{
    int rc = 0;

    if (kill(pid, SIGCHLD) < 0) {
        diminuto_perror("diminuto_reaper_signal: kill");
        rc = -1;
    } else if (diminuto_reaper_debug) {
        DIMINUTO_LOG_DEBUG("diminuto_reaper_signal: SIGCHLD");
    } else {
        /* Do nothing. */
    }

    return rc;
}

static void diminuto_reaper_handler(int signum)
{
    int mysignaled = -1;
    static const int MAXIMUM = ~(((int)1) << ((sizeof(signaled) * 8) - 1));

    if (signum == SIGCHLD) {
        mysignaled = signaled;
        if (mysignaled < MAXIMUM) {
            mysignaled += 1;
            signaled = mysignaled;
        }
    }

}

int diminuto_reaper_check(void)
{
    int mysignaled = -1;

    DIMINUTO_CRITICAL_SECTION_BEGIN(&mutex);
        DIMINUTO_UNINTERRUPTIBLE_SECTION_BEGIN(SIGCHLD);

            mysignaled = signaled;
            signaled = 0;

        DIMINUTO_UNINTERRUPTIBLE_SECTION_END;
    DIMINUTO_CRITICAL_SECTION_END;

    if (!mysignaled) {
        /* Do nothing. */
    } else if (diminuto_reaper_debug) {
        DIMINUTO_LOG_DEBUG("diminuto_reaper_check: SIGCHLD [%d]", mysignaled);
   } else {
        /* Do nothing. */
    }

    return mysignaled;
}

pid_t diminuto_reaper_reap_generic(pid_t pids, int * statusp, int flag)
{
    pid_t pid = -1;
    int status = -1;

    pid = waitpid(pids, &status, flag);
    if ((pid < 0) && (errno == ECHILD)) {
        pid = 0; /* SHould never happen, but does. */
    } else if (pid < 0) {
        diminuto_perror("diminuto_reaper_reap_generic: waitpid");
    } else if (pid == 0) {
        /* Do nothing. */
    } else if (diminuto_reaper_debug) {
        DIMINUTO_LOG_DEBUG("diminuto_reaper_reap_generic: waitpid [%d] (%d)", pid, status);
    } else {
        /* Do nothing. */
    }

    if (pid <= 0) {
        /* Do nothing. */
    } else if (statusp == (int *)0) {
        /* Do nothing. */
    } else {
        *statusp = status;
    }

    return pid;
}

int diminuto_reaper_install(int restart)
{
    int rc = 0;
    struct sigaction reaper = { 0 };

    reaper.sa_handler = diminuto_reaper_handler;
    reaper.sa_flags = restart ? SA_RESTART : 0;

    if (sigaction(SIGCHLD, &reaper, (struct sigaction *)0) < 0) {
        diminuto_perror("diminuto_reaper_install: sigaction");
        rc = -1;
    } else if (diminuto_reaper_debug) {
        DIMINUTO_LOG_DEBUG("diminuto_reaper_install: SIGCHLD");
    } else {
        /* Do nothing. */
    }

    return rc;
}
