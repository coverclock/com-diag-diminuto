/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2008-2015 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "com/diag/diminuto/diminuto_time.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_frequency.h"
#include "com/diag/diminuto/diminuto_platform.h"
#include "com/diag/diminuto/diminuto_criticalsection.h"
#include <stdio.h>
#include <time.h>
#include <errno.h>
#include <sys/time.h>

#if (defined(_POSIX_TIMERS) && (_POSIX_TIMERS > 0))

static diminuto_sticks_t diminuto_time_generic(clockid_t clock)
{
    diminuto_sticks_t ticks = -1;
    struct timespec elapsed;

    if (clock_gettime(clock, &elapsed) < 0) {
        diminuto_perror("diminuto_time_generic: clock_gettime");
    } else {
        ticks = diminuto_frequency_seconds2ticks(elapsed.tv_sec, elapsed.tv_nsec, diminuto_time_frequency());
    }

    return ticks;
}

#else

#   warning POSIX real-time clocks not available on this platform!

static diminuto_sticks_t diminuto_time_generic(int32_t clock)
{
    diminuto_sticks_t ticks = -1;
    struct timeval elapsed;

    if (gettimeofday(&elapsed, (void *)0) < 0) {
        diminuto_perror("diminuto_time_generic: gettimeofday");
    } else {
        ticks = diminuto_frequency_seconds2ticks(elapsed.tv_sec, elapsed.tv_usec, diminuto_time_frequency());
    }

    return ticks;
}

#endif

/*
 * IMPORTANT SAFETY TIP: you cannot use the Diminuto logging functions from
 * within this function since the logging functions call this function; to do
 * so will cause an infinite recursion (at least until you run out of stack
 * space).
 */
diminuto_sticks_t diminuto_time_clock()
{
#if (defined(_POSIX_TIMERS) && (_POSIX_TIMERS > 0))
    return diminuto_time_generic(CLOCK_REALTIME);
#else
    return diminuto_time_generic(0);
#endif
}

diminuto_sticks_t diminuto_time_elapsed()
{
#if 0
    return diminuto_time_generic(CLOCK_MONOTONIC_BOOTTIME); /* Since Linux 2.6.39 */
#elif (defined(CLOCK_MONOTONIC_RAW) && defined(_POSIX_TIMERS) && (_POSIX_TIMERS > 0))
    return diminuto_time_generic(CLOCK_MONOTONIC_RAW); /* Since Linux 2.6.28 */
#elif (defined(CLOCK_MONOTONIC) && defined(_POSIX_TIMERS) && (_POSIX_TIMERS > 0))
    return diminuto_time_generic(CLOCK_MONOTONIC); /* Prior to Linux 2.6.28 */
#else
    return diminuto_time_generic(0);
#endif
}

diminuto_sticks_t diminuto_time_process()
{
#if defined(CLOCK_PROCESS_CPUTIME_ID)
    return diminuto_time_generic(CLOCK_PROCESS_CPUTIME_ID);
#else
    errno = ENOSYS;
    return -1;
#endif
}

diminuto_sticks_t diminuto_time_thread()
{
#if defined(CLOCK_THREAD_CPUTIME_ID)
    return diminuto_time_generic(CLOCK_THREAD_CPUTIME_ID);
#else
    errno = ENOSYS;
    return -1;
#endif
}

diminuto_sticks_t diminuto_time_timezone(diminuto_sticks_t ticks)
{
    diminuto_sticks_t west;
    extern long timezone;
    extern int daylight;

    /*
     * tzset(3) is an expensive operation, but at least in glibc it looks like
     * it's protected with a lock and only does the expensive part once. So
     * we'll call it every time.
     */
    tzset();

    /*
     * POSIX specifies the number of seconds WEST of UTC, while this function
     * returns the ISO-8601 sense of seconds (or ticks) ahead (>0) or behind
     * (<0) UTC.
     */

    west = -timezone;

    return diminuto_frequency_seconds2ticks(west, 0, 1);
}

diminuto_sticks_t diminuto_time_daylightsaving(diminuto_sticks_t ticks)
{
    diminuto_sticks_t dst = 0;
    struct tm datetime;
    time_t juliet;
    extern int daylight;

    /*
     * tzset(3) is an expensive operation, but at least in glibc it looks like
     * it's protected with a lock and only does the expensive part once. So
     * we'll call it every time.
     */
    tzset();

    if (daylight) {
        juliet = diminuto_frequency_ticks2wholeseconds(ticks);
        if (localtime_r(&juliet, &datetime) == (struct tm *)0) {
            diminuto_perror("diminuto_time_daylightsaving: localtime_r");
        } else if (datetime.tm_isdst) {
            dst = 3600;
        } else {
            /* Do nothing. */
        }
    }

    return diminuto_frequency_seconds2ticks(dst, 0, 1);
}

