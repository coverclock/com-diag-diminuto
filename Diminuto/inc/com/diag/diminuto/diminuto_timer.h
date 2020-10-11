/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_TIMER_
#define _H_COM_DIAG_DIMINUTO_TIMER_

/**
 * @file
 *
 * Copyright 2009-2018 Digital Aggregates Corporation, Colorado, USA.
 * Licensed under the terms in LICENSE.txt.
 *
 * The Timer feature uses the real-time POSIX timer_create(2) and
 * timer_settime(2) facilites to mimic the setitimer(2) semantics, but
 * with a monotonic clock. The resulting one-shot or periodic timer is,
 * like setitimer(2), a singleton.
 */

#include "com/diag/diminuto/diminuto_types.h"
#include <pthread.h>
#include <signal.h>
#include <time.h>
#include <sched.h>

/**
 * @def COM_DIAG_DIMINUTO_TIMER_FREQUENCY
 * This manifest constant is the frequency in Hertz at which this feature
 * operates. The inverse of this value is the smallest unit of time in fractions
 * of a second that this feature can express or use. This constant is provided
 * for use in those cases where it is useful to have the value at compile time.
 * However, you should always prefer to use the inline function when possible.
 */
#define COM_DIAG_DIMINUTO_TIMER_FREQUENCY (1000000000LL)

/**
 * Return the resolution of the Diminuto timer units in ticks per second
 * (Hertz). Timer intervals smaller than the equivalent period in ticks will
 * not yield the expected results. In fact, there is no guarantee that the
 * underlying software platform or hardware target can support timer intervals
 * with even this resolution.
 * @return the resolution in ticks per second.
 */
static inline diminuto_sticks_t diminuto_timer_frequency(void)
{
    return COM_DIAG_DIMINUTO_TIMER_FREQUENCY;
}

typedef void * (diminuto_timer_function_t)(void *);

typedef struct DiminutoTimer {
    diminuto_ticks_t ticks;
    struct sigevent event;
    struct sched_param param;
    pthread_attr_t attributes;
    struct itimerspec current;
    struct itimerspec remaining;
    int periodic;
    timer_t timer;
    diminuto_timer_function_t * function;
    void * context;
    void * value;
} diminuto_timer_t;

extern diminuto_timer_t * diminuto_timer_init(diminuto_timer_t * tp, int periodic, diminuto_timer_function_t * fp, int signum);

extern diminuto_timer_t * diminuto_timer_fini(diminuto_timer_t * tp);

extern diminuto_sticks_t diminuto_timer_start(diminuto_timer_t * tp, diminuto_ticks_t ticks, void * cp);

extern diminuto_sticks_t diminuto_timer_stop(diminuto_timer_t * tp);

/**
 * Start a singleton one-shot timer for the specified number of ticks. When the
 * timer expires, the calling process will receive a SIGALRM. If a timer is
 * already running, the amount of time left in the prior timer will be returned.
 * The actual timer duration will be approximate depending on the
 * granularity of the system clock and latency in the implementation.
 * The timer fires only once per call. Calling with zero ticks
 * cancels any prior timer.
 * N.B. This implementation uses a static variable and so is not thread safe.
 * @param ticks is the desired timer duration in ticks.
 * @return the number of ticks remaining in prior timer if a timer was already
 * running, or -1 if an error occurred.
 */
static inline diminuto_sticks_t diminuto_timer_oneshot(diminuto_ticks_t ticks)
{
    extern diminuto_sticks_t diminuto_timer_setitimer(diminuto_ticks_t ticks, int periodic);
    return diminuto_timer_setitimer(ticks, 0);
}

/**
 * Start a singleton periodic timer for the specified number of ticks. When
 * the timer expires, the calling process will receive a SIGALRM. If a
 * timer is already running, the amount of time left in the prior timer
 * will be returned. The actual timer period will be approximate depending
 * on the granularity of the system clock and latency in the implementation.
 * The timer fires repeatedly with a periodicity of the specified ticks
 * until it is cancelled. Calling with zero ticks cancels the timer.
 * N.B. This implementation uses a static variable and so is not thread safe.
 * @param ticks is the desired period interval in ticks.
 * @return the number of ticks remaining in the prior timer if a timer was
 * already running, or -1 if an error occurred.
 */
static inline diminuto_sticks_t diminuto_timer_periodic(diminuto_ticks_t ticks)
{
    extern diminuto_sticks_t diminuto_timer_setitimer(diminuto_ticks_t ticks, int periodic);
    return diminuto_timer_setitimer(ticks, !0);
}

#endif
