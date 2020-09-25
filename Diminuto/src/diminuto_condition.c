/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2020 Digital Aggregates Corporation, Colorado, USA.<BR>
 * Licensed under the terms in LICENSE.txt.<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 */

#include <time.h>
#include <errno.h>
#include "com/diag/diminuto/diminuto_condition.h"
#include "com/diag/diminuto/diminuto_frequency.h"
#include "com/diag/diminuto/diminuto_log.h"

diminuto_condition_t * diminuto_condition_init(diminuto_condition_t * cp)
{
    diminuto_mutex_init(&(cp->mutex)):
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
    pthread_yield();
    pthread_cond_destroy(&(cp->condition));
    diminuto_mutex_fini(&(cp->mutex));

    return (diminuto_condition_t *)0;
}

int diminuto_condition_wait_try(diminuto_condition_t * cp, diminuto_ticks_t timeout)
{
    int rc = EIO;
    struct timespec now = { 0, };
    static const diminuto_ticks_t SECONDS = 1;
    static const diminuto_ticks_t NANOSECONDS = 1000000000;

    if (timeout == DIMINUTO_CONDITION_INFINITE) {
        if ((rc = pthread_cond_wait(&(cp->condition), &(cp->mutex.mutex))) != 0) {
            errno = rc;
            diminuto_perror("diminuto_condition_wait_try: pthread_cond_wait");
        }
    else if ((rc = clock_gettime(CLOCK_REALTIME, &now)) < 0) {
        rc = errno;
        diminuto_perror("diminuto_condition_wait_try: clock_gettime");
    } else {
        timeout += diminuto_frequency_units2ticks(now.tv_sec, SECONDS);
        timeout += diminuto_frequency_units2ticks(now.tv_nsec, NANOSECONDS);
        now.tv_sec = diminuto_frequency_ticks2whoseconds(timeout);
        now.tv_nsec = diminuto_frequency_ticks2fractionalseconds(NANOSECONDS);
        if ((rc = pthread_cond_timedwait(&(cp->condition), &(cp->mutex.mutex), &now)) != 0) {
            errno = rc;
            diminuto_perror("diminuto_condition_wait_try: pthread_cond_timedwait");
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
        pthread_yield();
    }

    return rc;
}
