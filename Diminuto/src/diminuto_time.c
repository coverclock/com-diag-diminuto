/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2008-2013 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "com/diag/diminuto/diminuto_time.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "diminuto_frequency.h"
#include <stdio.h>
#include <time.h>
#include <sys/time.h>

diminuto_ticks_t diminuto_time_resolution(void) {
	return COM_DIAG_DIMINUTO_FREQUENCY;
}

static diminuto_ticks_t diminuto_time_generic(clockid_t clock)
{
	diminuto_ticks_t ticks = -1;
	struct timespec elapsed;

	if (clock_gettime(clock, &elapsed) < 0) {
		diminuto_perror("diminuto_time_generic: clock_gettime");
	} else {
		ticks = COM_DIAG_DIMINUTO_TICKS_TO(elapsed.tv_sec, 1);
		ticks += COM_DIAG_DIMINUTO_TICKS_TO(elapsed.tv_nsec, 1000000000);
	}

	return ticks;
}

diminuto_ticks_t diminuto_time_clock()
{
    return diminuto_time_generic(CLOCK_REALTIME);
}

diminuto_ticks_t diminuto_time_elapsed()
{
#if defined(CLOCK_MONOTONIC_RAW)
	return diminuto_time_generic(CLOCK_MONOTONIC_RAW); /* Since Linux 2.6.28 */
#else
	return diminuto_time_generic(CLOCK_MONOTONIC); /* Prior to Linux 2.6.28 */
#endif
}

diminuto_ticks_t diminuto_time_process()
{
	return diminuto_time_generic(CLOCK_PROCESS_CPUTIME_ID);
}

diminuto_ticks_t diminuto_time_thread()
{
	return diminuto_time_generic(CLOCK_THREAD_CPUTIME_ID);
}

diminuto_ticks_t diminuto_time_timezone(diminuto_ticks_t ticks)
{
	diminuto_ticks_t timezone = -1;
	struct tm datetime;
	struct tm * datetimep;
	time_t juliet;

	ticks /= COM_DIAG_DIMINUTO_FREQUENCY;
	juliet = ticks;
	if ((datetimep = localtime_r(&juliet, &datetime)) == (struct tm *)0) {
		diminuto_perror("diminuto_time_timezone: localtime_r");
	} else if (datetimep->tm_isdst) {
		timezone = datetimep->tm_gmtoff;
		timezone -= 3600;
		timezone *= COM_DIAG_DIMINUTO_FREQUENCY;
	} else {
		timezone = datetimep->tm_gmtoff;
		timezone *= COM_DIAG_DIMINUTO_FREQUENCY;
	}

	return timezone;
}

diminuto_ticks_t diminuto_time_daylightsaving(diminuto_ticks_t ticks)
{
	diminuto_ticks_t daylightsaving = -1;
	struct tm datetime;
	struct tm * datetimep;
	time_t juliet;

	ticks /= COM_DIAG_DIMINUTO_FREQUENCY;
	juliet = ticks;
	if ((datetimep = localtime_r(&juliet, &datetime)) == (struct tm *)0) {
		diminuto_perror("diminuto_time_daylightsaving: localtime_r");
	} else if (datetimep->tm_isdst > 0) {
		daylightsaving = 3600;
		daylightsaving *= COM_DIAG_DIMINUTO_FREQUENCY;
	} else {
		daylightsaving = 0;
	}

	return daylightsaving;
}

diminuto_ticks_t diminuto_time_epoch(int year, int month, int day, int hour, int minute, int second, int tick, diminuto_ticks_t timezone, diminuto_ticks_t daylightsaving)
{
	diminuto_ticks_t ticks;
	struct tm datetime;
	time_t juliet;

	datetime.tm_year = year - 1900;
	datetime.tm_mon = month - 1;
	datetime.tm_mday = day;
	datetime.tm_hour = hour;
	datetime.tm_min = minute;
	datetime.tm_sec = second;
	datetime.tm_isdst = 0;
	/**
	 * mktime(3) indicates that a return of -1 indicates an error. But this
	 * isn't the case for Ubuntu 4.6.3: -1 is a valid return value that
	 * indicates a date and time one second earlier than the Epoch.
	 */
	juliet = mktime(&datetime);
	ticks = COM_DIAG_DIMINUTO_TICKS_TO(juliet, 1);
	ticks += diminuto_time_timezone(juliet); /* Because mktime(3) assumes local time. */
	ticks -= timezone;
	ticks -= daylightsaving;
	ticks += tick;

	return ticks;
}

