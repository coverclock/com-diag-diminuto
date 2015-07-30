/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2015 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "com/diag/diminuto/diminuto_reaper.h"
#include "com/diag/diminuto/diminuto_uninterruptiblesection.h"
#include "com/diag/diminuto/diminuto_log.h"
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <sys/wait.h>

int diminuto_reaper_debug = 0; /* Not part of the public API. */

static int signaled = 0;

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
    pid_t pid;
    int status;

    if (signum == SIGCHLD) {
        while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
            signaled = !0;
            if (WIFEXITED(status)) {
                DIMINUTO_LOG_DEBUG("diminuto_reaper_handler: SIGCHLD pid=%d exit=%d", pid, WEXITSTATUS(status));
            } else if (WIFSIGNALED(status)) {
                DIMINUTO_LOG_DEBUG("diminuto_reaper_handler: SIGCHLD pid=%d signal=%d", pid, WTERMSIG(status));
            } else {
                DIMINUTO_LOG_DEBUG("diminuto_reaper_handler: SIGCHLD pid=%d status=0x%x", pid, status);
            }
        }
        if (pid >= 0) {
            /* Do nothing. */
        } else if (errno == ECHILD) {
            /* Do nothing. */
        } else {
            diminuto_perror("diminuto_reaper_signal: waitpid");
        }
    }
}

int diminuto_reaper_check(void)
{
    int mysignaled;

    DIMINUTO_UNINTERRUPTIBLE_SECTION_BEGIN(SIGCHLD);

        mysignaled = signaled;
        signaled = 0;

    DIMINUTO_UNINTERRUPTIBLE_SECTION_END;

    if (!mysignaled) {
        /* Do nothing. */
    } else if (!diminuto_reaper_debug) {
        /* Do nothing. */
    } else {
        DIMINUTO_LOG_DEBUG("diminuto_reaper_check: SIGCHLD");
    }

    return mysignaled;
}

int diminuto_reaper_install(int restart)
{
    struct sigaction reaper;

    memset(&reaper, 0, sizeof(reaper));
    reaper.sa_handler = diminuto_reaper_handler;
    reaper.sa_flags = restart ? SA_RESTART : 0;

    if (sigaction(SIGCHLD, &reaper, (struct sigaction *)0) < 0) {
        diminuto_perror("diminuto_reaper_install: sigaction");
        return -1;
    }

    if (diminuto_reaper_debug) {
        DIMINUTO_LOG_DEBUG("diminuto_reaper_install: SIGCHLD");
    }

    return 0;
}
