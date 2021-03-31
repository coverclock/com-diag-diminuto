/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2013-2020 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is a unit test of the Epoch portion of the Time feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is a unit test of the Epoch portion of the Time feature.
 *
 * N.B. This unit test suite really stresses the underlying POSIX and
 * GNU time functions because it specifically tests edge cases where the
 * values are at the limits of what the underlying integer type can hold.
 * Specifically, on the Raspberry Pi SBC,  on Raspbian 8 (glib 2.19) and
 * Raspbian 10 (glibc 2.28), where time_t is only 32-bits, some of these
 * test cases are problematic.
 *
 * REFERENCES
 *
 * Wikipedia, "Unix time", 2014-10-24,
 * <http://en.wikipedia.org/wiki/Unix_time>
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_core.h"
#include "com/diag/diminuto/diminuto_frequency.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_time.h"
#include "com/diag/diminuto/diminuto_types.h"
#include "com/diag/diminuto/diminuto_minmaxof.h"
#include <errno.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>

#define SANITY(_YEAR_, _MONTH_, _DAY_, _HOUR_, _MINUTE_, _SECOND_, _TICK_) \
    do { \
        ASSERT((1901 <= _YEAR_) && (_YEAR_ <= 2038)); \
        ASSERT((1 <= _MONTH_) && (_MONTH_ <= 12)); \
        ASSERT((1 <= _DAY_) && (_DAY_ <= 31)); \
        ASSERT((0 <= _HOUR_) && (_HOUR_ <= 23)); \
        ASSERT((0 <= _MINUTE_) && (_MINUTE_ <= 59)); \
        ASSERT((0 <= _SECOND_) && (_SECOND_ <= 60)); \
        ASSERT((0 <= _TICK_) && (_TICK_ <= 999999999LL)); \
    } while (0)

#define VERIFY(_YEAR_, _MONTH_, _DAY_, _HOUR_, _MINUTE_, _SECOND_, _TICK_) \
    do { \
        EXPECT(zyear == (_YEAR_)); \
        EXPECT(zmonth == (_MONTH_)); \
        EXPECT(zday == (_DAY_)); \
        EXPECT(zhour == (_HOUR_)); \
        EXPECT(zminute == (_MINUTE_)); \
        EXPECT(zsecond == (_SECOND_)); \
        EXPECT(ztick == (_TICK_)); \
    } while (0)

static time_t low  = 0;
static time_t epoch = 0;
static time_t high = 0;

static diminuto_sticks_t minimum = 0;
static diminuto_sticks_t maximum = 0;

static void test1(void)
{
    int rc = -1;
    diminuto_sticks_t now = -1;
    diminuto_sticks_t then = -1;
    diminuto_sticks_t zone = -1;
    diminuto_sticks_t dst = -1;
    diminuto_sticks_t delta = -1;
    int year = -1;
    int month = -1;
    int day = -1;
    int hour = -1;
    int minute = -1;
    int second = -1;
    diminuto_ticks_t tick = (diminuto_ticks_t)-1;

    now = diminuto_time_clock();
    CHECKPOINT("clock %lld\n", (long long int)now);
    ASSERT(now >= 0);

    zone = diminuto_time_timezone();
    CHECKPOINT("timezone %lld\n", (long long int)zone);
    /* zone can be positive or negatuve. */

    dst = diminuto_time_daylightsaving(now);
    CHECKPOINT("daylightsaving  %lld\n", (long long int)dst);
    ASSERT(dst >= 0);

    rc = diminuto_time_zulu(now, &year, &month, &day, &hour, &minute, &second, &tick);
    ASSERT(rc >= 0);
    CHECKPOINT("zulu %04d-%02d-%02dT%02d:%02d:%02d.%09lld\n", year, month, day, hour, minute, second, (long long int)tick);

    SANITY(year, month, day, hour, minute, second, tick);

    then = diminuto_time_epoch(year, month, day, hour, minute, second, tick, 0, 0);
    ASSERT((then >= 0) || (errno == 0));
    CHECKPOINT("epoch %lld\n", (long long int)then);

    delta = then - now;
    CHECKPOINT("delta %lld\n", (long long int)delta);
    ASSERT(delta == 0);
}

