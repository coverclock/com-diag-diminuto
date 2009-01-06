/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_PERIODIC_
#define _H_COM_DIAG_DIMINUTO_PERIODIC_

/**
 * @file
 *
 * Copyright 2009 Digital Aggregates Corporation, Arvada CO 80001-0587 USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include <stdint.h>

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
extern uint32_t diminuto_periodic(uint32_t microseconds);

#endif
