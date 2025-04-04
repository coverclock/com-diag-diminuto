/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2008-2024 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is the implementation of the Time feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is the implementation of the Time feature.
 */

#include "com/diag/diminuto/diminuto_time.h"
#include "com/diag/diminuto/diminuto_absolute.h"
#include "com/diag/diminuto/diminuto_countof.h"
#include "com/diag/diminuto/diminuto_criticalsection.h"
#include "com/diag/diminuto/diminuto_environment.h"
#include "com/diag/diminuto/diminuto_frequency.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_platform.h"
#include "com/diag/diminuto/diminuto_policy.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include "../src/diminuto_time.h"

/**
 * Return the number of ticks since the epoch using a POSIX real-time clock.
 * @param clock is the POSIX clock type to use.
 * @param logging is true if this is being used by the Log feature.
 * @return ticks since the epoch or DIMINUTO_TIME_ERROR if an error occurred.
 */
static diminuto_sticks_t diminuto_time_generic(clockid_t clock, int logging)
{
    diminuto_sticks_t ticks = COM_DIAG_DIMINUTO_TIME_ERROR;
    struct timespec elapsed = { 0, };

    if (clock_gettime(clock, &elapsed) >= 0) {
        ticks = diminuto_frequency_seconds2ticks(elapsed.tv_sec, elapsed.tv_nsec, 1000000000LL);
    } else if (logging) {
        /* Do nothing. */
    } else {
        diminuto_perror("diminuto_time_generic: clock_gettime");
    }

    return ticks;
}

diminuto_sticks_t diminuto_time_clock()
{
    return diminuto_time_generic((clockid_t)DIMINUTO_POLICY_CLOCK_REALTIME, 0);
}

diminuto_sticks_t diminuto_time_logging()
{
    return diminuto_time_generic((clockid_t)DIMINUTO_POLICY_CLOCK_REALTIME, !0);
}

diminuto_sticks_t diminuto_time_atomic()
{
    return diminuto_time_generic((clockid_t)DIMINUTO_POLICY_CLOCK_TAI, 0);
}

diminuto_sticks_t diminuto_time_elapsed()
{
    return diminuto_time_generic((clockid_t)DIMINUTO_POLICY_CLOCK_BOOTTIME, 0); /* Linux 2.6.39 */
}

diminuto_sticks_t diminuto_time_process()
{
    return diminuto_time_generic((clockid_t)DIMINUTO_POLICY_CLOCK_PROCESS_CPUTIME_ID, 0);
}

diminuto_sticks_t diminuto_time_thread()
{
    return diminuto_time_generic((clockid_t)DIMINUTO_POLICY_CLOCK_THREAD_CPUTIME_ID, 0);
}

diminuto_sticks_t diminuto_time_timezone()
{
    diminuto_sticks_t result = 0;
    diminuto_sticks_t west = 0;
    extern long timezone;
    extern int daylight;

    /*
     * tzset(3) is an expensive operation, but at least in glibc it looks like
     * it's protected with a lock and only does the expensive part once. So
     * we'll call it every time. Note that tzset() has no error return.
     */

    DIMINUTO_ENVIRONMENT_READER_BEGIN;
        tzset();
    DIMINUTO_ENVIRONMENT_READER_END;

    /*
     * POSIX specifies the number of seconds WEST of UTC, while this function
     * returns the ISO8601 sense of seconds in ticks AHEAD (>0) or BEHIND
     * (<0) UTC. West will be behind UTC, East ahead of UTC.
     */

    west = -timezone;

    result = diminuto_frequency_seconds2ticks(west, 0, 1);

    return result;
}

