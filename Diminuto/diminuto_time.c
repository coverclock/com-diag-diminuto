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

static diminuto_usec_t diminuto_time_generic(clockid_t clock)
{
	diminuto_usec_t microseconds = -1;
	struct timespec elapsed;

	if (clock_gettime(clock, &elapsed) < 0) {
		diminuto_perror("diminuto_time_generic: clock_gettime");
	} else {
		microseconds = elapsed.tv_sec;
		microseconds *= 1000000ULL;
		microseconds += (elapsed.tv_nsec / 1000);
	}

	return microseconds;
}

diminuto_usec_t diminuto_time_clock()
{
    return diminuto_time_generic(CLOCK_REALTIME);
}

diminuto_usec_t diminuto_time_elapsed()
{
#if defined(CLOCK_MONOTONIC_RAW)
	return diminuto_time_generic(CLOCK_MONOTONIC_RAW); /* Since Linux 2.6.28 */
#else
	return diminuto_time_generic(CLOCK_MONOTONIC); /* Prior to Linux 2.6.28 */
#endif
}

diminuto_usec_t diminuto_time_process()
{
	return diminuto_time_generic(CLOCK_PROCESS_CPUTIME_ID);
}

diminuto_usec_t diminuto_time_thread()
{
	return diminuto_time_generic(CLOCK_THREAD_CPUTIME_ID);
}

diminuto_usec_t diminuto_time_offset()
{
	diminuto_usec_t microseconds = -1;
	struct tm datetime;
	struct tm * datetimep;
	time_t juliet = 0;

	if ((datetimep = localtime_r(&juliet, &datetime)) == (struct tm *)0) {
		diminuto_perror("diminuto_time_offset: localtime_r");
	} else {
		microseconds = datetimep->tm_gmtoff;
		microseconds *= 1000000;
	}

	return microseconds;
}

int diminuto_time_daylightsaving(void)
{
	int daylightsaving = -1;
	struct timespec juliet;
	struct tm datetime;
	struct tm * datetimep;

	if (clock_gettime(CLOCK_REALTIME, &juliet) < 0) {
		diminuto_perror("diminuto_time_daylightsaving: clock_gettime");
	} else if ((datetimep = localtime_r(&juliet, &datetime)) == (struct tm *)0) {
		diminuto_perror("diminuto_time_daylightsaving: localtime_r");
	} else {
		daylightsaving = datetime.tm_isdst;
	}

	return daylightsaving;
}

diminuto_usec_t diminuto_time_epoch(int year, int month, int day, int hour, int minute, int second, int microsecond, diminuto_usec_t offset, int daylightsaving)
{
	diminuto_usec_t microseconds = -1;
	struct tm datetime;
	time_t juliet;
	extern long timezone;

	datetime.tm_year = year;
	datetime.tm_year -= 1900;
	datetime.tm_mon = month;
	datetime.tm_mon -= 1;
	datetime.tm_mday = day;
	datetime.tm_hour = hour;
	datetime.tm_min = minute;
	datetime.tm_sec = second;
	datetime.tm_isdst = daylightsaving;
	if ((juliet = mktime(&datetime)) == -1) {
		diminuto_perror("diminuto_time_epoch: mktime");
	} else {
		microseconds = juliet;
		microseconds *= 1000000;
		microseconds += diminuto_time_offset();
		microseconds -= offset;
		microseconds += microsecond;
	}

	return microseconds;
}

diminuto_usec_t diminuto_time_zulu(diminuto_usec_t clock, int * yearp, int * monthp, int * dayp, int * hourp, int * minutep, int * secondp, int * microsecondp)
{
	struct tm datetime;
	struct tm * datetimep;
	time_t zulu;

	zulu = clock / 1000000;
	if ((datetimep = gmtime_r(&zulu, &datetime)) == (struct tm *)0) {
		diminuto_perror("diminuto_time_timestamp: gmtime_r");
		clock = -1;
	} else {
		if (yearp        != (int *)0) { *yearp        = datetimep->tm_year + 1900; }
		if (monthp       != (int *)0) { *monthp       = datetimep->tm_mon + 1;     }
		if (dayp         != (int *)0) { *dayp         = datetimep->tm_mday;        }
		if (hourp        != (int *)0) { *hourp        = datetimep->tm_hour;        }
		if (minutep      != (int *)0) { *minutep      = datetimep->tm_min;         }
		if (secondp      != (int *)0) { *secondp      = datetimep->tm_sec;         }
		if (microsecondp != (int *)0) { *microsecondp = clock % 1000000;           }
	}

	return clock;
}

diminuto_usec_t diminuto_time_juliet(diminuto_usec_t clock, int * yearp, int * monthp, int * dayp, int * hourp, int * minutep, int * secondp, int * microsecondp)
{
	struct tm datetime;
	struct tm * datetimep;
	time_t juliet;

	juliet = clock / 1000000;
	if ((datetimep = localtime_r(&juliet, &datetime)) == (struct tm *)0) {
		diminuto_perror("diminuto_time_timestamp: localtime_r");
		clock = -1;
	} else {
		if (yearp        != (int *)0) { *yearp        = datetimep->tm_year + 1900; }
		if (monthp       != (int *)0) { *monthp       = datetimep->tm_mon + 1;     }
		if (dayp         != (int *)0) { *dayp         = datetimep->tm_mday;        }
		if (hourp        != (int *)0) { *hourp        = datetimep->tm_hour;        }
		if (minutep      != (int *)0) { *minutep      = datetimep->tm_min;         }
		if (secondp      != (int *)0) { *secondp      = datetimep->tm_sec;         }
		if (microsecondp != (int *)0) { *microsecondp = clock % 1000000;           }
	}

	return clock;
}
