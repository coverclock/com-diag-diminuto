/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2009-2017 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 */

#include "com/diag/diminuto/diminuto_timer.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_frequency.h"
#include <signal.h>
#include <time.h>

/*
 * This implementation uses timer_settime(2) et al. which is monotonic,
 * but the implementation uses a static variable so is not thread safe.
 */

static int initialized = 0;
static timer_t timerid = 0;

/*
 * Exposed just for unit testing.
 */
void * diminuto_ptimer_get(void)
{
    return initialized ? timerid : (void *)-1;
}

/*
 * As much as possible this mimics the semantics of the setitimer(2) version
 * of this API but uses a monotonic timer to do so.
 */
diminuto_sticks_t diminuto_ptimer(diminuto_ticks_t ticks, int periodic)
{
    struct sigevent event = { 0 };
    struct itimerspec timer = { 0 };
    struct itimerspec remaining = { 0 };

    /*
     * If we don't have a timer, and are not deleting the timer, create
     * the timer.
     */

    if (initialized) {
        /* Do nothing: already have the timer. */
    } else if (ticks == 0) {
        /* Do nothing: deleting the timer. */
    } else {
        event.sigev_notify = SIGEV_SIGNAL;
        event.sigev_signo = SIGALRM;
        if (timer_create(CLOCK_MONOTONIC, &event, &timerid) < 0) {
            diminuto_perror("diminuto_ptimer: timer_create");
            return (diminuto_sticks_t)-1;
        } else {
            initialized = !0;
        }
    }

    /*
     * If we have a timer, and are deleting the timer, delete the
     * timer.
     */

    if (ticks != 0) {
        /* Do nothing: not deleting the timer. */
    } else if (timer_gettime(timerid, &remaining) < 0) {
        diminuto_perror("diminuto_ptimer: timer_gettime");
        return (diminuto_sticks_t)-1;
     } else if (timer_delete(timerid) < 0) {
        diminuto_perror("diminuto_ptimer: timer_delete");
        return (diminuto_sticks_t)-1;
    } else {
        ticks = diminuto_frequency_seconds2ticks(remaining.it_value.tv_sec, remaining.it_value.tv_nsec, diminuto_timer_frequency());
        initialized = 0;
        return (diminuto_sticks_t)ticks;
    }

    /*
     * If we are not deleeting the timer, then set the timer.
     */

    timer.it_value.tv_sec = diminuto_frequency_ticks2wholeseconds(ticks);
    timer.it_value.tv_nsec = diminuto_frequency_ticks2fractionalseconds(ticks, diminuto_timer_frequency());

    if (periodic) {
        timer.it_interval = timer.it_value;
    } else {
        timer.it_interval.tv_sec = 0;
        timer.it_interval.tv_nsec = 0;
    }

    remaining = timer;

    if (timer_settime(timerid, 0, &timer, &remaining) < 0) {
        diminuto_perror("diminuto_ptimer: timer_settime");
        return (diminuto_sticks_t)-1;
    }

    ticks = diminuto_frequency_seconds2ticks(remaining.it_value.tv_sec, remaining.it_value.tv_nsec, diminuto_timer_frequency());

    return (diminuto_sticks_t)ticks;
}
