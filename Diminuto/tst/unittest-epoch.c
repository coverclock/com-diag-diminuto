/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2013-2022 Digital Aggregates Corporation, Colorado, USA.
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
#include <string.h>

#define SANITY(_YEAR_, _MONTH_, _DAY_, _HOUR_, _MINUTE_, _SECOND_, _TICK_) \
    do { \
        COMMENT("SANITY %u %u %u %u %u %u %u\n", _YEAR_, _MONTH_, _DAY_, _HOUR_, _MINUTE_, _SECOND_, (int)_TICK_); \
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
        COMMENT("VERIFY A %u %u %u %u %u %u %u\n", _YEAR_, _MONTH_, _DAY_, _HOUR_, _MINUTE_, _SECOND_, (int)_TICK_); \
        COMMENT("VERIFY B %u %u %u %u %u %u %u\n", zyear, zmonth, zday, zhour, zminute, zsecond, (int)ztick); \
        EXPECT(zyear == (_YEAR_)); \
        EXPECT(zmonth == (_MONTH_)); \
        EXPECT(zday == (_DAY_)); \
        EXPECT(zhour == (_HOUR_)); \
        EXPECT(zminute == (_MINUTE_)); \
        EXPECT(zsecond == (_SECOND_)); \
        EXPECT(ztick == (_TICK_)); \
    } while (0)

static const time_t LOW  = (time_t)diminuto_minimumof(int32_t);
static const time_t EPOCH = 0;
static const time_t HIGH = (time_t)diminuto_maximumof(int32_t);

static const diminuto_sticks_t MINIMUM = COM_DIAG_DIMINUTO_FREQUENCY * ((diminuto_sticks_t)diminuto_minimumof(int32_t) + (60LL * 60LL * 24LL));
static const diminuto_sticks_t MAXIMUM = COM_DIAG_DIMINUTO_FREQUENCY * ((diminuto_sticks_t)diminuto_maximumof(int32_t) - (60LL * 60LL * 24LL));
static const diminuto_sticks_t FRACTION = 999999999LL;


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
    CHECKPOINT("clock %lld\n", (diminuto_lld_t)now);
    ASSERT(now >= 0);

    zone = diminuto_time_timezone();
    CHECKPOINT("timezone %lld\n", (diminuto_lld_t)zone);
    /* zone can be positive or negatuve. */

    dst = diminuto_time_daylightsaving(now);
    CHECKPOINT("daylightsaving  %lld\n", (diminuto_lld_t)dst);
    ASSERT(dst >= 0);

    rc = diminuto_time_zulu(now, &year, &month, &day, &hour, &minute, &second, &tick);
    ASSERT(rc >= 0);
    CHECKPOINT("zulu %04d-%02d-%02dT%02d:%02d:%02d.%09lld\n", year, month, day, hour, minute, second, (diminuto_lld_t)tick);

    SANITY(year, month, day, hour, minute, second, tick);

    then = diminuto_time_epoch(year, month, day, hour, minute, second, tick, 0, 0);
    ASSERT((then >= 0) || (errno == 0));
    CHECKPOINT("epoch %lld\n", (diminuto_lld_t)then);

    delta = then - now;
    CHECKPOINT("delta %lld\n", (diminuto_lld_t)delta);
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
    CHECKPOINT("clock %lld\n", (diminuto_lld_t)now);
    ASSERT(now >= 0);

    zone = diminuto_time_timezone();
    CHECKPOINT("timezone %lld\n", (diminuto_lld_t)zone);
    /* zone can be positive or negatuve. */

    dst = diminuto_time_daylightsaving(now);
    CHECKPOINT("daylightsaving %lld\n", (diminuto_lld_t)dst);
    ASSERT(dst >= 0);

    rc = diminuto_time_juliet(now, &year, &month, &day, &hour, &minute, &second, &tick);
    ASSERT(rc >= 0);
    CHECKPOINT("juliet %04d-%02d-%02dT%02d:%02d:%02d.%09llu\n", year, month, day, hour, minute, second, (diminuto_lld_t)tick);

    SANITY(year, month, day, hour, minute, second, tick);

    then = diminuto_time_epoch(year, month, day, hour, minute, second, tick, zone, dst);
    ASSERT((then >= 0) || (errno == 0));
    CHECKPOINT("epoch %lld\n", (diminuto_lld_t)then);

    delta = then - now;
    CHECKPOINT("delta %lld\n", (diminuto_lld_t)delta);
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
    if ((MINIMUM <= now) && (now <= MAXIMUM)) {
        juliet = diminuto_time_epoch(jyear, jmonth, jday, jhour, jminute, jsecond, jtick, timezone, daylightsaving);
        ASSERT((juliet >= 0) || (errno == 0));
        EXPECT(now == juliet);
    } else {
        juliet = zulu;
    }

    rc = diminuto_time_duration(now, &dday, &dhour, &dminute, &dsecond, &dtick);
    EXPECT((rc < 0) || (rc > 0));
    if (rc < 0) { dday = -dday; }
    SANITY(2017, 9, 29, dhour, dminute, dsecond, dtick);

    if ((now != zulu) || (now != juliet) || verbose) {
        if (!notfirst) {
            CHECKPOINT("%20s %20s %20s %18s %30s %30s %25s %15s %15s\n" , "NOW", "ZULU", "JULIET", "OFFSET" , "ZULU", "JULIET", "DURATION" , "TIMEZONE", "DAYLIGHTSAVING");
            notfirst = !0;
        }
        CHECKPOINT("%20lld %20lld %20lld 0x%016llx %4.4d-%2.2d-%2.2dT%2.2d:%2.2d:%2.2d.%9.9lluZ %4.4d-%2.2d-%2.2dT%2.2d:%2.2d:%2.2d.%9.9lluJ %6d/%2.2d:%2.2d:%2.2d.%9.9llu %15lld %15lld\n"
            , (diminuto_lld_t)now, (diminuto_lld_t)zulu, (diminuto_lld_t)juliet, (diminuto_lld_t)offset
            , zyear, zmonth, zday, zhour, zminute, zsecond, (diminuto_llu_t)ztick
            , jyear, jmonth, jday, jhour, jminute, jsecond, (diminuto_llu_t)jtick
            , dday, dhour, dminute, dsecond, (diminuto_llu_t)dtick
            , (diminuto_lld_t)timezone, (diminuto_lld_t)daylightsaving
        );
    }
}