static void diminuto_time_stamp(const struct tm *datetimep, diminuto_ticks_t ticks, int * yearp, int * monthp, int * dayp, int * hourp, int * minutep, int * secondp, int * tickp)
{
	if (yearp   != (int *)0) { *yearp   = datetimep->tm_year + 1900;           }
	if (monthp  != (int *)0) { *monthp  = datetimep->tm_mon + 1;               }
	if (dayp    != (int *)0) { *dayp    = datetimep->tm_mday;                  }
	if (hourp   != (int *)0) { *hourp   = datetimep->tm_hour;                  }
	if (minutep != (int *)0) { *minutep = datetimep->tm_min;                   }
	if (secondp != (int *)0) { *secondp = datetimep->tm_sec;                   }
	if (tickp   != (int *)0) { *tickp   = ticks % COM_DIAG_DIMINUTO_FREQUENCY; }
}

int diminuto_time_zulu(diminuto_ticks_t ticks, int * yearp, int * monthp, int * dayp, int * hourp, int * minutep, int * secondp, int * tickp)
{
	int rc = 0;
	struct tm datetime;
	struct tm * datetimep;
	time_t zulu;

	zulu = ticks / COM_DIAG_DIMINUTO_FREQUENCY;
	if ((datetimep = gmtime_r(&zulu, &datetime)) == (struct tm *)0) {
		diminuto_perror("diminuto_time_timestamp: gmtime_r");
		rc = -1;
	} else {
		diminuto_time_stamp(datetimep, ticks, yearp, monthp, dayp, hourp, minutep, secondp, tickp);
	}

	return rc;
}

int diminuto_time_juliet(diminuto_ticks_t ticks, int * yearp, int * monthp, int * dayp, int * hourp, int * minutep, int * secondp, int * tickp)
{
	int rc = 0;
	struct tm datetime;
	struct tm * datetimep;
	time_t juliet;

	juliet = ticks / COM_DIAG_DIMINUTO_FREQUENCY;
	if ((datetimep = localtime_r(&juliet, &datetime)) == (struct tm *)0) {
		diminuto_perror("diminuto_time_timestamp: localtime_r");
		rc = -1;
	} else {
		diminuto_time_stamp(datetimep, ticks, yearp, monthp, dayp, hourp, minutep, secondp, tickp);
	}

	return rc;
}

int diminuto_time_duration(diminuto_ticks_t ticks, int * dayp, int * hourp, int * minutep, int * secondp, int * tickp)
{
	int rc;
	diminuto_ticks_t divisor;

	if (ticks < 0) {
		ticks = -ticks;
		rc = -1;
	} else {
		rc = 1;
	}

	divisor = (diminuto_ticks_t)COM_DIAG_DIMINUTO_FREQUENCY * 60 * 60 * 24;
	if (dayp    != (int *)0) { *dayp    = ticks / divisor; }
	ticks = ticks % divisor;
	divisor = (diminuto_ticks_t)COM_DIAG_DIMINUTO_FREQUENCY * 60 * 60;
	if (hourp   != (int *)0) { *hourp   = ticks / divisor; }
	ticks = ticks % divisor;
	divisor = (diminuto_ticks_t)COM_DIAG_DIMINUTO_FREQUENCY * 60;
	if (minutep != (int *)0) { *minutep = ticks / divisor; }
	ticks = ticks % divisor;
	divisor = (diminuto_ticks_t)COM_DIAG_DIMINUTO_FREQUENCY;
	if (secondp != (int *)0) { *secondp = ticks / divisor; }
	if (tickp   != (int *)0) { *tickp   = ticks % divisor; }

	return rc;
}
