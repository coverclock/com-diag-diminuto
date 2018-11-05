/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_TIME_
#define _H_COM_DIAG_DIMINUTO_TIME_

/**
 * @file
 *
 * Copyright 2008-2018 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in LICENSE.txt<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 *
 * Important safety tip: although UNIX (and POSIX) professes to keep the time
 * in Coordinated Universal Time (UTC), it doesn't account for the occasional
 * leap second. This means it isn't strictly UTC. Worse, the time may appear
 * discontinuous if and when the system clock is manually adjusted. This has
 * no effect on the monotonically increasing clock, which is why that's the
 * clock you should use to measure the passage of time. So UNIX doesn't quite
 * do UTC, but it also doesn't quite do International Atomic Time (IAT) either,
 * which also doesn't do leap seconds. (I think GPS time falls into this
 * category as well.) Also: daylight saving time is just an abomination.
 */

#include "com/diag/diminuto/diminuto_types.h"
#include <unistd.h> /* For _POSIX_TIMERS. */

#if (defined(_POSIX_TIMERS) && (_POSIX_TIMERS > 0))

    /**
     * @def COM_DIAG_DIMINUTO_TIME_FREQUENCY
     * This manifest constant is the frequency in Hertz at which this feature
     * operates. The inverse of this value is the smallest unit of time in fractions
     * of a second that this feature can express or use. This constant is provided
     * for use in those cases where it is useful to have the value at compile time.
     * However, you chould always prefer to use the inline function when possible.
     */
#   define COM_DIAG_DIMINUTO_TIME_FREQUENCY (1000000000LL)

#else

    /**
     * @def COM_DIAG_DIMINUTO_TIME_FREQUENCY
     * This manifest constant is the frequency in Hertz at which this feature
     * operates. The inverse of this value is the smallest unit of time in fractions
     * of a second that this feature can express or use. This constant is provided
     * for use in those cases where it is useful to have the value at compile time.
     * However, you chould always prefer to use the inline function when possible.
     */
#   define COM_DIAG_DIMINUTO_TIME_FREQUENCY (1000000LL)

#endif

/**
 * Return the resolution of the Diminuto time units in ticks per second (Hertz).
 * Although the underlying platform may be able to return time with this
 * resolution, there is no guarantee that the system clock actually has this
 * degree of accuracy.
 * @return the resolution in ticks per second.
 */
static inline diminuto_sticks_t diminuto_time_frequency(void) {
    return COM_DIAG_DIMINUTO_TIME_FREQUENCY;
}

/**
 * Return the system clock time in Coordinated Universal Time (UTC) in
 * ticks since the Epoch (shown here in ISO8601 format)
 * 1970-01-01T00:00:00+0000.
 * @return the number of ticks elapsed since the Epoch or -1 with
 * errno set if an error occurred.
 */
extern diminuto_sticks_t diminuto_time_clock(void);

/**
 * Return the elapsed time in ticks of a monotonically increasing
 * clock. This value is immune to time adjustments in the system clock and
 * hence is usable to measure duration and elapsed time. If the underlying
 * platform does not support this feature, the standard clock time is returned;
 * however this time may have discontinuities due to NTP or manual time changes.
 * @return the number of ticks elapsed since an arbitrary epoch or
 * -1 with errno set if an error occurred.
 */
extern diminuto_sticks_t diminuto_time_elapsed(void);

/**
 * Return the CPU time in ticks for the calling process.
 * @return the number of ticks elapsed since an arbitrary epoch or
 * -1 with errno set if an error occurred.
 */
extern diminuto_sticks_t diminuto_time_process(void);

/**
 * Return the CPU time in ticks for the calling thread.
 * @return the number of ticks elapsed since an arbitrary epoch or
 * -1 with errno set if an error occurred.
 */
extern diminuto_sticks_t diminuto_time_thread(void);

/**
 * Return the logical clock, which is a simple incrementing sixty-four bit
 * unsigned counter value that is guaranteed to be unique for any thread in
 * the same process until the counter reaches its maximum possible value of
 * ~(uint64_t)0, after which it wraps around back to zero.
 * @return a unique logical clock value.
 */
extern uint64_t diminuto_time_logical(void);

