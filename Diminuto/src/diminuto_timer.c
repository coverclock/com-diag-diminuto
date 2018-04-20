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
#include <signal.h>
#include <time.h>
#include <pthread.h>

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
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
	diminuto_sticks_t sticks = 0;
    struct sigevent event = { 0 };
    struct itimerspec timer = { 0 };
    struct itimerspec remaining = { 0 };

    /*
     * If we don't have a timer, and are not deleting it, create the timer.
     */

    event.sigev_notify = SIGEV_SIGNAL;
	event.sigev_signo = SIGALRM;

    DIMINUTO_CRITICAL_SECTION_BEGIN(&mutex);

		if (initialized) {
			/* Do nothing: already have the timer. */
		} else if (ticks == 0) {
			/* Do nothing: deleting the timer. */
		} else if (timer_create(CLOCK_MONOTONIC, &event, &timerid) < 0) {
			diminuto_perror("diminuto_ptimer: timer_create");
			sticks = (diminuto_sticks_t)-1;
		} else {
			initialized = !0;
		}

    DIMINUTO_CRITICAL_SECTION_END;

    if (sticks < 0) {
    	return sticks;
    }

    /*
     * If we have a timer, and are deleting it, delete the timer.
     */

    DIMINUTO_CRITICAL_SECTION_BEGIN(&mutex);

		if (ticks != 0) {
			/* Do nothing: not deleting the timer. */
		} else if (!initialized) {
			/* Do nothing: do not have the timer. */
		} else if (timer_gettime(timerid, &remaining) < 0) {
			diminuto_perror("diminuto_ptimer: timer_gettime");
			sticks = (diminuto_sticks_t)-1;
		 } else if (timer_delete(timerid) < 0) {
			diminuto_perror("diminuto_ptimer: timer_delete");
			sticks = (diminuto_sticks_t)-1;
		} else {
			sticks = diminuto_frequency_seconds2ticks(remaining.it_value.tv_sec, remaining.it_value.tv_nsec, diminuto_timer_frequency());
			initialized = 0;
		}

    DIMINUTO_CRITICAL_SECTION_END;

    if ((sticks < 0) || (!initialized)) {
    	return sticks;
    }

    /*
     * If we are not deleting the timer, then set the timer.
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
        sticks = (diminuto_sticks_t)-1;
    } else {
    	sticks = diminuto_frequency_seconds2ticks(remaining.it_value.tv_sec, remaining.it_value.tv_nsec, diminuto_timer_frequency());
    }

    return sticks;
}
