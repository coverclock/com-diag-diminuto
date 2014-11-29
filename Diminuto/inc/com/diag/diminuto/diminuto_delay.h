/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_DELAY_
#define _H_COM_DIAG_DIMINUTO_DELAY_

/**
 * @file
 *
 * Copyright 2008-2014 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "com/diag/diminuto/diminuto_types.h"

/**
 * @def COM_DIAG_DIMINUTO_DELAY_FREQUENCY
 * This manifest constant is the frequency in Hertz at which this feature
 * operates. The inverse of this value is the smallest unit of time in fractions
 * of a second that this feature can express or use. This constant is provided
 * for use in those cases where it is useful to have the value at compile time.
 * However, you chould always prefer to use the inline function when possible.
 */
#define COM_DIAG_DIMINUTO_DELAY_FREQUENCY (1000000000LL)

/**
 * Return the resolution of the Diminuto delay units in ticks per second
 * (Hertz). Delay durations smaller than the equivalent period in ticks may
 * not yield the expected results. In fact, there is no guarantee that the
 * underlying software platform or hardware target can support delay durations
 * with even this resolution.
 * @return the resolution in ticks per second.
 */
static inline diminuto_ticks_t diminuto_delay_frequency(void)
{
	return COM_DIAG_DIMINUTO_DELAY_FREQUENCY;
}

/**
 * Delay the calling process for the specified number of ticks.
 * If interruptable, return if interrupted with errno set to EINTR,
 * otherwise continue delaying. Return with errno set to something other
 * then EINTR of an error occurs. The actual delay duration will be approximate
 * depending on the granularity of the system clock and latency in the
 * implementation. If a delay of zero ticks is specified, the behavior
 * of the function is undefined and will depend on what the underlying platform
 * does.
 * @param ticks is the desired delay interval in ticks.
 * @param interruptable is true if interruptable, false otherwise.
 * @return the number of ticks remaining in the delay duration if
 * the function returned prematurely.
 */
extern diminuto_ticks_t diminuto_delay(diminuto_ticks_t ticks, int interruptable);

/**
 * Force a context switch without otherwise blocking.
 * @return 0 if successful, <0 with errno set otherwise.
 */
extern int diminuto_yield(void);

#endif
