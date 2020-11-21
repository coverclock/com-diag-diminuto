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
#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_core.h"
#include "com/diag/diminuto/diminuto_time.h"
#include "com/diag/diminuto/diminuto_frequency.h"
#include "com/diag/diminuto/diminuto_types.h"
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

static void sanity0(void)
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
    printf("clock          %lld\n", (long long int)now);
    ASSERT(now >= 0);

    zone = diminuto_time_timezone(now);
    printf("timezone       %lld\n", (long long int)zone);
    /* zone can be positive or negatuve. */

    dst = diminuto_time_daylightsaving(now);
    printf("daylightsaving %lld\n", (long long int)dst);
    ASSERT(dst >= 0);

    rc = diminuto_time_zulu(now, &year, &month, &day, &hour, &minute, &second, &tick);
    ASSERT(rc >= 0);
    printf("zulu           %04d-%02d-%02dT%02d:%02d:%02d.%09lld\n", year, month, day, hour, minute, second, (long long int)tick);

    SANITY(year, month, day, hour, minute, second, tick);

    then = diminuto_time_epoch(year, month, day, hour, minute, second, tick, 0, 0);
    ASSERT(then >= 0);
    printf("epoch          %lld\n", (long long int)then);

    delta = then - now;
    printf("delta          %lld\n", (long long int)delta);
    ASSERT(delta == 0);
}

static void sanity1(void)
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
    printf("clock          %lld\n", (long long int)now);
    ASSERT(now >= 0);

    zone = diminuto_time_timezone(now);
    printf("timezone       %lld\n", (long long int)zone);
    /* zone can be positive or negatuve. */

    dst = diminuto_time_daylightsaving(now);
    printf("daylightsaving %lld\n", (long long int)dst);
    ASSERT(dst >= 0);

    rc = diminuto_time_juliet(now, &year, &month, &day, &hour, &minute, &second, &tick);
    ASSERT(rc >= 0);
    printf("juliet         %04d-%02d-%02dT%02d:%02d:%02d.%09llu\n", year, month, day, hour, minute, second, (long long int)tick);

    SANITY(year, month, day, hour, minute, second, tick);

    then = diminuto_time_epoch(year, month, day, hour, minute, second, tick, zone, dst);
    ASSERT(then >= 0);
    printf("epoch          %lld\n", (long long int)then);

    delta = then - now;
    printf("delta          %lld\n", (long long int)delta);
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
static int prior = -1;

static void epoch(diminuto_sticks_t now, int verbose)
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
    diminuto_time_zulu(now, &zyear, &zmonth, &zday, &zhour, &zminute, &zsecond, &ztick);
    zulu = diminuto_time_epoch(zyear, zmonth, zday, zhour, zminute, zsecond, ztick, 0, 0);
    timezone = diminuto_time_timezone(now);
    daylightsaving = diminuto_time_daylightsaving(now);
    diminuto_time_juliet(now, &jyear, &jmonth, &jday, &jhour, &jminute, &jsecond, &jtick);
    juliet = diminuto_time_epoch(jyear, jmonth, jday, jhour, jminute, jsecond, jtick, timezone, daylightsaving);
    rc = diminuto_time_duration(now, &dday, &dhour, &dminute, &dsecond, &dtick);
    if (rc < 0) { dday = -dday; }
    if ((now != zulu) || (now != juliet) || verbose || (zyear != prior)) {
        if (!notfirst) {
            DIMINUTO_LOG_DEBUG("%20s %20s %20s %018s %30s %30s %25s %15s %15s\n"
                , "NOW", "ZULU", "JULIET", "OFFSET"
                , "ZULU", "JULIET", "DURATION"
                , "TIMEZONE", "DAYLIGHTSAVING");
            notfirst = !0;
        }
        DIMINUTO_LOG_DEBUG("%20lld %20lld %20lld 0x%016llx %4.4d-%2.2d-%2.2dT%2.2d:%2.2d:%2.2d.%9.9lluZ %4.4d-%2.2d-%2.2dT%2.2d:%2.2d:%2.2d.%9.9lluJ %6d/%2.2d:%2.2d:%2.2d.%9.9llu %15lld %15lld\n"
            , (long long int)now, (long long int)zulu, (long long int)juliet, (long long int)offset
            , zyear, zmonth, zday, zhour, zminute, zsecond, (long long unsigned int)ztick
            , jyear, jmonth, jday, jhour, jminute, jsecond, (long long unsigned int)jtick
            , dday, dhour, dminute, dsecond, (long long unsigned int)dtick
            , timezone, daylightsaving
        );
    }
    /*
     * The conversion between ticks, time_t, and dddd/mm/yyThh:mm:ss should always
     * work for UTC (zulu) time. But at the boundaries of the maximum and minimum
     * the local (juliet) time may not work because the timezone and daylightsaving
     * offsets will place the values outside of the maximum or minimum. Detecting
     * this is made more difficult by the fact that the documented error return
     * for timegm(3) and timelocal(3) is (time_t)-1, but -1 is a valid epoch time
     * value. It's made even more difficult by the fact that at least for some
     * glibc versions errno is not set and returns as zero (the man pages suggest
     * EOVERFLOW), making any distinction impossible.
     */
    EXPECT(now == zulu);
    ADVISE(now == juliet);
    SANITY(zyear, zmonth, zday, zhour, zminute, zsecond, ztick);
    if (now == juliet) { SANITY(jyear, jmonth, jday, jhour, jminute, jsecond, jtick); }
    SANITY(2017, 9, 29, dhour, dminute, dsecond, dtick);
    EXPECT((rc < 0) || (rc > 0));

    prior = zyear;
}

