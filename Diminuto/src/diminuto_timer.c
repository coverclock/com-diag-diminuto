/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2009-2018 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in LICENSE.txt<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 */

#include "com/diag/diminuto/diminuto_timer.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_frequency.h"
#include "com/diag/diminuto/diminuto_criticalsection.h"
#include "../src/diminuto_timer.h"
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

static void proxy(union sigval sv)
{
    diminuto_timer_t * tp = (diminuto_timer_t *)0;

    tp = (diminuto_timer_t *)sv.sival_ptr;
    tp->value = (*(tp->function))(tp->context);
}

diminuto_timer_t * diminuto_timer_init_generic(diminuto_timer_t * tp, int periodic, diminuto_timer_function_t * fp, int signum)
{
    diminuto_timer_t * result = (diminuto_timer_t *)0;
    int rc = -1;

    do {

        memset(tp, 0, sizeof(*tp));

        tp->periodic = periodic;

        if ((rc = pthread_attr_init(&(tp->attributes))) != 0) {
            errno = rc;
            diminuto_perror("diminuto_timer_init: pthread_attr_init");
            break;
        }

        if ((rc = pthread_attr_setschedpolicy(&(tp->attributes), SCHED_FIFO)) != 0) {
            errno = rc;
            diminuto_perror("diminuto_timer_init: pthread_attr_setsched_policy");
            break;
        }

        if ((tp->param.sched_priority = sched_get_priority_max(SCHED_FIFO)) < 0) {
            diminuto_perror("diminuto_timer_init: sched_get_priority_max");
            break;
        }

        if ((rc = pthread_attr_setschedparam(&(tp->attributes), &(tp->param))) != 0) {
            errno = rc;
            diminuto_perror("diminuto_timer_init: pthread_attr_setschedparam");
            break;
        }

        if ((fp != (diminuto_timer_function_t *)0) && (signum > 0)) {
            errno = EINVAL;
            diminuto_perror("diminuto_timer_init: function and signal");
            break;
        } else if (fp != (diminuto_timer_function_t *)0) {
            tp->function = fp;
            tp->context = (void *)0;
            tp->value = (void *)0;
            tp->event.sigev_notify = SIGEV_THREAD;
            tp->event.sigev_value.sival_ptr = (void *)tp;
            tp->event.sigev_notify_function = proxy;
            tp->event.sigev_notify_attributes = &(tp->attributes);
        } else if (signum > 0) {
            tp->event.sigev_notify = SIGEV_SIGNAL;
            tp->event.sigev_signo = signum;
        } else {
            errno = EINVAL;
            diminuto_perror("diminuto_timer_init: function or signal");
            break;
        }

        if ((rc = timer_create(CLOCK_MONOTONIC, &(tp->event), &(tp->timer))) < 0) {
            diminuto_perror("diminuto_timer_init: timer_create");
            break;
        }

        result = tp;

    } while (0);

    return result;
}

diminuto_timer_t * diminuto_timer_fini(diminuto_timer_t * tp)
{
    diminuto_timer_t * result = tp;
    int rc = -1;

    if ((rc = timer_delete(tp->timer)) < 0) {
        diminuto_perror("diminuto_timer_fini: timer_delete");
    } else if ((rc = pthread_attr_destroy(&(tp->attributes))) != 0) {
        errno = rc;
        diminuto_perror("diminuto_timer_fini: pthread_attr_init");
    } else {
        result = (diminuto_timer_t *)0;
    }

    return result;
}

diminuto_sticks_t diminuto_timer_start(diminuto_timer_t * tp, diminuto_ticks_t ticks, void * cp)
{
    diminuto_sticks_t sticks = -1;

    tp->context = cp;

    tp->current.it_value.tv_sec = diminuto_frequency_ticks2wholeseconds(ticks);
    tp->current.it_value.tv_nsec = diminuto_frequency_ticks2fractionalseconds(ticks, diminuto_timer_frequency());

    if (tp->periodic) {
        tp->current.it_interval = tp->current.it_value;
    } else {
        tp->current.it_interval.tv_sec = 0;
        tp->current.it_interval.tv_nsec = 0;
    }

    tp->remaining = tp->current;

    if (timer_settime(tp->timer, 0, &(tp->current), &(tp->remaining)) < 0) {
        diminuto_perror("diminuto_timer_start: timer_settime");
    } else {
        sticks = diminuto_frequency_seconds2ticks(tp->remaining.it_value.tv_sec, tp->remaining.it_value.tv_nsec, diminuto_timer_frequency());
    }

    return sticks;
}

diminuto_sticks_t diminuto_timer_stop(diminuto_timer_t * tp)
{
    diminuto_sticks_t sticks = -1;

#if 0
    if (timer_gettime(tp->timer, &(tp->remaining)) < 0) {
        diminuto_perror("diminuto_timer_stop: timer_gettime");
    }
#endif

    tp->current.it_value.tv_sec = 0;
    tp->current.it_value.tv_nsec = 0;

    if (timer_settime(tp->timer, 0, &(tp->current), &(tp->remaining)) < 0) {
        diminuto_perror("diminuto_timer_stop: timer_settime");
    } else {
        sticks = diminuto_frequency_seconds2ticks(tp->remaining.it_value.tv_sec, tp->remaining.it_value.tv_nsec, diminuto_timer_frequency());
    }

    return sticks;
}

/*******************************************************************************
 * As much as possible the code below mimics the semantics of setitimer(2)
 * but uses a monotonic POSIX real-time timer to do so. The timer is a
 * singleton and sends a SIGALRM to the calling process, just like setitimer(2).
 ******************************************************************************/

diminuto_sticks_t diminuto_timer_setitimer(diminuto_ticks_t ticks, int periodic)
{
    diminuto_sticks_t sticks = -1;
    static int initialized = 0;
    static int running = 0;
    static diminuto_timer_t singleton;
    static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

    DIMINUTO_CRITICAL_SECTION_BEGIN(&mutex);

        do {

            if (initialized) {
                /* Do nothing. */
            } else if (diminuto_timer_init_generic(&singleton, periodic, (diminuto_timer_function_t *)0, SIGALRM) != (diminuto_timer_t *)0) {
                initialized = !0;
            } else {
                break;
            }

            if (ticks > 0) {
                sticks = diminuto_timer_start(&singleton, ticks, (void *)0);
            } else {
                sticks = diminuto_timer_stop(&singleton);
            }

        } while (0);

    DIMINUTO_CRITICAL_SECTION_END;

    return sticks;
}