static void test2(void)
{
    int rc = -1;
    diminuto_sticks_t now = -1;
    diminuto_sticks_t then = -1;
    diminuto_sticks_t zone = -1;
    diminuto_sticks_t dst = -1;
    diminuto_sticks_t delta = -1;
    int year = -1;
    int month = -1;
    int day = -1;
    int hour = -1;
    int minute = -1;
    int second = -1;
    diminuto_ticks_t tick = (diminuto_ticks_t)-1;

    now = diminuto_time_clock();
    CHECKPOINT("clock %lld\n", (long long int)now);
    ASSERT(now >= 0);

    zone = diminuto_time_timezone();
    CHECKPOINT("timezone %lld\n", (long long int)zone);
    /* zone can be positive or negatuve. */

    dst = diminuto_time_daylightsaving(now);
    CHECKPOINT("daylightsaving %lld\n", (long long int)dst);
    ASSERT(dst >= 0);

    rc = diminuto_time_juliet(now, &year, &month, &day, &hour, &minute, &second, &tick);
    ASSERT(rc >= 0);
    CHECKPOINT("juliet %04d-%02d-%02dT%02d:%02d:%02d.%09llu\n", year, month, day, hour, minute, second, (long long int)tick);

    SANITY(year, month, day, hour, minute, second, tick);

    then = diminuto_time_epoch(year, month, day, hour, minute, second, tick, zone, dst);
    ASSERT((then >= 0) || (errno == 0));
    CHECKPOINT("epoch %lld\n", (long long int)then);

    delta = then - now;
    CHECKPOINT("delta %lld\n", (long long int)delta);
    ASSERT(delta == 0);
}

static int zyear = -1;
static int zmonth = -1;
static int zday = -1;
static int zhour = -1;
static int zminute = -1;
static int zsecond = -1;
static diminuto_ticks_t ztick = (diminuto_ticks_t)-1;
static int notfirst = 0;

static void test3(diminuto_sticks_t now, int verbose)
{
    int dday = -1;
    int dhour = -1;
    int dminute = -1;
    int dsecond = -1;
    diminuto_ticks_t dtick = (diminuto_ticks_t)-1;
    int jyear = -1;
    int jmonth = -1;
    int jday = -1;
    int jhour = -1;
    int jminute = -1;
    int jsecond = -1;
    diminuto_ticks_t jtick = (diminuto_ticks_t)-1;
    diminuto_sticks_t zulu;
    diminuto_sticks_t juliet;
    diminuto_sticks_t timezone;
    diminuto_sticks_t daylightsaving;
    int zh;
    int zm;
    int dh;
    int dm;
    int rc;
    time_t offset;

    zyear = -1;
    zmonth = -1;
    zday = -1;
    zhour = -1;
    zminute = -1;
    zsecond = -1;
    ztick = -1;

    offset = diminuto_frequency_ticks2wholeseconds(now);
    timezone = diminuto_time_timezone();
    daylightsaving = diminuto_time_daylightsaving(now);

    rc = diminuto_time_zulu(now, &zyear, &zmonth, &zday, &zhour, &zminute, &zsecond, &ztick);
    ASSERT(rc == 0);
    SANITY(zyear, zmonth, zday, zhour, zminute, zsecond, ztick);
    zulu = diminuto_time_epoch(zyear, zmonth, zday, zhour, zminute, zsecond, ztick, 0, 0);
    ASSERT((zulu >= 0) || (errno == 0));
    EXPECT(now == zulu);

    /*
     * Not all local times (a.k.a. juliet) are capable of being decoded into
     * YYYYMMDD hhmmss and then encoded back into ticks. The latter operation
     * overflows, especially on targets for which time_t is 32-bits. Worse,
     * for some versions of glibc, a -1 is returned by errno is not set.
     * Diminuto tries to detect this, and we avoid testing tick values within
     * 24 hours of the minimum and maximum that works for UTC (a.k.a. zulu).
     */

    rc = diminuto_time_juliet(now, &jyear, &jmonth, &jday, &jhour, &jminute, &jsecond, &jtick);
    ASSERT(rc == 0);
    SANITY(jyear, jmonth, jday, jhour, jminute, jsecond, jtick);
    if ((minimum <= now) && (now <= maximum)) {
        juliet = diminuto_time_epoch(jyear, jmonth, jday, jhour, jminute, jsecond, jtick, timezone, daylightsaving);
        ASSERT((juliet >= 0) || (errno == 0));
        EXPECT(now == juliet);
    } 

    rc = diminuto_time_duration(now, &dday, &dhour, &dminute, &dsecond, &dtick);
    EXPECT((rc < 0) || (rc > 0));
    if (rc < 0) { dday = -dday; }
    SANITY(2017, 9, 29, dhour, dminute, dsecond, dtick);

    if ((now != zulu) || verbose) {
        if (!notfirst) {
            CHECKPOINT("%20s %20s %20s %018s %30s %30s %25s %15s %15s\n"
                , "NOW", "ZULU", "JULIET", "OFFSET"
                , "ZULU", "JULIET", "DURATION"
                , "TIMEZONE", "DAYLIGHTSAVING");
            notfirst = !0;
        }
        CHECKPOINT("%20lld %20lld %20lld 0x%016llx %4.4d-%2.2d-%2.2dT%2.2d:%2.2d:%2.2d.%9.9lluZ %4.4d-%2.2d-%2.2dT%2.2d:%2.2d:%2.2d.%9.9lluJ %6d/%2.2d:%2.2d:%2.2d.%9.9llu %15lld %15lld\n"
            , (long long int)now, (long long int)zulu, (long long int)juliet, (long long int)offset
            , zyear, zmonth, zday, zhour, zminute, zsecond, (long long unsigned int)ztick
            , jyear, jmonth, jday, jhour, jminute, jsecond, (long long unsigned int)jtick
            , dday, dhour, dminute, dsecond, (long long unsigned int)dtick
            , timezone, daylightsaving
        );
    }
}

