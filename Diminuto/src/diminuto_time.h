/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_TIME_PRIVATE_
#define _H_COM_DIAG_DIMINUTO_TIME_PRIVATE_

/**
 * @file
 * @copyright Copyright 2022 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is the private API for the Time feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is the private API for the Time feature. It provides functions
 * that do not log messages for use by the Log feature. This is to
 * prevent an infinite recursion in the event of an error in the
 * underlying C library functions or kernel system calls.
 */

#include "com/diag/diminuto/diminuto_types.h"

/**
 * Return the system clock time in Coordinated Universal Time (UTC) in
 * ticks since the Epoch (shown here in ISO8601 format)
 * 1970-01-01T00:00:00+0000. If an error occurs, no error message is
 * logged.
 * @return the number of ticks elapsed since the Epoch or -1 with
 * errno set if an error occurred.
 */
extern diminuto_sticks_t diminuto_time_clock_logging(void);

/**
 * Convert the number in ticks since the Epoch (shown here in ISO8601
 * format) 1970-01-01T00:00:00+0000 into individual fields representing the
 * Common Era (CE) date and and Coordinated Universal Time (UTC) time. If
 * an error occurs, no error message is logged.
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
extern int diminuto_time_zulu_logging(diminuto_sticks_t ticks, int * yearp, int * monthp, int * dayp, int * hourp, int * minutep, int * secondp, diminuto_ticks_t * tickp);

/**
 * Set the process logical clock to the specified value and clear the
 * error state. This value will be the next logical clock value
 * that is returned. FOR UNIT TESTING ONLY.
 * @param value is assigned to the logical clock counter.
 */
extern void diminuto_time_logical_set(uint64_t value);

#endif