/**
 * Return the number of ticks the local time zone is offset from Coordinated
 * Universal Time (UTC). For time zones west of UTC, this will be a negative
 * value (earlier), and for east of UTC, a positive value (later). This offset
 * does NOT account for Daylight Saving Time (DST) if it is used and in
 * effect. Note that one hundred and eighty degrees away from UTC is the
 * International Date Line (IDL), which splits its time zone in half. Clocks on
 * either side of the IDL in that time zone read the same clock time, but are
 * twenty-four hours apart. (Since the offset of the time zone doesn't shift
 * over the span of a year like Daylight Saving Time does, the argument to this
 * function can probably always be zero.) Note that this is an ISO-8601 sense
 * of time zone offset AHEAD (>0) or BEHIND (<0) of UTC, NOT the offset WEST
 * that is used by POSIX (it will be the negative of the POSIX value).
 * @param ticks is the number of ticks since the Epoch.
 * @return the number of ticks west of UTC.
 */
extern diminuto_sticks_t diminuto_time_timezone(diminuto_sticks_t ticks);

/**
 * Return the number of ticks the local time zone is offset _west_
 * because of Daylight Saving Time (DST). It DST is not used for local time,
 * or is not in effect for the specified time, then this value will be zero,
 * otherwise it will be a positive number since when DST is in effect the local
 * clock time moves forward. Note that DST is an abomination.
 * @param ticks is the number of ticks since the Epoch.
 * @return the number of ticks added to this time zone.
 */
extern diminuto_sticks_t diminuto_time_daylightsaving(diminuto_sticks_t ticks);

/**
 * Return the number of ticks after the Epoch (shown here in ISO8601
 * format) 1970-01-01T00:00:00+0000 for the specified date and time. If the
 * specified date or time is invalid, the results are unspecified.
 * @param year is the year including the century in the range [1970..].
 * @param month is the month of the year in the range [1..12].
 * @param day is the day of the month in the range [1..31].
 * @param hour is the hour of the day in the range [0..23].
 * @param minute is the minute of the hour in the range [0..59].
 * @param second is the second of the minute in the range [0..59].
 * @param tick is the fraction of a second in the range [0..(resolution-1)].
 * @param offset is the number of ticks AHEAD (>0) or BEHIND (<0) of UTC.
 * @param daylightsaving is the number of ticks offset for DST (0 if UTC).
 * @return the number of ticks since the Epoch.
 */
extern diminuto_sticks_t diminuto_time_epoch(int year, int month, int day, int hour, int minute, int second, int tick, diminuto_sticks_t offset, diminuto_sticks_t daylightsaving);

/**
 * Convert the number in ticks since the Epoch (shown here in ISO8601
 * format) 1970-01-01T00:00:00+0000 into individual fields representing the
 * Common Era (CE) date and and Coordinated Universal Time (UTC) time.
 * @param ticks is the number of ticks since the Epoch.
 * @param yearp is where the year including the century will be returned.
 * @param monthp is where the month of the year will be returned.
 * @param dayp is where the day of the month will be returned.
 * @param hourp is where the hour of the day will be returned.
 * @param minutep is where the minute of the hour will be returned.
 * @param secondp is where the second of the minute will be returned.
 * @param tickp is where the fraction of a second will be returned.
 * @return 0 for success, <0 otherwise.
 */
extern int diminuto_time_zulu(diminuto_sticks_t ticks, int * yearp, int * monthp, int * dayp, int * hourp, int * minutep, int * secondp, diminuto_ticks_t * tickp);

/**
 * Convert the number in ticks since the Epoch (shown here in ISO8601
 * format) 1970-01-01T00:00:00+0000 into individual fields representing the
 * local date and time.
 * @param ticks is the number of ticks since the Epoch.
 * @param yearp is where the year including the century will be returned.
 * @param monthp is where the month of the year will be returned.
 * @param dayp is where the day of the month will be returned.
 * @param hourp is where the hour of the day will be returned.
 * @param minutep is where the minute of the hour will be returned.
 * @param secondp is where the second of the minute will be returned.
 * @param tickp is where the fraction of a second will be returned.
 * @return 0 for success, <0 otherwise.
 */
extern int diminuto_time_juliet(diminuto_sticks_t ticks, int * yearp, int * monthp, int * dayp, int * hourp, int * minutep, int * secondp, diminuto_ticks_t * tickp);

/**
 * Convert ticks into individual fields representing a time duration. All of
 * the field values will be positive; the value returned by the function
 * indicates whether the duration was positive or negative.
 * @param ticks is the number of ticks in a time interval.
 * @param dayp is where the number of days will be returned.
 * @param hourp is where the number of hours will be returned.
 * @param minutep is where the number of minutes will be returned.
 * @param secondp is where the number of seconds will be returned.
 * @param tickp is where the number of ticks will be returned.
 * @return >0 if the duration was positive, <0 if the duration was negative.
 */
extern int diminuto_time_duration(diminuto_sticks_t ticks, int * dayp, int * hourp, int * minutep, int * secondp, diminuto_ticks_t * tickp);

#endif