int main(int argc, char ** argv)
{
    diminuto_sticks_t ticks;
    diminuto_sticks_t hertz;
    extern char *tzname[2];
    extern long timezone;
    extern int daylight;

    SETLOGMASK();

    {
        static const int32_t LOWEST = diminuto_minimumof(int32_t);
        static const int32_t HIGHEST = diminuto_maximumof(int32_t);
        int year, month, day, hour, minute, second;
        diminuto_ticks_t tick;
        int rc;

        TEST();

        low = LOWEST;
        epoch = 0;
        high = HIGHEST;

        hertz = diminuto_frequency();
        tzset();

        minimum = hertz * (low + (60 * 60 * 24));
        maximum = hertz * (high - (60 * 60 * 24));

        CHECKPOINT("hertz %lld\n", (long long int)hertz);
        CHECKPOINT("timezone %lld\n", (long long int)timezone);
        CHECKPOINT("daylight %lld\n", (long long int)daylight);

        CHECKPOINT("low %lld 0x%llx\n", (long long int)low, (long long int)low);
        ticks = low;
        ticks *= hertz;

        year = month = day = hour = minute = second = tick = 0;
        rc = diminuto_time_zulu(ticks, &year, &month, &day, &hour, &minute, &second, &tick);
        CHECKPOINT("low %02d-%02d-%02dT%02d:%02d:%02d.%09dZ %d %d\n", year, month, day, hour, minute, second, tick, rc, errno);

        year = month = day = hour = minute = second = tick = 0;
        rc = diminuto_time_juliet(ticks, &year, &month, &day, &hour, &minute, &second, &tick);
        CHECKPOINT("low %02d-%02d-%02dT%02d:%02d:%02d.%09dJ %d %d\n", year, month, day, hour, minute, second, tick, rc, errno);

        CHECKPOINT("minimum %lld 0x%llx\n", (long long int)minimum / hertz, (long long int) minimum / hertz);
        ticks = minimum;

        year = month = day = hour = minute = second = tick = 0;
        rc = diminuto_time_zulu(ticks, &year, &month, &day, &hour, &minute, &second, &tick);
        CHECKPOINT("minimum %02d-%02d-%02dT%02d:%02d:%02d.%09dZ %d %d\n", year, month, day, hour, minute, second, tick, rc, errno);

        year = month = day = hour = minute = second = tick = 0;
        rc = diminuto_time_juliet(ticks, &year, &month, &day, &hour, &minute, &second, &tick);
        CHECKPOINT("minimum %02d-%02d-%02dT%02d:%02d:%02d.%09dJ %d %d\n", year, month, day, hour, minute, second, tick, rc, errno);

        CHECKPOINT("epoch %lld 0x%llx\n", (long long int)epoch, (long long int)epoch);
        ticks = epoch;
        ticks *= hertz;

        year = month = day = hour = minute = second = tick = 0;
        rc = diminuto_time_zulu(ticks, &year, &month, &day, &hour, &minute, &second, &tick);
        CHECKPOINT("epoch %02d-%02d-%02dT%02d:%02d:%02d.%09dZ %d %d\n", year, month, day, hour, minute, second, tick, rc, errno);

        year = month = day = hour = minute = second = tick = 0;
        rc = diminuto_time_juliet(ticks, &year, &month, &day, &hour, &minute, &second, &tick);
        CHECKPOINT("epoch %02d-%02d-%02dT%02d:%02d:%02d.%09dJ %d %d\n", year, month, day, hour, minute, second, tick, rc, errno);

        CHECKPOINT("maximum %lld 0x%llx\n", (long long int)maximum / hertz, (long long int)maximum / hertz);
        ticks = maximum;

        year = month = day = hour = minute = second = tick = 0;
        rc = diminuto_time_zulu(ticks, &year, &month, &day, &hour, &minute, &second, &tick);
        CHECKPOINT("maximum %02d-%02d-%02dT%02d:%02d:%02d.%09dZ %d %d\n", year, month, day, hour, minute, second, tick, rc, errno);

        year = month = day = hour = minute = second = tick = 0;
        rc = diminuto_time_juliet(ticks, &year, &month, &day, &hour, &minute, &second, &tick);
        CHECKPOINT("maximum %02d-%02d-%02dT%02d:%02d:%02d.%09dJ %d %d\n", year, month, day, hour, minute, second, tick, rc, errno);

        CHECKPOINT("high %lld 0x%llx\n", (long long int)high, (long long int)high);
        ticks = high;
        ticks *= hertz;

        year = month = day = hour = minute = second = tick = 0;
        rc = diminuto_time_zulu(ticks, &year, &month, &day, &hour, &minute, &second, &tick);
        CHECKPOINT("high %02d-%02d-%02dT%02d:%02d:%02d.%09dZ %d %d\n", year, month, day, hour, minute, second, tick, rc, errno);

        year = month = day = hour = minute = second = tick = 0;
        rc = diminuto_time_juliet(ticks, &year, &month, &day, &hour, &minute, &second, &tick);
        CHECKPOINT("high %02d-%02d-%02dT%02d:%02d:%02d.%09dJ %d %d\n", year, month, day, hour, minute, second, tick, rc, errno);

        STATUS();
    }

    {
        TEST();

        test1();

        STATUS();
    }

    {
        TEST();

        test2();

        STATUS();
    }

    {
        TEST();

        /*
         * See also fun/timestuff.c
         */

        CHECKPOINT();
        test3((ticks = low) * hertz, !0);
        VERIFY(1901, 12, 13, 20, 45, 52, 0);

        CHECKPOINT();
        notfirst = 0;
        test3((ticks = minimum), !0);
        VERIFY(1901, 12, 14, 20, 45, 52, 0);

        CHECKPOINT();
        notfirst = 0;
        test3(ticks = epoch, !0);
        VERIFY(1970, 1, 1, 0, 0, 0, 0);

        CHECKPOINT();
        notfirst = 0;
        test3(ticks = 1, !0);
        VERIFY(1970, 1, 1, 0, 0, 0, 1);

        CHECKPOINT();
        notfirst = 0;
        test3(ticks = hertz - 1, !0);
        VERIFY(1970, 1, 1, 0, 0, 0, hertz - 1);

        CHECKPOINT();
        notfirst = 0;
        test3(ticks = hertz, !0);
        VERIFY(1970, 1, 1, 0, 0, 1, 0);

        CHECKPOINT();
        notfirst = 0;
        test3(ticks = -hertz, !0);
        VERIFY(1969, 12, 31, 23, 59, 59, 0);

        CHECKPOINT();
        notfirst = 0;
        test3(ticks = 1000000000LL * hertz, !0);
        VERIFY(2001, 9, 9, 1, 46, 40, 0);

        CHECKPOINT();
        notfirst = 0;
        test3(ticks = 1234567890LL * hertz, !0);
        VERIFY(2009, 2, 13, 23, 31, 30, 0);

        CHECKPOINT();
        notfirst = 0;
        test3(ticks = 15000LL * 24LL * 3600LL * hertz, !0);
        VERIFY(2011, 1, 26, 0, 0, 0, 0);

        CHECKPOINT();
        notfirst = 0;
        test3(ticks = 1400000000LL * hertz, !0);
        VERIFY(2014, 5, 13, 16, 53, 20, 0);

        CHECKPOINT();
        notfirst = 0;
        test3((ticks = maximum), !0);
        VERIFY(2038, 1, 18, 3, 14, 7, 0);

        CHECKPOINT();
        notfirst = 0;
        test3((ticks = high) * hertz, !0);
        VERIFY(2038, 1, 19, 3, 14, 7, 0);

        STATUS();
    }

    {
        TEST();

        notfirst = 0;

        for (ticks = low; ticks <= high; ticks += (365 * 24 * 60 * 60)) {
            test3(ticks * hertz, 0);
        }

        STATUS();
    }

    {
        TEST();

        notfirst = 0;

        for (ticks = low; ticks <= high; ticks += (24 * 60 * 60)) {
            test3(ticks * hertz, 0);
        }

        STATUS();
    }

    {
        TEST();

        notfirst = 0;

        for (ticks = low; ticks <= high; ticks += (60 * 60)) {
            test3(ticks * hertz, 0);
        }

        STATUS();
    }

    {
        TEST();

        notfirst = 0;

        for (ticks = low; ticks <= high; ticks += 60) {
            test3(ticks * hertz, 0);
        }

        STATUS();
    }

    {
        TEST();

        notfirst = 0;

        for (ticks = low; ticks <= high; ticks += 1) {
            test3(ticks * hertz, 0);
        }

        STATUS();
    }

    EXIT();
}
