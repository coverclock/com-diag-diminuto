/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_TIME_
#define _H_COM_DIAG_DIMINUTO_TIME_

/**
 * @file
 *
 * Copyright 2008 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "com/diag/diminuto/diminuto_types.h"

/**
 * Return the system clock time in microseconds since the Epoch (shown here in
 * ISO8601 format): 1970-01-01T00:00:00+0000.
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
 * Return the number of microseconds west of Universal Coordinated Time (UTC)
 * of the target system.
 * @return the number of microseconds west of UTC.
 */
extern diminuto_usec_t diminuto_time_timezone(void);

/**
 * Return the number of microseconds of the specified Common Era (CE) date and
 * and Universal Coordinated Time (UTC) time after the Epoch (shown here in
 * ISO8601 format): 1970-01-01T00:00:00+0000. If the specified date or time is
 * invalid, the results are unspecified. This function depends on the time zone
 * of the underlying target having been administered correctly.
 * @param year is the year including the century in the range [1970..].
 * @param month is the month of the year in the range [1..12].
 * @param day is the day of the month in the range [1..31].
 * @param hour is the hour of the day in the range [0..23].
 * @param minute is the minute of the hour in the range [0..59].
 * @param second is the second of the minute in the range [0..59].
 * @param microsecond is the fraction of a second in the range [0..999999].
 * @return the number of microseconds since the Epoch.
 */
extern diminuto_usec_t diminuto_time_epoch(int year, int month, int day, int hour, int minute, int second, int microsecond);

/**
 * Convert the number in microseconds since the Epoch into individual fields
 * representing the Common Era (CE) date and and Universal Coordinated Time
 * (UTC) time. The Epoch (shown here in ISO8601 format) is
 * 1970-01-01T00:00:00+0000.
 * @clock is the number of microseconds since the Epoch.
 * @param yearp is where the year including the century will be returned.
 * @param monthp is where the month of the year will be returned.
 * @param dayp is where the day of the month will be returned.
 * @param hourp is where the hour of the day will be returned.
 * @param minutep is where the minute of the hour will be returned.
 * @param secondp is where the second of the minute will be returned.
 * @param microsecondp is where the fraction of a second will be returned.
 * @return 0 for success, <0 otherwise.
 */
extern int diminuto_time_datetime(diminuto_usec_t clock, int * yearp, int * monthp, int * dayp, int * hourp, int * minutep, int * secondp, int * microsecondp);

#endif