diminuto_sticks_t diminuto_time_daylightsaving(diminuto_sticks_t ticks)
{
    diminuto_sticks_t result = 0;
    diminuto_sticks_t dst = 0;
    struct tm datetime = { 0, };
    struct tm * tmp = (struct tm *)0;
    time_t juliet = 0;
    extern long timezone;
    extern int daylight;

    /*
     * tzset(3) is an expensive operation, but at least in glibc it looks like
     * it's protected with a lock and only does the expensive part once. So
     * we'll call it every time. Note that tzset() has no error return.
     */

    DIMINUTO_ENVIRONMENT_READER_BEGIN;
        tzset();
    DIMINUTO_ENVIRONMENT_READER_END;

    if (daylight) {
        juliet = diminuto_frequency_ticks2wholeseconds(ticks);
        DIMINUTO_ENVIRONMENT_READER_BEGIN;
            tmp = localtime_r(&juliet, &datetime);
        DIMINUTO_ENVIRONMENT_READER_END;
        if (tmp == (struct tm *)0) {
            diminuto_perror("diminuto_time_daylightsaving: localtime_r");
            result = DIMINUTO_TIME_ERROR;
        } else {
            if (datetime.tm_isdst) {
                dst = 3600;
            }
            result = diminuto_frequency_seconds2ticks(dst, 0, 1);
        }
    }

    return result;
}

diminuto_sticks_t diminuto_time_epoch(int year, int month, int day, int hour, int minute, int second, diminuto_ticks_t tick, diminuto_sticks_t timezone, diminuto_sticks_t daylightsaving)
{
    diminuto_sticks_t ticks = DIMINUTO_TIME_ERROR;
    struct tm datetime = { 0, };
    time_t seconds = 0;

    datetime.tm_year = year - 1900;
    datetime.tm_mon = month - 1;
    datetime.tm_mday = day;
    datetime.tm_hour = hour;
    datetime.tm_min = minute;
    datetime.tm_sec = second;
    datetime.tm_isdst = 0;

    do {

        /*
         * timegm(3) is a GNU extension since 2.19. It might be non-standard
         * but it sure solves a thorny problem. Note that -1 is an error
         * return, but it is also a legitimate return value indicating one
         * second prior to the Epoch. So we check the errno value as well
         * and hope the function sets it.
         */

        errno = 0;
        DIMINUTO_ENVIRONMENT_READER_BEGIN;
            seconds = timegm(&datetime);
        DIMINUTO_ENVIRONMENT_READER_END;
        if (seconds != (time_t)-1) {
            /* Do nothing. */
        } else if (errno == 0) {
            /* Do nothing. */
        } else {
            diminuto_perror("diminuto_time_epoch: timegm");
            break;
        }

        ticks = diminuto_frequency_seconds2ticks(seconds, 0, 1);

        /*
         * The manual pages for both mktime(3) and timegm(3) specify that
         * these functions may return -1 (which in other circumstances is a
         * completely legitimate return value and not an error) with errno
         * set to EOVERFLOW. However, on some targets (e.g. ARMv7), in some
         * versions of glibc (8.3), in which time_t is 32-bits, -1 apparently
         * can be returned as an error without errno being set. Both functions
         * work correctly on other targets (e.g. x86_64), in other versions
         * (9.3), in which time_t is 64-bits. We try to detect this here, by
         * relying on the fact that is either function returns -1, then the
         * year must have been 1969 in the second right before the POSIX Epoch
         * (1970-01-01 00:00:00). The first leap second was added in 1972, so
         * we shouldn't have to worry about that here. With a 32-bit signed
         * time_t, dates from 1901/12/13 20:45:52 UTC to 2038/01/19 03:14:07
         * (or about 68 years on either side of 1970/01/01) should be
         * encodeable. See (and run) the functional test fun/timestuff.c for
         * more information.
         */

        if (seconds != (time_t)-1) {
            /* Do nothing. */
        } else if ((year == 1969) && (month == 12) && (day == 31) && (hour == 23) && (minute == 59) && (second == 59)) {
            /* Do nothing. */
        } else {
            DIMINUTO_LOG_DEBUG("diminuto_time_epoch: overflow %02d/%02d/%02d %02d:%02d:%02d.%09llu %lld %lld\n", year, month, day, hour, minute, second, (diminuto_llu_t)tick, (diminuto_lld_t)timezone, (diminuto_lld_t)daylightsaving);
            errno = EOVERFLOW;
            diminuto_perror("diminuto_time_epoch: overflow");
            ticks = DIMINUTO_TIME_ERROR;
            break;
        }

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
         * time_t is a signed type, and the time_t value returned by mktime(3)
         * or timegm(3) may legitimately be negative, indicating a reverse
         * offset from the Epoch. That's why this function returns a signed
         * value, why the error return is the special negative value
         * DIMINUTO_TIME_ERROR, and why we have to check the errno returned
         * by mktime(3) and timegm(3). The fractional tick value is always
         * positive, so we add it to the signed ticks, which will move either
         * a positive or negative result forward on the time line.
         */

        ticks += tick;

    } while (0);

    return ticks;
}

