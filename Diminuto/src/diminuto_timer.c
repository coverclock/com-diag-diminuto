/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2009-2020 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in LICENSE.txt<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 *
 * The condition is necessary because, at least in this POSIX implementation,
 * there is a race condition in which we can already be in the timer
 * callback function as we stop the timer. Deallocating resources
 * (especially if we are next going to call the fini function) while
 * the callback is executing (for example, on another core) cause
 * wackiness (like a segmentation fault) to ensue. I've seen this
 * in the modulator unit test. While I regret having to put a
 * block on a mutex in the timer proxy function, the only time
 * the proxy will find the mutex locked is when we are stopping the
 * timer. We wait for the proxy for longer than timeout value, just
 * to provide some margin.
 */

#include "com/diag/diminuto/diminuto_timer.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_frequency.h"
#include "com/diag/diminuto/diminuto_delay.h"
#include "com/diag/diminuto/diminuto_criticalsection.h"
#include "com/diag/diminuto/diminuto_coherentsection.h"
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

static void proxy(union sigval sv)
{
    diminuto_timer_t * tp = (diminuto_timer_t *)0;

    tp = (diminuto_timer_t *)sv.sival_ptr;

    /*
     * Mutex locking is relatively fast. The only time
     * we will block is when the owner is trying to stop
     * the timer, in which case some delay here is moot.
     */

    DIMINUTO_CONDITION_BEGIN(&(tp->condition));

        if (tp->state == DIMINUTO_TIMER_STATE_DISARM) {
            tp->state = DIMINUTO_TIMER_STATE_IDLE;
            (void)diminuto_condition_signal(&(tp->condition));
        }

    DIMINUTO_CONDITION_END;

    if (tp->state == DIMINUTO_TIMER_STATE_ARM) {
        tp->value = (*(tp->function))(tp->context);
    }

    if (!tp->periodic) {

        /*
         * If we are a one-shot timer, then we might as
         * well stop ourselves, which will prevent the need
         * for the owner to wait.
         */

        DIMINUTO_CONDITION_BEGIN(&(tp->condition));

            if (tp->state != DIMINUTO_TIMER_STATE_IDLE) {
                tp->state = DIMINUTO_TIMER_STATE_IDLE;
                (void)diminuto_condition_signal(&(tp->condition));
            }

        DIMINUTO_CONDITION_END;

     }
}

diminuto_timer_t * diminuto_timer_init_generic(diminuto_timer_t * tp, int periodic, diminuto_timer_function_t * fp, int signum)
{
    diminuto_timer_t * result = (diminuto_timer_t *)0;
    int rc = -1;

    do {

        tp->ticks = 0;
        tp->periodic = periodic;
        tp->state = DIMINUTO_TIMER_STATE_IDLE;
        tp->function = (diminuto_timer_function_t *)0;
        tp->context = (void *)0;
        tp->value = (void *)0;

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

        if (diminuto_condition_init(&(tp->condition)) == (diminuto_condition_t *)0) {
            break;
        }

        if ((fp != (diminuto_timer_function_t *)0) && (signum > 0)) {
            errno = EINVAL;
            diminuto_perror("diminuto_timer_init: function and signal");
            break;
        } else if (fp != (diminuto_timer_function_t *)0) {
            tp->function = fp;
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
    tp->ticks = ticks;
    tp->state = DIMINUTO_TIMER_STATE_ARM;

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
    diminuto_ticks_t later = 0;
    int rc = 0;

    if (tp->function != (diminuto_timer_function_t *)0) {

        DIMINUTO_CONDITION_BEGIN(&(tp->condition));

            if (tp->state == DIMINUTO_TIMER_STATE_ARM) {
                tp->state = DIMINUTO_TIMER_STATE_DISARM;
                later = diminuto_condition_clock() + (tp->ticks * 2); /* Clock time, not duration. */
                while (tp->state != DIMINUTO_TIMER_STATE_IDLE) {
                    if ((rc = diminuto_condition_wait_until(&(tp->condition), later)) == DIMINUTO_CONDITION_TIMEDOUT) {
                        errno = rc;
                        diminuto_perror("diminuto_timer_stop");
                        break;
                    }
                }
            }

        DIMINUTO_CONDITION_END;

    }

    tp->current.it_value.tv_sec = 0;
    tp->current.it_value.tv_nsec = 0;

    if (timer_settime(tp->timer, 0, &(tp->current), &(tp->remaining)) < 0) {
        diminuto_perror("diminuto_timer_stop: timer_settime");
    } else if (rc != 0) {
        /* Do nothing. */
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
