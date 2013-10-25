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
#include <stdio.h>
#include <time.h>
#include <sys/time.h>

static const int HERTZ = 1000000;

static diminuto_usec_t diminuto_time_microseconds(clockid_t clock)
{
	diminuto_usec_t microseconds = -1;
	struct timespec elapsed;

	if (clock_gettime(clock, &elapsed) < 0) {
		diminuto_perror("diminuto_time_generic: clock_gettime");
	} else {
		microseconds = elapsed.tv_sec;
		microseconds *= HERTZ;
		microseconds += (elapsed.tv_nsec / 1000);
	}

	return microseconds;
}

diminuto_usec_t diminuto_time_clock()
{
    return diminuto_time_microseconds(CLOCK_REALTIME);
}

diminuto_usec_t diminuto_time_elapsed()
{
#if defined(CLOCK_MONOTONIC_RAW)
	return diminuto_time_microseconds(CLOCK_MONOTONIC_RAW); /* Since Linux 2.6.28 */
#else
	return diminuto_time_microseconds(CLOCK_MONOTONIC); /* Prior to Linux 2.6.28 */
#endif
}

diminuto_usec_t diminuto_time_process()
{
	return diminuto_time_microseconds(CLOCK_PROCESS_CPUTIME_ID);
}

diminuto_usec_t diminuto_time_thread()
{
	return diminuto_time_microseconds(CLOCK_THREAD_CPUTIME_ID);
}

diminuto_usec_t diminuto_time_timezone(diminuto_usec_t microseconds)
{
	diminuto_usec_t timezone = -1;
	struct tm datetime;
	struct tm * datetimep;
	time_t juliet;

	microseconds /= HERTZ;
	juliet = microseconds;
	if ((datetimep = localtime_r(&juliet, &datetime)) == (struct tm *)0) {
		diminuto_perror("diminuto_time_timezone: localtime_r");
	} else if (datetimep->tm_isdst) {
		timezone = datetimep->tm_gmtoff;
		timezone -= 3600;
		timezone *= HERTZ;
	} else {
		timezone = datetimep->tm_gmtoff;
		timezone *= HERTZ;
	}

	return timezone;
}

diminuto_usec_t diminuto_time_daylightsaving(diminuto_usec_t microseconds)
{
	diminuto_usec_t daylightsaving = -1;
	struct tm datetime;
	struct tm * datetimep;
	time_t juliet;

	microseconds /= HERTZ;
	juliet = microseconds;
	if ((datetimep = localtime_r(&juliet, &datetime)) == (struct tm *)0) {
		diminuto_perror("diminuto_time_daylightsaving: localtime_r");
	} else if (datetimep->tm_isdst > 0) {
		daylightsaving = 3600;
		daylightsaving *= HERTZ;
	} else {
		daylightsaving = 0;
	}

	return daylightsaving;
}

diminuto_usec_t diminuto_time_epoch(int year, int month, int day, int hour, int minute, int second, int microsecond, diminuto_usec_t timezone, diminuto_usec_t daylightsaving)
{
	diminuto_usec_t microseconds = -1;
	struct tm datetime;
	time_t juliet;

	datetime.tm_year = year;
	datetime.tm_year -= 1900;
	datetime.tm_mon = month;
	datetime.tm_mon -= 1;
	datetime.tm_mday = day;
	datetime.tm_hour = hour;
	datetime.tm_min = minute;
	datetime.tm_sec = second;
	datetime.tm_isdst = 0;
	if ((juliet = mktime(&datetime)) == -1) {
		diminuto_perror("diminuto_time_epoch: mktime");
	} else {
		microseconds = juliet;
		microseconds *= HERTZ;
		microseconds += diminuto_time_timezone(juliet); /* Because mktime(3) assumes local time. */
		microseconds -= timezone;
		microseconds -= daylightsaving;
		microseconds += microsecond;
	}

	return microseconds;
}

static void diminuto_time_stamp(diminuto_usec_t microseconds, const struct tm *datetimep, int * yearp, int * monthp, int * dayp, int * hourp, int * minutep, int * secondp, int * microsecondp)
{
	if (yearp        != (int *)0) { *yearp        = datetimep->tm_year + 1900; }
	if (monthp       != (int *)0) { *monthp       = datetimep->tm_mon + 1;     }
	if (dayp         != (int *)0) { *dayp         = datetimep->tm_mday;        }
	if (hourp        != (int *)0) { *hourp        = datetimep->tm_hour;        }
	if (minutep      != (int *)0) { *minutep      = datetimep->tm_min;         }
	if (secondp      != (int *)0) { *secondp      = datetimep->tm_sec;         }
	if (microsecondp != (int *)0) { *microsecondp = microseconds % HERTZ;      }
}

diminuto_usec_t diminuto_time_zulu(diminuto_usec_t microseconds, int * yearp, int * monthp, int * dayp, int * hourp, int * minutep, int * secondp, int * microsecondp)
{
	struct tm datetime;
	struct tm * datetimep;
	time_t zulu;

	zulu = microseconds / HERTZ;
	if ((datetimep = gmtime_r(&zulu, &datetime)) == (struct tm *)0) {
		diminuto_perror("diminuto_time_timestamp: gmtime_r");
		microseconds = -1;
	} else {
		diminuto_time_stamp(microseconds, datetimep, yearp, monthp, dayp, hourp, minutep, secondp, microsecondp);
	}

	return microseconds;
}

diminuto_usec_t diminuto_time_juliet(diminuto_usec_t microseconds, int * yearp, int * monthp, int * dayp, int * hourp, int * minutep, int * secondp, int * microsecondp)
{
	struct tm datetime;
	struct tm * datetimep;
	time_t juliet;

	juliet = microseconds / HERTZ;
	if ((datetimep = localtime_r(&juliet, &datetime)) == (struct tm *)0) {
		diminuto_perror("diminuto_time_timestamp: localtime_r");
		microseconds = -1;
	} else {
		diminuto_time_stamp(microseconds, datetimep, yearp, monthp, dayp, hourp, minutep, secondp, microsecondp);
	}

	return microseconds;
}