/**
 * Separate the whole seconds and fractional seconds in ticks from a full
 * ticks value. In the case that the ticks value is negative (which is
 * valid, indicating an offset before the Epoch), the fractional seconds
 * is converted from a reverse offset into a forward offset by borrowing
 * a second, preparing it to be the decimal fractional seconds part of a
 * time stamp.
 * @param ticks is the full ticks value.
 * @param fractionp points to the fractional seconds variable in ticks.
 * @return the whole seconds.
 */
static time_t diminuto_time_separate(diminuto_sticks_t ticks, diminuto_ticks_t * fractionp)
{
    time_t seconds = 0;
    diminuto_ticks_t fraction = 0;

    seconds = diminuto_frequency_ticks2wholeseconds(ticks);
    fraction = diminuto_frequency_ticks2fractionalseconds(abs64(ticks), diminuto_frequency());
    /*
     * Note that we *must* check ticks here, not seconds. The former can be
     * negative (but less than one second) but the latter might be zero (and
     * hence effectively positive).
     */
    if (ticks >=0) {
        /* Do nothing. */
    } else if (fraction == 0) {
        /* Do nothing. */
    } else {
        seconds -= 1;
        fraction = diminuto_frequency() - fraction;
    }
    *fractionp = fraction;

    return seconds;
}

/**
 * Extract the portions of the tm structure into separate variables.
 * @param datetimep points to a tm structure.
 * @param ticks is the fractions of a second in ticks.
 * @param yearp points to the year variable.
 * @param monthp points to the month variable.
 * @param dayp points to the day of the month variable.
 * @param hourp points to the hour variable.
 * @param minutep points to the minute variable.
 * @param secondp points to the second variable.
 * @param tickp points to the fraction of a second in ticks variable.
 */
static void diminuto_time_extract(const struct tm *datetimep, diminuto_ticks_t ticks, int * yearp, int * monthp, int * dayp, int * hourp, int * minutep, int * secondp, diminuto_ticks_t * tickp)
{
    if (yearp != (int *)0) { *yearp = datetimep->tm_year + 1900; }
    if (monthp != (int *)0) { *monthp = datetimep->tm_mon + 1; }
    if (dayp != (int *)0) { *dayp = datetimep->tm_mday; }
    if (hourp != (int *)0) { *hourp = datetimep->tm_hour; }
    if (minutep != (int *)0) { *minutep = datetimep->tm_min; }
    if (secondp != (int *)0) { *secondp = datetimep->tm_sec; }
    if (tickp != (diminuto_ticks_t *)0) { *tickp = ticks; }
}

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
 * @param logging is true if this is being used by the Log feature.
 * @return 0 for success, <0 otherwise.
 */
static int diminuto_time_zulu_generic(diminuto_sticks_t ticks, int * yearp, int * monthp, int * dayp, int * hourp, int * minutep, int * secondp, diminuto_ticks_t * tickp, int logging)
{
    int rc = -1;
    struct tm datetime = { 0, };
    struct tm * datetimep = (struct tm *)0;
    time_t zulu = 0;
    diminuto_ticks_t fraction = 0;

    zulu = diminuto_time_separate(ticks, &fraction);
    DIMINUTO_ENVIRONMENT_READER_BEGIN;
        datetimep = gmtime_r(&zulu, &datetime);
    DIMINUTO_ENVIRONMENT_READER_END;
    if (datetimep != (struct tm *)0) {
        diminuto_time_extract(datetimep, fraction, yearp, monthp, dayp, hourp, minutep, secondp, tickp);
        rc = 0;
    } else if (logging) {
        /* Do nothing. */
    } else {
        diminuto_perror("diminuto_time_timestamp: gmtime_r");
    }

    return rc;
}