diminuto_sticks_t diminuto_time_epoch(int year, int month, int day, int hour, int minute, int second, int tick, diminuto_sticks_t timezone, diminuto_sticks_t daylightsaving)
{
    diminuto_sticks_t ticks;
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
    ticks = diminuto_frequency_seconds2ticks(juliet, 0, 1);

    /*
     * mktime(3) assumes the time in the tm structure is local time and
     * adjusts the number of seconds it returns accordingly. There doesn't
     * seem to be an API call to generate epoch seconds in terms of UTC or
     * some other time zone. And mktime(3) doesn't respect the time zone
     * that can optionally be part of the tm structure. So we have to make
     * our own adjustments to eliminate the effects of the local time zone
     * and of DST. This is really stupid.
     */

    ticks += diminuto_time_timezone(juliet);
    ticks += diminuto_time_daylightsaving(juliet);

    /*
     * The caller provides a date and time for some time zone. It might
     * not have been UTC. So we ask that the caller provides the time
     * zone and DST offset so we can back them out, since the Epoch is
     * always in UTC. If the date and time were in local time (juliet),
     * then we are just undoing what we just did above.
     */

    ticks -= timezone;
    ticks -= daylightsaving;

    /*
     * Finally, we add in the fractional second provided by the caller.
     */

    ticks += tick;

    return ticks;
}

static void diminuto_time_stamp(const struct tm *datetimep, diminuto_sticks_t ticks, int * yearp, int * monthp, int * dayp, int * hourp, int * minutep, int * secondp, diminuto_ticks_t * tickp)
{
    if (yearp   != (int *)0) { *yearp   = datetimep->tm_year + 1900;    }
    if (monthp  != (int *)0) { *monthp  = datetimep->tm_mon + 1;        }
    if (dayp    != (int *)0) { *dayp    = datetimep->tm_mday;            }
    if (hourp   != (int *)0) { *hourp   = datetimep->tm_hour;            }
    if (minutep != (int *)0) { *minutep = datetimep->tm_min;            }
    if (secondp != (int *)0) { *secondp = datetimep->tm_sec;            }
    if (tickp   != (diminuto_ticks_t *)0) { *tickp = ticks % diminuto_frequency(); }
}

/*
 * IMPORTANT SAFETY TIP: you cannot use the Diminuto logging functions from
 * within this function since the logging functions call this function; to do
 * so will cause an infinite recursion (at least until you run out of stack
 * space).
 */
int diminuto_time_zulu(diminuto_sticks_t ticks, int * yearp, int * monthp, int * dayp, int * hourp, int * minutep, int * secondp, diminuto_ticks_t * tickp)
{
    int rc = 0;
    struct tm datetime;
    struct tm * datetimep;
    time_t zulu;

    zulu = diminuto_frequency_ticks2wholeseconds(ticks);
    if ((datetimep = gmtime_r(&zulu, &datetime)) == (struct tm *)0) {
        diminuto_perror("diminuto_time_timestamp: gmtime_r");
        rc = -1;
    } else {
        diminuto_time_stamp(datetimep, ticks, yearp, monthp, dayp, hourp, minutep, secondp, tickp);
    }

    return rc;
}

int diminuto_time_juliet(diminuto_sticks_t ticks, int * yearp, int * monthp, int * dayp, int * hourp, int * minutep, int * secondp, diminuto_ticks_t * tickp)
{
    int rc = 0;
    struct tm datetime;
    struct tm * datetimep;
    time_t juliet;

    juliet = diminuto_frequency_ticks2wholeseconds(ticks);
    if ((datetimep = localtime_r(&juliet, &datetime)) == (struct tm *)0) {
        diminuto_perror("diminuto_time_timestamp: localtime_r");
        rc = -1;
    } else {
        diminuto_time_stamp(datetimep, ticks, yearp, monthp, dayp, hourp, minutep, secondp, tickp);
    }

    return rc;
}

int diminuto_time_duration(diminuto_sticks_t ticks, int * dayp, int * hourp, int * minutep, int * secondp, diminuto_ticks_t * tickp)
{
    int rc;
    diminuto_sticks_t divisor;

    if (ticks < 0) {
        ticks = -ticks;
        rc = -1;
    } else {
        rc = 1;
    }

    divisor = diminuto_frequency() * 60 * 60 * 24;
    if (dayp    != (int *)0) { *dayp    = ticks / divisor; }
    ticks = ticks % divisor;
    divisor = diminuto_frequency() * 60 * 60;
    if (hourp   != (int *)0) { *hourp   = ticks / divisor; }
    ticks = ticks % divisor;
    divisor = diminuto_frequency() * 60;
    if (minutep != (int *)0) { *minutep = ticks / divisor; }
    ticks = ticks % divisor;
    divisor = diminuto_frequency();
    if (secondp != (int *)0) { *secondp = ticks / divisor; }
    if (tickp   != (diminuto_ticks_t *)0) { *tickp = ticks % divisor; }

    return rc;
}

uint64_t diminuto_time_logical(void)
{
    static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    static uint64_t counter = 0;
    uint64_t result;

    DIMINUTO_CRITICAL_SECTION_BEGIN(&mutex);
        result = counter++;
    DIMINUTO_CRITICAL_SECTION_END;

    return result;
}
