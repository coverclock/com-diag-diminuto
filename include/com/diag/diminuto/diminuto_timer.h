/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_TIMER_
#define _H_COM_DIAG_DIMINUTO_TIMER_

/**
 * @file
 *
 * Copyright 2009 Digital Aggregates Corporation, Arvada CO 80001-0587 USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "com/diag/diminuto/diminuto_types.h"

/**
 * Start a timer for the specified number of microseconds. When the timer
 * expires, the calling process will receive a SIGALRM. If a timer is already
 * running, the amount of time left in the prior timer will be returned.
 * The actual timer duration will be approximate depending on the
 * granularity of the system clock and latency in the implementation.
 * The timer fires only once per call. Calling with zero microseconds
 * cancels any prior timer.
 * @param microseconds is the desired timer duration in microseconds.
 * @return the number of microseconds remaining in prior timer if
 * a timer was already running.
 */
extern diminuto_usec_t diminuto_timer_oneshot(diminuto_usec_t microseconds);

/**
 * Start an periodic timer for the specified number of microseconds. When
 * the timer expires, the calling process will receive a SIGALRM. If a
 * timer is already running, the amount of time left in the prior timer
 * will be returned. The actual timer period will be approximate depending
 * on the granularity of the system clock and latency in the implementation.
 * The timer fires repeatedly with a periodicity of the specified microseconds
 * until it is cancelled. Calling with zero microseconds cancels the timer.
 * @param microseconds is the desired period interval in microseconds.
 * @return the number of microseconds remaining in the prior timer if
 * a timer was already running.
 */
extern diminuto_usec_t diminuto_timer_periodic(diminuto_usec_t microseconds);

#endif
