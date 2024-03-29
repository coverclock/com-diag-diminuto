/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_TIME_
#define _H_COM_DIAG_DIMINUTO_TIME_

/**
 * @file
 * @copyright Copyright 2008-2023 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief Implements a consistent interface for dealing with time;
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 *
 * The Time feature provides an API for dealing with durations, Time Of Day
 * (TOD), and time and date stamps. Important safety tip: although UNIX (and
 * POSIX) professes to keep the time in Coordinated Universal Time (UTC), it
 * doesn't account for the occasional leap second. This means it isn't strictly
 * UTC. Worse, the time may appear discontinuous if and when the system clock
 * is manually adjusted. This has no effect on the monotonically increasing
 * clock, which is why that's the clock you should use to measure the passage
 * of time. So UNIX doesn't quite do UTC, but it also doesn't quite do
 * International Atomic Time (TAI) either, which also doesn't do leap seconds.
 * Nor does it do GPS time, which accounts for the leap seconds that were
 * present with GPS was inaugurated, but not for those that happened
 * subsequently. Also: daylight saving time is just an abomination.
 *
 * REFERENCES
 *
 * L. Lamport, "Time, Clocks, and the Ordering of Events in a Distributed
 * System", CACM, 21.7, 1978-07, pp. 558-565
 */

#include "com/diag/diminuto/diminuto_types.h"
#include "com/diag/diminuto/diminuto_minmaxof.h"
#include <unistd.h> /* For _POSIX_TIMERS. */

#if (defined(_POSIX_TIMERS) && (_POSIX_TIMERS > 0))

    /**
     * @def COM_DIAG_DIMINUTO_TIME_FREQUENCY
     * This manifest constant is the frequency in Hertz at which this feature
     * operates. The inverse of this value is the smallest unit of time in
     * fractions of a second that this feature can express or use. This
     * constant is provided for use in those cases where it is useful to have
     * the value at compile time. However, you should always prefer to use the
     * inline function when possible. NOTE that this is NOT the frequency of
     * the units of ticks.
     */
#   define COM_DIAG_DIMINUTO_TIME_FREQUENCY (1000000000LL)

#else

    /**
     * @def COM_DIAG_DIMINUTO_TIME_FREQUENCY
     * This manifest constant is the frequency in Hertz at which this feature
     * operates. The inverse of this value is the smallest unit of time in
     * fractions of a second that this feature can express or use. This
     * constant is provided for use in those cases where it is useful to have
     * the value at compile time. However, you should always prefer to use the
     * inline function when possible. NOTE that this is NOT the frequency of
     * the units of ticks.
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
 * @def COM_DIAG_DIMINUTO_TIME_ERROR
 * This manifest constant is used as an error return for those functions
 * that return a signed number of ticks since the Epoch. This is because
 * negative values since the Epoch are valid. However, functions that
 * return clock time like diminuto_time_clock() should never legitimately
 * return a negative value upon success.
 */
#define COM_DIAG_DIMINUTO_TIME_ERROR diminuto_minimumof(diminuto_sticks_t);

/**
 * This constant is used as an error return for those functions that
 * return a signed number of ticks since the Epoch. This is because
 * negative values since the Epoch are valid. However, functions that
 * return clock time like diminuto_time_clock() should never legitimately
 * return a negative value upon success.
 */
static const diminuto_sticks_t DIMINUTO_TIME_ERROR = COM_DIAG_DIMINUTO_TIME_ERROR;

/**
 * Return the system clock time in Coordinated Universal Time (UTC) in
 * ticks since the Epoch (shown here in ISO8601 format)
 * 1970-01-01T00:00:00+0000.
 * @return the number of ticks elapsed since the Epoch or <0 with
 * errno set if an error occurred.
 */
extern diminuto_sticks_t diminuto_time_clock(void);

/**
 * Return the time in International Atomic Time (TAI) in ticks since the
 * Epoch (shown here in ISO8601 format) 1970-01-01T00:00:00+0000.
 * @return the number of ticks elapsed since the Epoch or <0 with
 * errno set if an error occurred.
 */
extern diminuto_sticks_t diminuto_time_atomic(void);

/**
 * Return the elapsed time in ticks of a monotonically increasing
 * clock. This value is immune to time adjustments in the system clock and
 * hence is usable to measure duration and elapsed time. If the underlying
 * platform does not support this feature, the standard clock time is returned;
 * however this time may have discontinuities due to NTP or manual time changes.
 * @return the number of ticks elapsed since an arbitrary epoch or
 * <0 with errno set if an error occurred.
 */
extern diminuto_sticks_t diminuto_time_elapsed(void);

/**
 * Return the CPU time in ticks for the calling process.
 * @return the number of ticks elapsed since an arbitrary epoch or
 * <0 with errno set if an error occurred.
 */
