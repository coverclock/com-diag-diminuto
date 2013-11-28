/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_FREQUENCY_
#define _H_COM_DIAG_DIMINUTO_FREQUENCY_

/**
 * @file
 *
 * Copyright 2013 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "com/diag/diminuto/diminuto_types.h"

/**
 * Return the fundamental Diminuto frequency in ticks per second (Hertz). The
 * inverse of this value is the duration of a Diminuto tick, the smallest
 * expressible unit of time. Other features in Diminuto may implement lower
 * frequencies, typically because of inherent limitations in the underlying
 * platform or target, but all of them express time in units of this period.
 * @return the number of ticks per second.
 */
static inline diminuto_ticks_t diminuto_frequency(void)
{
	return 1000000000LL;
}

/**
 * Convert a value in ticks into units of the specified frequency.
 * @param ticks is the value in ticks.
 * @param hertz is the specified frequency.
 * @return a value in units of the specified frequency.
 */
static inline diminuto_ticks_t diminuto_frequency_ticks2units(diminuto_ticks_t ticks, diminuto_ticks_t hertz)
{
	return (hertz > diminuto_frequency()) ? (ticks * (hertz / diminuto_frequency())) : (hertz < diminuto_frequency()) ? (ticks / (diminuto_frequency() / hertz)) : ticks;
}

/**
 * Convert a value in units of the specified frequency into ticks.
 * @param units is the value in units of a specified frequency.
 * @param hertz is the specified frequency.
 * @return a value in ticks.
 */
static inline diminuto_ticks_t diminuto_frequency_units2ticks(diminuto_ticks_t units, diminuto_ticks_t hertz)
{
	return (hertz > diminuto_frequency()) ? (units / (hertz / diminuto_frequency())) : (hertz < diminuto_frequency()) ? (units * (diminuto_frequency() / hertz)) : units;

}

/**
 * Convert a value in ticks into whole seconds with no fractional part.
 * @param ticks is the value in ticks.
 * @return the value in whole seconds.
 */
static inline diminuto_ticks_t diminuto_frequency_ticks2wholeseconds(diminuto_ticks_t ticks)
{
    return diminuto_frequency_ticks2units(ticks, 1);
}

/**
 * Convert a value in ticks into a fraction of a second in units of the
 * specified frequency and discard the whole seconds portion of the result.
 * @param ticks is the value in ticks.
 * @param hertz is the specified frequency.
 * @return the fractional portion of the value in seconds.
 */
static inline diminuto_ticks_t diminuto_frequency_ticks2fractionalseconds(diminuto_ticks_t ticks, diminuto_ticks_t hertz)
{
	return diminuto_frequency_ticks2units(ticks % diminuto_frequency(), hertz);
}

/**
 * Convert a value in whole and fraction of a second in the specified frequency
 * into ticks.
 * @param whole is the value of whole seconds.
 * @param fraction is the value of the fractions of a second.
 * @param hertz is the specified frequency.
 * @return the value in ticks.
 */
static inline diminuto_ticks_t diminuto_frequency_seconds2ticks(diminuto_ticks_t whole, diminuto_ticks_t fraction, diminuto_ticks_t hertz)
{
	return diminuto_frequency_units2ticks(whole, 1) + diminuto_frequency_units2ticks(fraction, hertz);
}

#endif
