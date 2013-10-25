/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_TIME_
#define _H_COM_DIAG_DIMINUTO_TIME_

/**
 * @file
 *
 * Copyright 2008-2013 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 *
 * Important safety tip: although UNIX (and POSIX) professes to keep the time
 * in Coordinated Universal Time (UTC) it doesn't account for periodic leap
 * seconds. This means it isn't strictly UTC, and worse, the time may appear
 * discontinuous if and when the system clock is manually adjusted. This has
 * no effect on the monotonically increasing clock, which is why that's the
 * clock you should use to measure the passage of time. So UNIX doesn't quite
 * do UTC, but it also doesn't quite do International Atomic Time (IAT) either,
 * which also doesn't do leap seconds. (I think GPS time falls into this
 * category as well.) Also: daylight saving time is just an abomination.
 */

#include "com/diag/diminuto/diminuto_types.h"

/**
 * Return the system clock time in Coordinated Universal Time (UTC) in
 * microseconds since the Epoch (shown here in ISO8601 format)
 * 1970-01-01T00:00:00+0000.
 * @return the number of microseconds elapsed since the Epoch or ~0ULL with
 * errno set if an error occurred.
 */
extern diminuto_usec_t diminuto_time_clock(void);

/**
 * Return the elapsed time in microseconds of a monotonically increasing
 * clock. This value is immune to time adjustments in the system clock and
 * hence is usable to measure duration and elapsed time.
 * @return the number of microseconds elapsed since an arbitrary epoch or
 * ~0ULL with errno set if an error occurred.
 */
extern diminuto_usec_t diminuto_time_elapsed(void);

/**
 * Return the CPU time in microseconds for the calling process.
 * @return the number of microseconds elapsed since an arbitrary epoch or
 * ~0ULL with errno set if an error occurred.
 */
extern diminuto_usec_t diminuto_time_process(void);

/**
 * Return the CPU time in microseconds for the calling thread.
 * @return the number of microseconds elapsed since an arbitrary epoch or
 * ~0ULL with errno set if an error occurred.
 */
extern diminuto_usec_t diminuto_time_thread(void);

/**
 * Return the number of microseconds east of Universal Coordinated Time (UTC)
 * of the target system. This will be a negative number that indicates how
 * much earlier the local time zone is from UTC.
 * @return the number of microseconds east of UTC.
 */
extern diminuto_usec_t diminuto_time_offset(void);

/**
 * Return >0 if Daylight Saving Time is used in the local time zone _and_ is
 * currently in effect, <0 if an error occurred, 0 otherwise.
 * @return non-zero for DST, zero otherwise.
 */
extern int diminuto_time_daylightsaving(void);

/**
 * Return the number of microseconds after the Epoch (shown here in ISO8601
 * format) 1970-01-01T00:00:00+0000 for the specified date and time. If the
 * specified date or time is invalid, the results are unspecified.
 * @param year is the year including the century in the range [1970..].
 * @param month is the month of the year in the range [1..12].
 * @param day is the day of the month in the range [1..31].
 * @param hour is the hour of the day in the range [0..23].
 * @param minute is the minute of the hour in the range [0..59].
 * @param second is the second of the minute in the range [0..59].
 * @param microsecond is the fraction of a second in the range [0..999999].
 * @param offset is the number of microseconds east for the timezone (0 if UTC).
 * @param daylightsaving if true assumes DST is in effect (0 if UTC).
 * @return the number of microseconds since the Epoch.
 */
extern diminuto_usec_t diminuto_time_epoch(int year, int month, int day, int hour, int minute, int second, int microsecond, diminuto_usec_t offset, int daylightsaving);

/**
 * Convert the number in microseconds since the Epoch (shown here in ISO8601
 * format) 1970-01-01T00:00:00+0000 into individual fields representing the
 * Common Era (CE) date and and Coordinated Universal Time (UTC) time.
 * @clock is the number of microseconds since the Epoch.
 * @param yearp is where the year including the century will be returned.
 * @param monthp is where the month of the year will be returned.
 * @param dayp is where the day of the month will be returned.
 * @param hourp is where the hour of the day will be returned.
 * @param minutep is where the minute of the hour will be returned.
 * @param secondp is where the second of the minute will be returned.
 * @param microsecondp is where the fraction of a second will be returned.
 * @return clock for success, -1 otherwise.
 */
extern diminuto_usec_t diminuto_time_zulu(diminuto_usec_t clock, int * yearp, int * monthp, int * dayp, int * hourp, int * minutep, int * secondp, int * microsecondp);

/**
 * Convert the number in microseconds since the Epoch (shown here in ISO8601
 * format) 1970-01-01T00:00:00+0000 into individual fields representing the
 * local date and time.
 * @clock is the number of microseconds since the Epoch.
 * @param yearp is where the year including the century will be returned.
 * @param monthp is where the month of the year will be returned.
 * @param dayp is where the day of the month will be returned.
 * @param hourp is where the hour of the day will be returned.
 * @param minutep is where the minute of the hour will be returned.
 * @param secondp is where the second of the minute will be returned.
 * @param microsecondp is where the fraction of a second will be returned.
 * @return clock for success, -1 otherwise.
 */
extern diminuto_usec_t diminuto_time_juliet(diminuto_usec_t clock, int * yearp, int * monthp, int * dayp, int * hourp, int * minutep, int * secondp, int * microsecondp);

#endif
