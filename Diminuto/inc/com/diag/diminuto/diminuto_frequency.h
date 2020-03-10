/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_FREQUENCY_
#define _H_COM_DIAG_DIMINUTO_FREQUENCY_

/**
 * @file
 *
 * Copyright 2013-2017 Digital Aggregates Corporation, Colorado, USA.
 * Licensed under the terms in LICENSE.txt.
 *
 * The Frequency feature defines the Diminuto fundamental unit of time, in terms
 * of the duration of a single Diminuto tick, represented as the inverse of this
 * period, which is a frequency in Hertz or cycles per second. _All_ Diminuto
 * features represent time at this resolution, unlike the system calls and
 * library functions in the underlying POSIX implementation. Frequencies
 * reported by other Diminuto features are to indicate the maximum resolution of
 * the underlying implementation. So while Diminuto may allow you to specify a
 * timer in billion-ths of a second, and the timer feature expects this, the
 * underlying implementation in POSIX truncates the timer value to million-ths
 * of a second.
 *
 * The POSIX type time_t is signed, which allows values of this type to express
 * dates both after and before the POSIX epoch in 1970. When time_t is 32-bits,
 * this limits the range of dates from 1901 to 2038. But on most POSIX systems
 * (at least the ones I typically deal with), time_t is now 64-bits, which
 * yields a date dynamic range significantly larger than the age of the
 * universe. Because time_t has 1 second resolution, and diminuto_sticks_t (the
 * signed version of Diminuto ticks) has a 1 nanosecond resolution, the date
 * range of time_t is a billion times larger than that of Diminuto. Generally
 * that isn't an issue.
 *
 * Note that the frequency values reported by this feature, as well as others
 * such as delay, mux, poll, time, and timer, are signed. You are correct in
 * assuming that there is no such thing as a negative frequency for any of these
 * features. However, since negative offsets from the epoch _are_ valid, correct
 * conversion between units requires that they all be signed. Mixing signed and
 * unsigned values in an arithmetic expression results, in C, in an unsigned
 * result, according to the ANSI standard. This causes wackiness to ensue when,
 * for example, the date offset is signed and negative, but the frequency is
 * unsigned. (The few hours I spent debugging this are part of my life I will
 * never get back.)
 *
 * So why does Diminuto have a signed (diminuto_sticks_t) and an unsigned
 * (diminuto_ticks_t) version of its time data type? To indicate in the API of
 * each feature what time values may be legitimately negative and which ones
 * may not. You can, for example, have a negative offset from the epoch to
 * indicate a date prior to 1970, but you cannot have a negative delay time or
 * a negative timer interval.
 *
 * Also, some features, like time, have functions that return a signed value of
 * ticks just so that they can return a -1 to indicate an error. But typically,
 * after checking for that, you may want to use an unsigned variable so that you
 * can use unsigned arithmetic to calculate duration between two tick values
 * without concern for rollover.
 */

#include "com/diag/diminuto/diminuto_types.h"

/**
 * @def COM_DIAG_DIMINUTO_FREQUENCY
 * This manifest constant is the frequency in Hertz at which this feature
 * operates. The inverse of this value is the smallest unit of time in fractions
 * of a second that this feature can express or use. This constant is provided
 * for use in those cases where it is useful to have the value at compile time.
 * However, you chould always prefer to use the inline function when possible.
 */
#define COM_DIAG_DIMINUTO_FREQUENCY (1000000000LL)

/**
 * Return the fundamental Diminuto frequency in ticks per second (Hertz). The
 * inverse of this value is the duration of a Diminuto tick, the smallest
 * expressible unit of time, in seconds. Other features in Diminuto may
 * implement lower frequencies, typically because of inherent limitations in
 * the underlying platform or target, but all of them express time in units of
 * this period.
 * @return the number of ticks per second.
 */
static inline diminuto_sticks_t diminuto_frequency(void)
{
    return COM_DIAG_DIMINUTO_FREQUENCY;
}

/**
 * Return the fundamental Diminuto frequency in ticks per second (Hertz).
 * This is merely an outline version of diminuto_frequency() provided as an
 * API for non-C applications.
 * @return the number of ticks per second.
 */
extern diminuto_sticks_t diminuto_frequency_f(void);

/**
 * Convert a value in ticks into units of the specified frequency.
 * @param ticks is the value in ticks.
 * @param hertz is the specified frequency.
 * @return a value in units of the specified frequency.
 */
static inline diminuto_sticks_t diminuto_frequency_ticks2units(diminuto_sticks_t ticks, diminuto_sticks_t hertz)
{
    return
        (hertz > diminuto_frequency()) ?
            (ticks * (hertz / diminuto_frequency())) :
        (hertz < diminuto_frequency()) ?
            (ticks / (diminuto_frequency() / hertz)) :
        ticks;
}

/**
 * Convert a value in units of the specified frequency into ticks.
 * @param units is the value in units of a specified frequency.
 * @param hertz is the specified frequency.
 * @return a value in ticks.
 */
static inline diminuto_sticks_t diminuto_frequency_units2ticks(diminuto_sticks_t units, diminuto_sticks_t hertz)
{
    return
        (hertz > diminuto_frequency()) ?
            (units / (hertz / diminuto_frequency())) :
        (hertz < diminuto_frequency()) ?
            (units * (diminuto_frequency() / hertz)) :
        units;

}

/**
 * Convert a value in ticks into whole seconds with no fractional part.
 * @param ticks is the value in ticks.
 * @return the value in whole seconds.
 */
static inline diminuto_sticks_t diminuto_frequency_ticks2wholeseconds(diminuto_sticks_t ticks)
{
    return diminuto_frequency_ticks2units(ticks, 1);
}

/**
 * Convert a value in ticks into a fraction of a second in units of the
 * specified frequency, and discard the whole seconds portion of the result.
 * @param ticks is the value in ticks.
 * @param hertz is the specified frequency.
 * @return the fractional portion of the value in seconds.
 */
static inline diminuto_sticks_t diminuto_frequency_ticks2fractionalseconds(diminuto_sticks_t ticks, diminuto_sticks_t hertz)
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
static inline diminuto_sticks_t diminuto_frequency_seconds2ticks(diminuto_sticks_t whole, diminuto_sticks_t fraction, diminuto_sticks_t hertz)
{
    return diminuto_frequency_units2ticks(whole, 1) + diminuto_frequency_units2ticks(fraction, hertz);
}

/**
 * Round up a value in ticks to an even multiple of the specified frequency.
 * @param ticks is the value in ticks.
 * @param hertz is the spefified frequency.
 * @return the value in ticks.
 */
static inline diminuto_sticks_t diminuto_frequency_roundup(diminuto_sticks_t ticks, diminuto_sticks_t hertz) {
    diminuto_sticks_t remainder;
    remainder = ticks % hertz;
    return (remainder == 0) ? ticks : ((ticks - remainder) + ((ticks < 0) ? -hertz : hertz));
}

/**
 * Round down a value in ticks to an even multiple of the specified frequency.
 * @param ticks is the value in ticks.
 * @param hertz is the spefified frequency.
 * @return the value in ticks.
 */
static inline diminuto_sticks_t diminuto_frequency_rounddown(diminuto_sticks_t ticks, diminuto_sticks_t hertz) {
    diminuto_sticks_t remainder;
    remainder = ticks % hertz;
    return (remainder == 0) ? ticks : (ticks - remainder);
}

#endif