enum Scope {
    SANITY = 'S',
    EXTENDED = 'E',
    GEOLOGIC = 'G',
};

int main(int argc, char ** argv)
{
    diminuto_sticks_t ticks;
    diminuto_sticks_t hertz;
    extern char *tzname[2];
    extern long timezone;
    extern int daylight;
    enum Scope scope = GEOLOGIC;

    if (argc < 2) {
        /* Do nothing. */
    } else if (strcasecmp(argv[1], "sanity") == 0) {
        scope = SANITY;
    } else if (strcasecmp(argv[1], "extended") == 0) {
        scope = EXTENDED;
    } else if (strcasecmp(argv[1], "geologic") == 0) {
        scope = GEOLOGIC;
    } else {
        /* Do nothing. */
    }

    SETLOGMASK();

    /* SANITY */

    {
        int year, month, day, hour, minute, second;
        diminuto_ticks_t tick;
        int rc;

        TEST();

        tzset();

        hertz = diminuto_frequency();
        ASSERT(hertz == COM_DIAG_DIMINUTO_FREQUENCY);

        CHECKPOINT("scope %c\n", scope);
        CHECKPOINT("hertz %lld\n", (diminuto_lld_t)hertz);
        CHECKPOINT("timezone %lld\n", (diminuto_lld_t)timezone);
        CHECKPOINT("daylight %lld\n", (diminuto_lld_t)daylight);

        CHECKPOINT("low %lld 0x%llx\n", (diminuto_lld_t)LOW, (diminuto_lld_t)LOW);
        ticks = LOW;
        ticks *= hertz;

        year = month = day = hour = minute = second = tick = 0;
        rc = diminuto_time_zulu(ticks, &year, &month, &day, &hour, &minute, &second, &tick);
        CHECKPOINT("low %02d-%02d-%02dT%02d:%02d:%02d.%09lluZ %d %d\n", year, month, day, hour, minute, second, (diminuto_llu_t)tick, rc, errno);

        year = month = day = hour = minute = second = tick = 0;
        rc = diminuto_time_juliet(ticks, &year, &month, &day, &hour, &minute, &second, &tick);
        CHECKPOINT("low %02d-%02d-%02dT%02d:%02d:%02d.%09lluJ %d %d\n", year, month, day, hour, minute, second, (diminuto_llu_t)tick, rc, errno);

        CHECKPOINT("minimum %lld 0x%llx\n", (diminuto_lld_t)(MINIMUM / hertz), (diminuto_lld_t)(MINIMUM / hertz));
        ticks = MINIMUM;

        year = month = day = hour = minute = second = tick = 0;
        rc = diminuto_time_zulu(ticks, &year, &month, &day, &hour, &minute, &second, &tick);
        CHECKPOINT("minimum %02d-%02d-%02dT%02d:%02d:%02d.%09lluZ %d %d\n", year, month, day, hour, minute, second, (diminuto_llu_t)tick, rc, errno);

        year = month = day = hour = minute = second = tick = 0;
        rc = diminuto_time_juliet(ticks, &year, &month, &day, &hour, &minute, &second, &tick);
        CHECKPOINT("minimum %02d-%02d-%02dT%02d:%02d:%02d.%09lluJ %d %d\n", year, month, day, hour, minute, second, (diminuto_llu_t)tick, rc, errno);

        CHECKPOINT("epoch %lld 0x%llx\n", (diminuto_lld_t)EPOCH, (diminuto_lld_t)EPOCH);
        ticks = EPOCH;
        ticks *= hertz;

        year = month = day = hour = minute = second = tick = 0;
        rc = diminuto_time_zulu(ticks, &year, &month, &day, &hour, &minute, &second, &tick);
        CHECKPOINT("epoch %02d-%02d-%02dT%02d:%02d:%02d.%09lluZ %d %d\n", year, month, day, hour, minute, second, (diminuto_llu_t)tick, rc, errno);

        year = month = day = hour = minute = second = tick = 0;
        rc = diminuto_time_juliet(ticks, &year, &month, &day, &hour, &minute, &second, &tick);
        CHECKPOINT("epoch %02d-%02d-%02dT%02d:%02d:%02d.%09lluJ %d %d\n", year, month, day, hour, minute, second, (diminuto_llu_t)tick, rc, errno);

        CHECKPOINT("maximum %lld 0x%llx\n", (diminuto_lld_t)(MAXIMUM / hertz), (diminuto_lld_t)(MAXIMUM / hertz));
        ticks = MAXIMUM;

        year = month = day = hour = minute = second = tick = 0;
        rc = diminuto_time_zulu(ticks, &year, &month, &day, &hour, &minute, &second, &tick);
        CHECKPOINT("maximum %02d-%02d-%02dT%02d:%02d:%02d.%09lluZ %d %d\n", year, month, day, hour, minute, second, (diminuto_llu_t)tick, rc, errno);

        year = month = day = hour = minute = second = tick = 0;
        rc = diminuto_time_juliet(ticks, &year, &month, &day, &hour, &minute, &second, &tick);
        CHECKPOINT("maximum %02d-%02d-%02dT%02d:%02d:%02d.%09lluJ %d %d\n", year, month, day, hour, minute, second, (diminuto_llu_t)tick, rc, errno);

        CHECKPOINT("high %lld 0x%llx\n", (diminuto_lld_t)HIGH, (diminuto_lld_t)HIGH);
        ticks = HIGH;
        ticks *= hertz;

        year = month = day = hour = minute = second = tick = 0;
        rc = diminuto_time_zulu(ticks, &year, &month, &day, &hour, &minute, &second, &tick);
        CHECKPOINT("high %02d-%02d-%02dT%02d:%02d:%02d.%09lluZ %d %d\n", year, month, day, hour, minute, second, (diminuto_llu_t)tick, rc, errno);

        year = month = day = hour = minute = second = tick = 0;
        rc = diminuto_time_juliet(ticks, &year, &month, &day, &hour, &minute, &second, &tick);
        CHECKPOINT("high %02d-%02d-%02dT%02d:%02d:%02d.%09lluJ %d %d\n", year, month, day, hour, minute, second, (diminuto_llu_t)tick, rc, errno);

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

        notfirst = 0;

        /*
         * See also fun/timestuff.c
         */

        test3((ticks = LOW * hertz), !0);
        VERIFY(1901, 12, 13, 20, 45, 52, 0);

        test3((ticks = MINIMUM), !0);
        VERIFY(1901, 12, 14, 20, 45, 52, 0);

        test3((ticks = ((EPOCH - 1) * hertz) - FRACTION), !0);
        test3((ticks = (EPOCH - 1) * hertz), !0);
        test3((ticks = ((EPOCH - 1) * hertz) + FRACTION), !0);

        test3((ticks = (EPOCH * hertz) - FRACTION), !0);
        test3((ticks = EPOCH * hertz), !0);
        VERIFY(1970, 1, 1, 0, 0, 0, 0);
        test3((ticks = (EPOCH * hertz) + FRACTION), !0);

        test3((ticks = ((EPOCH + 1) * hertz) - FRACTION), !0);
        test3((ticks = (EPOCH + 1) * hertz), !0);
        VERIFY(1970, 1, 1, 0, 0, 0, 1);
        test3((ticks = (EPOCH + 1) + FRACTION), !0);

        test3((ticks = hertz - 1), !0);
        VERIFY(1970, 1, 1, 0, 0, 0, hertz - 1);

        test3((ticks = hertz), !0);
        VERIFY(1970, 1, 1, 0, 0, 1, 0);

        test3((ticks = hertz + 1), !0);

        test3((ticks = -hertz), !0);
        VERIFY(1969, 12, 31, 23, 59, 59, 0);

        test3((ticks = 1000000000LL * hertz), !0);
        VERIFY(2001, 9, 9, 1, 46, 40, 0);

        test3((ticks = 1234567890LL * hertz), !0);
        VERIFY(2009, 2, 13, 23, 31, 30, 0);

        test3((ticks = 15000LL * 24LL * 3600LL * hertz), !0);
        VERIFY(2011, 1, 26, 0, 0, 0, 0);

        test3((ticks = 1400000000LL * hertz), !0);
        VERIFY(2014, 5, 13, 16, 53, 20, 0);

        test3((ticks = MAXIMUM), !0);
        VERIFY(2038, 1, 18, 3, 14, 7, 0);

        test3((ticks = HIGH) * hertz, !0);
        VERIFY(2038, 1, 19, 3, 14, 7, 0);

        STATUS();
    }

    {
        TEST();

        notfirst = 0;

        for (ticks = LOW; ticks <= HIGH; ticks += (365 * 24 * 60 * 60)) {
            test3(ticks * hertz, 0);
        }

        STATUS();
    }

    {
        TEST();

        notfirst = 0;

        for (ticks = LOW; ticks <= HIGH; ticks += (24 * 60 * 60)) {
            test3(ticks * hertz, 0);
        }

        STATUS();
    }

    {
        TEST();

        notfirst = 0;

        for (ticks = LOW; ticks <= HIGH; ticks += (60 * 60)) {
            test3(ticks * hertz, 0);
        }

        STATUS();
    }

    /* EXTENDED */
    if (scope == SANITY) { goto fini; }

    {
        TEST();

        notfirst = 0;

        for (ticks = LOW; ticks <= HIGH; ticks += 60) {
            test3(ticks * hertz, 0);
        }

        STATUS();
    }

    /* GEOLOGIC */
    if (scope == EXTENDED) { goto fini; }

    {
        TEST();

        notfirst = 0;

        for (ticks = LOW; ticks <= HIGH; ticks += 1) {
            test3(ticks * hertz, 0);
        }

        STATUS();
    }

fini:

    EXIT();
}
