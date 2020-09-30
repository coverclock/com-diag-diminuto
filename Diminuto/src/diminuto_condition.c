/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2020 Digital Aggregates Corporation, Colorado, USA.<BR>
 * Licensed under the terms in LICENSE.txt.<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 * This module creates a framework the implements POSIX condition
 * operations using a very specific model of behavior: all conditions
 * contain a dedicated mutex that is used to access them (this may
 * not be appropriate for all algorithms).
 */

/***********************************************************************
 * PREREQUISITES
 **********************************************************************/

#include <errno.h>
#include "com/diag/diminuto/diminuto_condition.h"
#include "com/diag/diminuto/diminuto_thread.h"
#include "com/diag/diminuto/diminuto_frequency.h"
#include "com/diag/diminuto/diminuto_log.h"

/***********************************************************************
 * INITIALIZERS AND FINALIZERS
 **********************************************************************/

diminuto_condition_t * diminuto_condition_init(diminuto_condition_t * cp)
{
    diminuto_condition_t * result = (diminuto_condition_t *)0;
    int rc = EIO;

    if (diminuto_mutex_init(&(cp->mutex)) != &(cp->mutex)) {
        /* Do nothing. */
    } else if ((rc = pthread_cond_init(&(cp->condition), (pthread_condattr_t *)0)) != 0) {
        errno = rc;
        diminuto_perror("diminution_condition_init: pthread_cond_init");
    } else {
        result = cp;
    }

    return result;
}

diminuto_condition_t * diminuto_condition_fini(diminuto_condition_t * cp)
{
    diminuto_condition_t * result = cp;
    int rc = EIO;

    if ((rc = pthread_cond_broadcast(&(cp->condition))) != 0) {
        errno = rc;
        diminuto_perror("diminuto_condition_fini: pthread_cond_broadcast");
    } else {
        (void)diminuto_thread_yield();
        if ((rc = pthread_cond_destroy(&(cp->condition))) != 0) {
            errno = rc;
            diminuto_perror("diminuto_condition_fini: pthread_cond_destroy");
        } else if (diminuto_mutex_fini(&(cp->mutex)) != (diminuto_mutex_t *)0) {
            /* Do nothing. */
        } else {
            result = (diminuto_condition_t *)0;
        }
    }

    return result;
}

/***********************************************************************
 * OPERATIONS
 **********************************************************************/

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

/***********************************************************************
 * CALLBACKS
 **********************************************************************/

void diminuto_condition_cleanup(void * vp)
{
    diminuto_condition_t * cp = (diminuto_condition_t *)vp;

    diminuto_condition_unlock(cp);
}
