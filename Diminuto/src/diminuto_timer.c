/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2009-2024 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is the implementation of the Timer feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is the implementation of the Timer feature.
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
#include "com/diag/diminuto/diminuto_coherentsection.h"
#include "com/diag/diminuto/diminuto_criticalsection.h"
#include "com/diag/diminuto/diminuto_delay.h"
#include "com/diag/diminuto/diminuto_frequency.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_policy.h"
#include "diminuto_timer.h"
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/**
 * This is the scheduling policy used for Diminuto timers.
 */
static const int DIMINUTO_TIMER_SCHEDULER = DIMINUTO_POLICY_SCHEDULER_TIMER;

/**
 * This is the maximum scheduling priority for Diminuto timers.
 */
static const int DIMINUTO_TIMER_PRIORITY = DIMINUTO_POLICY_PRIORITY_TIMER;

/**
 * This is the kind of clock Diminuto timers use.
 */
static const clockid_t DIMINUTO_TIMER_CLOCK = (clockid_t)DIMINUTO_POLICY_CLOCK_TIMER;

static void proxy(union sigval sv)
{
    diminuto_timer_t * tp = (diminuto_timer_t *)0;
    diminuto_timer_function_t * function = (diminuto_timer_function_t *)0;
    void * context = (void *)0;
    void * value = (void *)0;
    int rc = 0;

    if ((tp = (diminuto_timer_t *)sv.sival_ptr) != (diminuto_timer_t *)0) {

        DIMINUTO_CONDITION_BEGIN(&(tp->condition));

            if (tp->state == DIMINUTO_TIMER_STATE_ARM) {
                function = tp->function;
                context = tp->context;
            }
    
        DIMINUTO_CONDITION_END;

        /*
         * The user-specified timer function may call the
         * timer stop function, which will re-enter the
         * critical section.
         */

        if (function != (diminuto_timer_function_t *)0) {
            value = (*function)(context);
        }

        DIMINUTO_CONDITION_BEGIN(&(tp->condition));

            if (tp->function != (diminuto_timer_function_t *)0) {
                tp->value = value;
            }

            if (tp->periodic) {
                if (tp->state == DIMINUTO_TIMER_STATE_DISARM) {
                    tp->state = DIMINUTO_TIMER_STATE_IDLE;
                    rc = diminuto_condition_signal(&(tp->condition));
                    if (rc != 0) { tp->error = rc; }
                }
            } else {
                if (tp->state != DIMINUTO_TIMER_STATE_IDLE) {
                    tp->state = DIMINUTO_TIMER_STATE_IDLE;
                    rc = diminuto_condition_signal(&(tp->condition));
                    if (rc != 0) { tp->error = rc; }
                }
            }
    
        DIMINUTO_CONDITION_END;

    }
}

int diminuto_timer_error(diminuto_timer_t * tp)
{
    int result = -1;

    DIMINUTO_CONDITION_BEGIN(&(tp->condition));
        result = tp->error;
    DIMINUTO_CONDITION_END;

    return result;
}

diminuto_timer_state_t diminuto_timer_state(diminuto_timer_t * tp)
{
    diminuto_timer_state_t result = DIMINUTO_TIMER_STATE_UNKNOWN;

    DIMINUTO_CONDITION_BEGIN(&(tp->condition));
        result = tp->state;
    DIMINUTO_CONDITION_END;

    return result;
}

