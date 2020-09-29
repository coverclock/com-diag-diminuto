/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2020 Digital Aggregates Corporation, Colorado, USA.<BR>
 * Licensed under the terms in LICENSE.txt.<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 */

#include <errno.h>
#include "com/diag/diminuto/diminuto_condition.h"
#include "com/diag/diminuto/diminuto_thread.h"
#include "com/diag/diminuto/diminuto_frequency.h"
#include "com/diag/diminuto/diminuto_log.h"

diminuto_condition_t * diminuto_condition_init(diminuto_condition_t * cp)
{
    diminuto_mutex_init(&(cp->mutex));
    pthread_cond_init(&(cp->condition), (pthread_condattr_t *)0);
    return cp;
}

diminuto_condition_t * diminuto_condition_fini(diminuto_condition_t * cp)
{
    int rc = EIO;

    if ((rc = pthread_cond_broadcast(&(cp->condition))) != 0) {
        errno = rc;
        diminuto_perror("diminuto_condition_fini: pthread_cond_broadcast");
    }
    diminuto_thread_yield();
    pthread_cond_destroy(&(cp->condition));
    diminuto_mutex_fini(&(cp->mutex));

    return (diminuto_condition_t *)0;
}

void diminuto_condition_cleanup(void * vp)
{
    diminuto_condition_t * cp = (diminuto_condition_t *)vp;

    diminuto_condition_unlock(cp);
}

int diminuto_condition_wait_until(diminuto_condition_t * cp, diminuto_ticks_t clocktime)
{
    int rc = EIO;
    struct timespec later = { 0, };
    static const diminuto_ticks_t NANOSECONDS = 1000000000;

    if (clocktime == DIMINUTO_CONDITION_INFINITY) {
        if ((rc = pthread_cond_wait(&(cp->condition), &(cp->mutex.mutex))) != 0) {
            errno = rc;
            diminuto_perror("diminuto_condition_wait_until: pthread_cond_wait");
        }
    } else {
        later.tv_sec = diminuto_frequency_ticks2wholeseconds(clocktime);
        later.tv_nsec = diminuto_frequency_ticks2fractionalseconds(clocktime, NANOSECONDS);
        if ((rc = pthread_cond_timedwait(&(cp->condition), &(cp->mutex.mutex), &later)) == 0) {
            /* Do nothing. */
        } else if (rc == DIMINUTO_CONDITION_TIMEDOUT) {
            /* Do nothing. */
        } else {
            errno = rc;
            diminuto_perror("diminuto_condition_wait_until: pthread_cond_timedwait");
        }
    }

    return rc;
}

int diminuto_condition_signal(diminuto_condition_t * cp)
{
    int rc = EIO;

    if ((rc = pthread_cond_broadcast(&(cp->condition))) != 0) {
        errno = rc;
        diminuto_perror("diminuto_condition_signal: pthread_cond_broadcast");
    } else {
        diminuto_thread_yield();
    }

    return rc;
}
