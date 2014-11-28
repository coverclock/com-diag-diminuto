/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_TIMER_
#define _H_COM_DIAG_DIMINUTO_TIMER_

/**
 * @file
 *
 * Copyright 2009 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "com/diag/diminuto/diminuto_types.h"

#define COM_DIAG_DIMINUTO_TIMER_FREQUENCY (1000000LL)

/**
 * Return the resolution of the Diminuto timer units in ticks per second
 * (Hertz). Timer intervals smaller than the equivalent period in ticks will
 * not yield the expected results. In fact, there is no guarantee that the
 * underlying software platform or hardware target can support timer intervals
 * with even this resolution.
 * @return the resolution in ticks per second.
 */
static inline diminuto_ticks_t diminuto_timer_frequency(void)
{
	return COM_DIAG_DIMINUTO_TIMER_FREQUENCY;
}

/**
 * Start a timer for the specified number of ticks. When the timer
 * expires, the calling process will receive a SIGALRM. If a timer is already
 * running, the amount of time left in the prior timer will be returned.
 * The actual timer duration will be approximate depending on the
 * granularity of the system clock and latency in the implementation.
 * The timer fires only once per call. Calling with zero ticks
 * cancels any prior timer.
 * @param ticks is the desired timer duration in ticks.
 * @return the number of ticks remaining in prior timer if a timer was already
 * running.
 */
extern diminuto_ticks_t diminuto_timer_oneshot(diminuto_ticks_t ticks);

/**
 * Start an periodic timer for the specified number of ticks. When
 * the timer expires, the calling process will receive a SIGALRM. If a
 * timer is already running, the amount of time left in the prior timer
 * will be returned. The actual timer period will be approximate depending
 * on the granularity of the system clock and latency in the implementation.
 * The timer fires repeatedly with a periodicity of the specified ticks
 * until it is cancelled. Calling with zero ticks cancels the timer.
 * @param ticks is the desired period interval in ticks.
 * @return the number of ticks remaining in the prior timer if a timer was
 * already running.
 */
extern diminuto_ticks_t diminuto_timer_periodic(diminuto_ticks_t ticks);

#endif
