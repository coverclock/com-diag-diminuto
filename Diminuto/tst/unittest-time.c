/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2008-2022 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is a partial unit test of the Time feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is a partial unit test of the Time feature. See the Epoch unit test
 * for further testing.
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_core.h"
#include "com/diag/diminuto/diminuto_delay.h"
#include "com/diag/diminuto/diminuto_time.h"
#include "com/diag/diminuto/diminuto_frequency.h"
#include "com/diag/diminuto/diminuto_types.h"
#include "com/diag/diminuto/diminuto_minmaxof.h"
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
    diminuto_ticks_t atomic;
    diminuto_ticks_t atomicprime;
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

    ASSERT(issigned(typeof(DIMINUTO_TIME_ERROR)));
    ASSERT(DIMINUTO_TIME_ERROR < 0);

    CHECKPOINT(" %8s %12s %12s %12s %12s %12s %12s %12s %12s %6s %6s %12s %12s %21s %35s %35s\n", "logical", "atomic", "requested", "remaining", "claimed", "measured", "delta", "elapsed", "delta", "err-z", "err-j", "process", "thread", "duration", "zulu", "juliet");

    for (requested = 0; requested <= (hertz * 240); requested = (requested > 0) ? requested * 2 : 1) {
        logical = diminuto_time_logical();
        ASSERT(first || ((logical > logicalprime) && ((logical - logicalprime) == 1)));
        result = diminuto_time_atomic();
        ASSERT(result >= 0);
        ASSERT(first || (atomic >= atomicprime));
        atomic = result;
        result = diminuto_time_elapsed();
        ASSERT(result >= 0);
        then = result;
        result = diminuto_time_clock();
        ASSERT(result >= 0);
        before = result;
        remaining = diminuto_delay(requested, 0);
        result = diminuto_time_elapsed();
        ASSERT(result >= 0);
        now = result;
        result = diminuto_time_clock();
        ASSERT(result >= 0);
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
        ASSERT(result >= 0);
        process = result;
        result = diminuto_time_thread();
        ASSERT(result >= 0);
        thread = result;
        CHECKPOINT("%8lld %12lld %12lld %12lld %12lld %12lld %12lld %12lld %12lld %6lld %6lld %12lld %12lld %c%1.1d/%2.2d:%2.2d:%2.2d.%9.9llu %4.4d-%2.2d-%2.2dT%2.2d:%2.2d:%2.2d.%9.9llu+%2.2d:%2.2d %4.4d-%2.2d-%2.2dT%2.2d:%2.2d:%2.2d.%9.9llu%c%2.2d:%2.2d\n"
            , (diminuto_lld_t)logical
            , (diminuto_lld_t)(first ? 0 : atomic - atomicprime)
            , (diminuto_lld_t)requested, (diminuto_lld_t)remaining
            , (diminuto_lld_t)claimed
            , (diminuto_lld_t)measured, (diminuto_lld_t)(measured - requested)
            , (diminuto_lld_t)elapsed, (diminuto_lld_t)(elapsed - requested)
            , (diminuto_lld_t)(after - zulu)
            , (diminuto_lld_t)(after - juliet)
            , (diminuto_lld_t)process, (diminuto_lld_t)thread
            , ss, dday, dhour, dminute, dsecond, (diminuto_llu_t)dtick
            , zyear, zmonth, zday, zhour, zminute, zsecond, (diminuto_llu_t)ztick, 0, 0
            , jyear, jmonth, jday, jhour, jminute, jsecond, (diminuto_llu_t)jtick, (offset < 0) ? '-' : '+', zh, zm
        );
        ASSERT(remaining == 0);
        ASSERT(after == zulu);
        ASSERT(after == juliet);
        first = 0;
        logicalprime = logical;
        atomicprime = atomic;
        diminuto_yield();
    }

    EXIT();
}
