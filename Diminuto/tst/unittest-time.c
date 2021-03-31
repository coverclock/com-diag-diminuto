/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2008-2020 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is a unit test of the Time feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is a unit test of the Time feature.
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_core.h"
#include "com/diag/diminuto/diminuto_delay.h"
#include "com/diag/diminuto/diminuto_time.h"
#include "com/diag/diminuto/diminuto_frequency.h"
#include <stdio.h>
#include <errno.h>

int main(int argc, char ** argv)
{
    diminuto_sticks_t result;
    diminuto_ticks_t then;
    diminuto_ticks_t now;
    diminuto_sticks_t measured;
    diminuto_ticks_t requested;
    diminuto_ticks_t remaining;
    diminuto_sticks_t claimed;
    diminuto_ticks_t before;
    diminuto_ticks_t after;
    diminuto_sticks_t elapsed;
    diminuto_ticks_t process;
    diminuto_ticks_t thread;
    diminuto_ticks_t rc;
    diminuto_ticks_t zulu;
    diminuto_ticks_t juliet;
    diminuto_sticks_t timezone;
    diminuto_sticks_t daylightsaving;
    diminuto_sticks_t offset;
    diminuto_ticks_t hertz;
    uint64_t logical;
    uint64_t logicalprime;
    int first;
    int dday;
    int dhour;
    int dminute;
    int dsecond;
    diminuto_ticks_t dtick;
    int zyear;
    int zmonth;
    int zday;
    int zhour;
    int zminute;
    int zsecond;
    diminuto_ticks_t ztick;
    int zh;
    int zm;
    int jyear;
    int jmonth;
    int jday;
    int jhour;
    int jminute;
    int jsecond;
    diminuto_ticks_t jtick;
    char ss;

    SETLOGMASK();

    diminuto_core_enable();

    hertz = diminuto_frequency();

    first = !0;

    CHECKPOINT(" %12s %12s %12s %12s %12s %12s %12s %12s %12s %12s %12s %12s %12s %21s %35s %35s\n"
        , "logical"
        , "difference"
        , "requested"
        , "remaining"
        , "claimed"
        , "measured"
        , "difference"
        , "elapsed"
        , "difference"
        , "error-z"
        , "error-j"
        , "process"
        , "thread"
        , "duration"
        , "zulu"
        , "juliet"
    );

    for (requested = 0; requested <= (hertz * 240); requested = (requested > 0) ? requested * 2 : 1) {
        logical = diminuto_time_logical();
        ASSERT(first || (logical > logicalprime));
        result = diminuto_time_elapsed();
        ASSERT(result != (diminuto_sticks_t)-1);
        then = result;
        result = diminuto_time_clock();
        ASSERT(result != (diminuto_sticks_t)-1);
        before = result;
        remaining = diminuto_delay(requested, 0);
        result = diminuto_time_elapsed();
        ASSERT(result != (diminuto_sticks_t)-1);
        now = result;
        result = diminuto_time_clock();
        ASSERT(result != (diminuto_sticks_t)-1);
        after = result;
        claimed = requested - remaining;
        ASSERT(claimed >= 0);
        measured = now - then;
        ASSERT(measured > 0);
        elapsed = after - before;
        ASSERT(elapsed > 0);
        dday = -1; dhour = -1; dminute = -1; dsecond = -1; dtick = -1;
        ss = (diminuto_time_duration(elapsed, &dday, &dhour, &dminute, &dsecond, &dtick) < 0) ? '-' : '+';
        zyear = -1; zmonth = -1; zday = -1; zhour = -1; zminute = -1; zsecond = -1; ztick = -1;
        rc = diminuto_time_zulu(after, &zyear,   (int *)0, (int *)0, (int *)0, (int *)0, (int *)0, (diminuto_ticks_t *)0);
        ASSERT(rc >= 0);
        rc = diminuto_time_zulu(after, (int *)0, &zmonth,  (int *)0, (int *)0, (int *)0, (int *)0, (diminuto_ticks_t *)0);
        ASSERT(rc >= 0);
        rc = diminuto_time_zulu(after, (int *)0, (int *)0, &zday,    (int *)0, (int *)0, (int *)0, (diminuto_ticks_t *)0);
        ASSERT(rc >= 0);
        rc = diminuto_time_zulu(after, (int *)0, (int *)0, (int *)0, &zhour,   (int *)0, (int *)0, (diminuto_ticks_t *)0);
        ASSERT(rc >= 0);
        rc = diminuto_time_zulu(after, (int *)0, (int *)0, (int *)0, (int *)0, &zminute, (int *)0, (diminuto_ticks_t *)0);
        ASSERT(rc >= 0);
        rc = diminuto_time_zulu(after, (int *)0, (int *)0, (int *)0, (int *)0, (int *)0, &zsecond, (diminuto_ticks_t *)0);
        ASSERT(rc >= 0);
        rc = diminuto_time_zulu(after, (int *)0, (int *)0, (int *)0, (int *)0, (int *)0, (int *)0, &ztick);
        ASSERT(rc >= 0);
        zulu = diminuto_time_epoch(zyear, zmonth, zday, zhour, zminute, zsecond, ztick, 0, 0);
        ASSERT((zulu >= 0) || (errno == 0));
        jyear = -1; jmonth = -1; jday = -1; jhour = -1; jminute = -1; jsecond = -1; jtick = -1;
        rc = diminuto_time_juliet(after, &jyear,   (int *)0, (int *)0, (int *)0, (int *)0, (int *)0, (diminuto_ticks_t *)0);
        ASSERT(rc >= 0);
        rc = diminuto_time_juliet(after, (int *)0, &jmonth,  (int *)0, (int *)0, (int *)0, (int *)0, (diminuto_ticks_t *)0);
        ASSERT(rc >= 0);
        rc = diminuto_time_juliet(after, (int *)0, (int *)0, &jday,    (int *)0, (int *)0, (int *)0, (diminuto_ticks_t *)0);
        ASSERT(rc >= 0);
        rc = diminuto_time_juliet(after, (int *)0, (int *)0, (int *)0, &jhour,   (int *)0, (int *)0, (diminuto_ticks_t *)0);
        ASSERT(rc >= 0);
        rc = diminuto_time_juliet(after, (int *)0, (int *)0, (int *)0, (int *)0, &jminute, (int *)0, (diminuto_ticks_t *)0);
        ASSERT(rc >= 0);
        rc = diminuto_time_juliet(after, (int *)0, (int *)0, (int *)0, (int *)0, (int *)0, &jsecond, (diminuto_ticks_t *)0);
        ASSERT(rc >= 0);
        rc = diminuto_time_juliet(after, (int *)0, (int *)0, (int *)0, (int *)0, (int *)0, (int *)0, &jtick);
        ASSERT(rc >= 0);
        timezone = diminuto_time_timezone();
        daylightsaving = diminuto_time_daylightsaving(after);
        offset = timezone + daylightsaving;
        zh = ((((offset < 0) ? -offset : offset)) / hertz) / 3600;
        zm = ((((offset < 0) ? -offset : offset)) / hertz) % 3600;
        juliet = diminuto_time_epoch(jyear, jmonth, jday, jhour, jminute, jsecond, jtick, timezone, daylightsaving);
        ASSERT((juliet >= 0) || (errno == 0));
        result = diminuto_time_process();
        ASSERT(result != (diminuto_sticks_t)-1);
        process = result;
        result = diminuto_time_thread();
        ASSERT(result != (diminuto_sticks_t)-1);
        thread = result;
        CHECKPOINT("%12lld %12lld %12lld %12lld %12lld %12lld %12lld %12lld %12lld %12lld %12lld %12lld %12lld %c%1.1d/%2.2d:%2.2d:%2.2d.%9.9llu %4.4d-%2.2d-%2.2dT%2.2d:%2.2d:%2.2d.%9.9llu+%2.2d:%2.2d %4.4d-%2.2d-%2.2dT%2.2d:%2.2d:%2.2d.%9.9llu%c%2.2d:%2.2d\n"
            , logical, first ? 0 : logical - logicalprime
            , requested, remaining
            , claimed
            , measured, measured - requested
            , elapsed, elapsed - requested
            , after - zulu
            , after - juliet
            , process, thread
            , ss, dday, dhour, dminute, dsecond, dtick
            , zyear, zmonth, zday, zhour, zminute, zsecond, ztick, 0, 0
            , jyear, jmonth, jday, jhour, jminute, jsecond, jtick, (offset < 0) ? '-' : '+', zh, zm
        );
        ASSERT(remaining == 0);
        ASSERT(after == zulu);
        ASSERT(after == juliet);
        diminuto_yield();
        first = 0;
        logicalprime = logical;
    }

    EXIT();
}
