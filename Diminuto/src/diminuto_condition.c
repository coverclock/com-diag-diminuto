/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2020 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is the implementation of the Condition feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is the implementation of the Condition feature.
 */

/***********************************************************************
 * PREREQUISITES
 **********************************************************************/

#include "com/diag/diminuto/diminuto_condition.h"
#include "com/diag/diminuto/diminuto_frequency.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_thread.h"
#include <errno.h>
#include <string.h>

/***********************************************************************
 * INITIALIZERS AND FINALIZERS
 **********************************************************************/

diminuto_condition_t * diminuto_condition_init(diminuto_condition_t * cp)
{
    diminuto_condition_t * result = (diminuto_condition_t *)0;
    int rc = DIMINUTO_CONDITION_ERROR;

    memset(cp, 0, sizeof(*cp));

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
    int rc = DIMINUTO_CONDITION_ERROR;

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
    int rc = DIMINUTO_CONDITION_ERROR;
    struct timespec later = { 0, };

    if (clocktime == DIMINUTO_CONDITION_INFINITY) {
        if ((rc = pthread_cond_wait(&(cp->condition), &(cp->mutex.mutex))) != 0) {
            errno = rc;
            diminuto_perror("diminuto_condition_wait_until: pthread_cond_wait");
        }
    } else {
        later.tv_sec = diminuto_frequency_ticks2wholeseconds(clocktime);
        later.tv_nsec = diminuto_frequency_ticks2fractionalseconds(clocktime, 1000000000LL);
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
    int rc = DIMINUTO_CONDITION_ERROR;

    if ((rc = pthread_cond_broadcast(&(cp->condition))) != 0) {
        errno = rc;
        diminuto_perror("diminuto_condition_signal: pthread_cond_broadcast");
    } else {
        /*
         * Give any woken threads the chance to reevaluate their
         * condition variables and determine if they need to wait
         * again.
         */
        diminuto_thread_yield();
    }

    return rc;
}