extern diminuto_sticks_t diminuto_time_process(void);

/**
 * Return the CPU time in ticks for the calling thread.
 * @return the number of ticks elapsed since an arbitrary epoch or
 * <0 with errno set if an error occurred.
 */
extern diminuto_sticks_t diminuto_time_thread(void);

/**
 * @def COM_DIAG_DIMINUTO_TIME_LOGICAL_MAXIMUM
 * This symbol defines the largest possible logical clock value.
 */
#define COM_DIAG_DIMINUTO_TIME_LOGICAL_MAXIMUM diminuto_maximumof(uint64_t)

/**
 * This constant is the largest possible logical clock value.
 */
static const uint64_t DIMINUTO_TIME_LOGICAL_MAXIMUM = COM_DIAG_DIMINUTO_TIME_LOGICAL_MAXIMUM;

/**
 * Return the logical clock, which is a simple incrementing sixty-four bit
 * unsigned counter value that is guaranteed to be unique for any thread in
 * the same process. Under normal circumstances, the function returns with
 * errno set to zero. When the counter wraps around back to zero, the function
 * will continue to return successive values but with errno set to EOVERFLOW
 * until the error state is acknowledged by a logical clock reset. (If the
 * application does so inside its own critical section, this can allow it to
 * count the number of logical clock wraps.)
 * @return a unique logical clock value, with errno explicitly set.
 */
extern uint64_t diminuto_time_logical(void);

/**
 * Reset the logical clock error state. The value of errno after a logical
 * clock request will return to being zero until the logical clock wraps
 * around again.
 */
extern void diminuto_time_logical_reset(void);

/**
 * Return the number of ticks the local time zone is offset from Coordinated
 * Universal Time (UTC). For time zones west of UTC, this will be a negative
 * value (earlier), and for east of UTC, a positive value (later). This offset
 * does NOT account for Daylight Saving Time (DST) if it is used and in
 * effect. Note that one hundred and eighty degrees away from UTC is the
 * International Date Line (IDL), which splits its time zone in half. Clocks on
 * either side of the IDL in that time zone read the same clock time, but are
 * twenty-four hours apart. Note that this is an ISO8601 sense of time zone
 * zone offset AHEAD (>0) or BEHIND (<0) of UTC, NOT the offset WEST that is
 * used by POSIX (it will be the negative of the POSIX value).
 * @return the number of ticks west of UTC.
 */
extern diminuto_sticks_t diminuto_time_timezone(void);

/**
 * Return the number of ticks the local time zone is offset _west_
 * because of Daylight Saving Time (DST). It DST is not used for local time,
 * or is not in effect for the specified time, then this value will be zero,
 * otherwise it will be a positive number since when DST is in effect the local
 * clock time moves forward. Note that DST is an abomination.
 * @param ticks is the number of ticks since the Epoch.
 * @return ticks added to this time zone, or -1 with (errno>0) for error.
 */
extern diminuto_sticks_t diminuto_time_daylightsaving(diminuto_sticks_t ticks);

/**
 * Return the number of ticks after the Epoch (shown here in ISO8601
 * format) 1970-01-01T00:00:00+0000 for the specified date and time. If the
 * specified date or time is invalid or causes an overflow during the
 * conversion, the results are unspecified. Note that negative numbers
 * (including -1) is a valid return value. In the event of an error, -1
 * is return AND (errno>0) MUST be checked as well.
 * @param year is the year including the century in the range [1970..].
 * @param month is the month of the year in the range [1..12].
 * @param day is the day of the month in the range [1..31].
 * @param hour is the hour of the day in the range [0..23].
 * @param minute is the minute of the hour in the range [0..59].
 * @param second is the second of the minute in the range [0..59].
 * @param tick is the fraction of a second in the range [0..(resolution-1)].
 * @param offset is the number of ticks AHEAD (>0) or BEHIND (<0) of UTC.
 * @param daylightsaving is the number of ticks offset for DST (0 if UTC).
 * @return the number of ticks since the Epoch, or -1 with (errno!=0) for error.
 */
extern diminuto_sticks_t diminuto_time_epoch(int year, int month, int day, int hour, int minute, int second, diminuto_ticks_t tick, diminuto_sticks_t offset, diminuto_sticks_t daylightsaving);

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
 * Return the character used as the military time zone name given the time zone
 * offset, positive or negative, in ticks. If the time zone offset it not an
 * even number of hours (some aren't), or if it is invalid, "J" (Juliet), the
 * unspecified local time zone, is returned.
 * @param ticks is the time zone offset in ticks since the Epoch.
 * @return the military time zone name.
 */
extern char diminuto_time_zonename(diminuto_sticks_t ticks);

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