int main(int argc, char ** argv)
{
    diminuto_sticks_t now;
    diminuto_sticks_t hertz;
    diminuto_sticks_t low;
    diminuto_sticks_t high;
    extern char *tzname[2];
    extern long timezone;
    extern int daylight;

    SETLOGMASK();

    hertz = diminuto_frequency();
    tzset();

    low = 0xffffffff80000000LL;
    high = 0x000000007fffffffLL - (7 * 3600) - 3600;

    printf("hertz          %lld\n", (long long int)hertz);
    printf("timezone       %lld\n", (long long int)timezone);
    printf("daylight       %lld\n", (long long int)daylight);
    printf("low            %lld\n", (long long int)low);
    printf("high           %lld\n", (long long int)high);

    TEST();

    sanity0();

    STATUS();

    TEST();

    sanity1();

    STATUS();

    /*
     * sanity0 and sanity1 are basic sanity tests.
     * But if they pass, the code is probably
     * okay. Some of the subsequent tests for
     * edge cases are pretty out there.
     */

    TEST();

    notfirst = 0;
    prior = -1;

    /*
     * See also fun/timestuff.c
     */

    epoch(0xffffffff80000000LL * hertz, !0);
    VERIFY(1901, 12, 13, 20, 45, 52, 0);

    epoch(-1 * hertz, !0);
    VERIFY(1969, 12, 31, 23, 59, 59, 0);

    epoch(0, !0);
    VERIFY(1970, 1, 1, 0, 0, 0, 0);

    epoch(1, !0);
    VERIFY(1970, 1, 1, 0, 0, 0, 1);

    epoch(hertz - 1, !0);
    VERIFY(1970, 1, 1, 0, 0, 0, hertz - 1);

    epoch(hertz, !0);
    VERIFY(1970, 1, 1, 0, 0, 1, 0);

    epoch(1000000000LL * hertz, !0);
    VERIFY(2001, 9, 9, 1, 46, 40, 0);

    epoch(1234567890LL * hertz, !0);
    VERIFY(2009, 2, 13, 23, 31, 30, 0);

    epoch(15000LL * 24LL * 3600LL * hertz, !0);
    VERIFY(2011, 1, 26, 0, 0, 0, 0);

    epoch(1400000000LL * hertz, !0);
    VERIFY(2014, 5, 13, 16, 53, 20, 0);

    epoch(0x000000007fffffffLL * hertz, !0);
    VERIFY(2038, 1, 19, 3, 14, 7, 0);

    STATUS();

    TEST();

    notfirst = 0;
    prior = -1;

    epoch(low * hertz, !0);
    epoch(-hertz, !0);
    epoch(0, !0);
    epoch(high * hertz, !0);

    STATUS();

    TEST();

    notfirst = 0;
    prior = -1;

    for (now = low; now <= high; now += (365 * 24 * 60 * 60)) {
        epoch(now * hertz, 0);
    }

    STATUS();

    TEST();

    notfirst = 0;
    prior = -1;

    for (now = low; now <= high; now += (24 * 60 * 60)) {
        epoch(now * hertz, 0);
    }

    STATUS();

    TEST();

    notfirst = 0;
    prior = -1;

    for (now = low; now <= high; now += (60 * 60)) {
        epoch(now * hertz, 0);
    }

    STATUS();

    TEST();

    notfirst = 0;
    prior = -1;

    for (now = low; now <= high; now += 60) {
        epoch(now * hertz, 0);
    }

    STATUS();

    TEST();

    notfirst = 0;
    prior = -1;

    for (now = low; now <= high; now += 1) {
        epoch(now * hertz, 0);
    }

    STATUS();

    EXIT();
}