diminuto_timer_t * diminuto_timer_init_generic(diminuto_timer_t * tp, int periodic, diminuto_timer_function_t * fp, int signum)
{
    diminuto_timer_t * result = (diminuto_timer_t *)0;
    int rc = -1;
    int limit = 0;

    do {

        memset(tp, 0, sizeof(*tp));

        tp->ticks = 0;
        tp->periodic = !!periodic;
        tp->state = DIMINUTO_TIMER_STATE_IDLE;
        tp->function = (diminuto_timer_function_t *)0;
        tp->context = (void *)0;
        tp->value = (void *)0;
        tp->error = 0;

        if ((rc = pthread_attr_init(&(tp->attributes))) != 0) {
            errno = rc;
            diminuto_perror("diminuto_timer_init: pthread_attr_init");
            break;
        }

        if ((rc = pthread_attr_setinheritsched(&(tp->attributes), PTHREAD_EXPLICIT_SCHED)) != 0) {
            errno = rc;
            diminuto_perror("diminuto_timer_init: pthread_attr_setinheritsched");
            break;
        }

        if ((rc = pthread_attr_setschedpolicy(&(tp->attributes), DIMINUTO_TIMER_SCHEDULER)) != 0) {
            errno = rc;
            diminuto_perror("diminuto_timer_init: pthread_attr_setsched_policy");
            break;
        }

        tp->parameters.sched_priority = DIMINUTO_TIMER_PRIORITY;
        if ((limit = sched_get_priority_min(DIMINUTO_TIMER_SCHEDULER)) < 0) {
            diminuto_perror("diminuto_timer_init: sched_get_priority_min");
            break;
        }
        if (tp->parameters.sched_priority < limit) {
            tp->parameters.sched_priority = limit;
        }
        if ((limit = sched_get_priority_max(DIMINUTO_TIMER_SCHEDULER)) < 0) {
            diminuto_perror("diminuto_timer_init: sched_get_priority_max");
            break;
        }
        if (tp->parameters.sched_priority > limit) {
            tp->parameters.sched_priority = limit;
        }

        if ((rc = pthread_attr_setschedparam(&(tp->attributes), &(tp->parameters))) != 0) {
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

        if ((rc = timer_create(DIMINUTO_TIMER_CLOCK, &(tp->event), &(tp->timer))) < 0) {
            diminuto_perror("diminuto_timer_init: timer_create");
            break;
        }

        result = tp;

    } while (0);

    return result;
}

diminuto_timer_t * diminuto_timer_fini(diminuto_timer_t * tp)
{
    diminuto_timer_t * result = (diminuto_timer_t *)0;
    int rc = -1;

    if (timer_delete(tp->timer) < 0) {
        diminuto_perror("diminuto_timer_fini: timer_delete");
        result = tp;
    }

    if ((rc = pthread_attr_destroy(&(tp->attributes))) != 0) {
        errno = rc;
        diminuto_perror("diminuto_timer_fini: pthread_attr_init");
        result = tp;
    }

    if (diminuto_condition_fini(&(tp->condition)) != (diminuto_condition_t *)0) {
        result = tp;
    }

    memset(tp, 0, sizeof(*tp));

    return result;
}

diminuto_sticks_t diminuto_timer_start(diminuto_timer_t * tp, diminuto_ticks_t ticks, void * cp)
{
    diminuto_sticks_t sticks = -1;

    tp->context = cp;
    tp->ticks = ticks;
    tp->state = DIMINUTO_TIMER_STATE_ARM;
    tp->error = 0;

    tp->current.it_value.tv_sec = diminuto_frequency_ticks2wholeseconds(ticks);
    tp->current.it_value.tv_nsec = diminuto_frequency_ticks2fractionalseconds(ticks, 1000000000LL);

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
        sticks = diminuto_frequency_seconds2ticks(tp->remaining.it_value.tv_sec, tp->remaining.it_value.tv_nsec, 1000000000LL);
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
                later = diminuto_condition_clock() + diminuto_timer_window(tp->ticks); /* Clock time, not duration. */
                tp->state = DIMINUTO_TIMER_STATE_DISARM;
                do {
                    if ((rc = diminuto_condition_wait_until(&(tp->condition), later)) == DIMINUTO_CONDITION_TIMEDOUT) {
                        errno = rc;
                        diminuto_perror("diminuto_timer_stop: diminuto_condition_wait_until");
                        break;
                    }
                } while (tp->state != DIMINUTO_TIMER_STATE_IDLE);
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
        sticks = diminuto_frequency_seconds2ticks(tp->remaining.it_value.tv_sec, tp->remaining.it_value.tv_nsec, 1000000000LL);
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