int diminuto_time_zulu(diminuto_sticks_t ticks, int * yearp, int * monthp, int * dayp, int * hourp, int * minutep, int * secondp, diminuto_ticks_t * tickp)
{
    return diminuto_time_zulu_generic(ticks, yearp, monthp, dayp, hourp, minutep, secondp, tickp, 0);
}

int diminuto_time_zulu_logging(diminuto_sticks_t ticks, int * yearp, int * monthp, int * dayp, int * hourp, int * minutep, int * secondp, diminuto_ticks_t * tickp)
{
    return diminuto_time_zulu_generic(ticks, yearp, monthp, dayp, hourp, minutep, secondp, tickp, !0);
}

int diminuto_time_juliet(diminuto_sticks_t ticks, int * yearp, int * monthp, int * dayp, int * hourp, int * minutep, int * secondp, diminuto_ticks_t * tickp)
{
    int rc = 0;
    struct tm datetime = { 0, };
    struct tm * datetimep = (struct tm *)0;
    time_t juliet = 0;
    diminuto_ticks_t fraction = 0;

    juliet = diminuto_time_separate(ticks, &fraction);
    DIMINUTO_ENVIRONMENT_READER_BEGIN;
        datetimep = localtime_r(&juliet, &datetime);
    DIMINUTO_ENVIRONMENT_READER_END;
    if (datetimep == (struct tm *)0) {
        diminuto_perror("diminuto_time_timestamp: localtime_r");
        rc = -1;
    } else {
        diminuto_time_extract(datetimep, fraction, yearp, monthp, dayp, hourp, minutep, secondp, tickp);
    }

    return rc;
}

/*
 * https://en.wikipedia.org/wiki/List_of_military_time_zones, 2018-04-26
 */
char diminuto_time_zonename(diminuto_sticks_t ticks) {
    char name = 'J';
    diminuto_sticks_t factor = 0;
    static char NAMES[] = {
        'Y', 'X', 'W', 'V', 'U',
        'T', 'S', 'R', 'Q', 'P',
        'O', 'N', 'Z', 'A', 'B',
        'C', 'D', 'E', 'F', 'G',
        'H', 'I', 'K', 'L', 'M'
    };

    factor = diminuto_frequency();
    factor *= 3600;
    if ((ticks % factor) == 0) {
        ticks /= factor;
        ticks += 12;
        if ((0 <= ticks) && (ticks < countof(NAMES))) {
            name = NAMES[ticks];
        }
    }

    return name;
}

int diminuto_time_duration(diminuto_sticks_t ticks, int * dayp, int * hourp, int * minutep, int * secondp, diminuto_ticks_t * tickp)
{
    int rc = 0;
    diminuto_sticks_t divisor = 0;

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

static pthread_mutex_t diminuto_time_logical_mutex = PTHREAD_MUTEX_INITIALIZER;
static uint64_t diminuto_time_logical_counter = 0;
static int diminuto_time_logical_errno = 0;

uint64_t diminuto_time_logical(void)
{
    uint64_t result = 0;

    DIMINUTO_CRITICAL_SECTION_BEGIN(&diminuto_time_logical_mutex);
        result = diminuto_time_logical_counter++;
        if (diminuto_time_logical_errno != 0) {
            errno = diminuto_time_logical_errno;
            diminuto_perror("diminuto_time_logical");
        } else if (diminuto_time_logical_counter == 0) {
            errno = 0;
            diminuto_time_logical_errno = EOVERFLOW;
        } else {
            errno = 0;
        }
    DIMINUTO_CRITICAL_SECTION_END;

    return result;
}

void diminuto_time_logical_reset(void)
{
    DIMINUTO_CRITICAL_SECTION_BEGIN(&diminuto_time_logical_mutex);
        diminuto_time_logical_errno = 0;
    DIMINUTO_CRITICAL_SECTION_END;
}

/*
 * For unit testing only.
 */
void diminuto_time_logical_set(uint64_t value)
{
    DIMINUTO_CRITICAL_SECTION_BEGIN(&diminuto_time_logical_mutex);
        diminuto_time_logical_counter = value;
        diminuto_time_logical_errno = 0;
    DIMINUTO_CRITICAL_SECTION_END;
}
