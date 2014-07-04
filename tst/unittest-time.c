/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2008-2013 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_core.h"
#include "com/diag/diminuto/diminuto_delay.h"
#include "com/diag/diminuto/diminuto_time.h"
#include <stdio.h>
#include <errno.h>

int main(int argc, char ** argv)
{
    diminuto_ticks_t then;
    diminuto_ticks_t now;
    diminuto_ticks_t measured;
    diminuto_ticks_t requested;
    diminuto_ticks_t remaining;
    diminuto_ticks_t claimed;
    diminuto_ticks_t before;
    diminuto_ticks_t after;
    diminuto_ticks_t elapsed;
    diminuto_ticks_t process;
    diminuto_ticks_t thread;
    diminuto_ticks_t rc;
    diminuto_ticks_t zulu;
    diminuto_ticks_t juliet;
    diminuto_ticks_t timezone;
    diminuto_ticks_t daylightsaving;
    diminuto_ticks_t hertz;
    int dday;
    int dhour;
    int dminute;
    int dsecond;
    int dtick;
    int zyear;
    int zmonth;
    int zday;
    int zhour;
    int zminute;
    int zsecond;
    int ztick;
    int zh;
    int zm;
    int dh;
    int dm;
    int jyear;
    int jmonth;
    int jday;
    int jhour;
    int jminute;
    int jsecond;
    int jtick;
    char ss;

    diminuto_core_enable();

    hertz = diminuto_time_frequency();

    printf("%12s %12s %12s %12s %12s %12s %12s %12s %12s %12s %12s %21s %41s %41s\n",
        "requested",
        "remaining",
        "claimed",
        "measured",
        "difference",
        "elapsed",
        "difference",
        "error-z",
        "error-j",
        "process",
        "thread",
        "duration",
        "zulu",
        "juliet"
    );

    for (requested = 0; requested <= (hertz * 240); requested = (requested > 0) ? requested * 2 : 1) {
        then = diminuto_time_elapsed();
        before = diminuto_time_clock();
        remaining = diminuto_delay(requested, 0);
        now = diminuto_time_elapsed();
        after = diminuto_time_clock();
        claimed = requested - remaining;
        measured = now - then;
        elapsed = after - before;
        dday = -1; dhour = -1; dminute = -1; dsecond = -1; dtick = -1;
        ss = (diminuto_time_duration(elapsed, &dday, &dhour, &dminute, &dsecond, &dtick) < 0) ? '-' : '+';
        zyear = -1; zmonth = -1; zday = -1; zhour = -1; zminute = -1; zsecond = -1; ztick = -1;
        rc = diminuto_time_zulu(after, &zyear,   (int *)0, (int *)0, (int *)0, (int *)0, (int *)0, (int *)0);
        rc = diminuto_time_zulu(after, (int *)0, &zmonth,  (int *)0, (int *)0, (int *)0, (int *)0, (int *)0);
        rc = diminuto_time_zulu(after, (int *)0, (int *)0, &zday,    (int *)0, (int *)0, (int *)0, (int *)0);
        rc = diminuto_time_zulu(after, (int *)0, (int *)0, (int *)0, &zhour,   (int *)0, (int *)0, (int *)0);
        rc = diminuto_time_zulu(after, (int *)0, (int *)0, (int *)0, (int *)0, &zminute, (int *)0, (int *)0);
        rc = diminuto_time_zulu(after, (int *)0, (int *)0, (int *)0, (int *)0, (int *)0, &zsecond, (int *)0);
        rc = diminuto_time_zulu(after, (int *)0, (int *)0, (int *)0, (int *)0, (int *)0, (int *)0, &ztick);
        zulu = diminuto_time_epoch(zyear, zmonth, zday, zhour, zminute, zsecond, ztick, 0, 0);
        jyear = -1; jmonth = -1; jday = -1; jhour = -1; jminute = -1; jsecond = -1; jtick = -1;
        rc = diminuto_time_juliet(after, &jyear,   (int *)0, (int *)0, (int *)0, (int *)0, (int *)0, (int *)0);
        rc = diminuto_time_juliet(after, (int *)0, &jmonth,  (int *)0, (int *)0, (int *)0, (int *)0, (int *)0);
        rc = diminuto_time_juliet(after, (int *)0, (int *)0, &jday,    (int *)0, (int *)0, (int *)0, (int *)0);
        rc = diminuto_time_juliet(after, (int *)0, (int *)0, (int *)0, &jhour,   (int *)0, (int *)0, (int *)0);
        rc = diminuto_time_juliet(after, (int *)0, (int *)0, (int *)0, (int *)0, &jminute, (int *)0, (int *)0);
        rc = diminuto_time_juliet(after, (int *)0, (int *)0, (int *)0, (int *)0, (int *)0, &jsecond, (int *)0);
        rc = diminuto_time_juliet(after, (int *)0, (int *)0, (int *)0, (int *)0, (int *)0, (int *)0, &jtick);
        timezone = diminuto_time_timezone(after);
        zh = (-timezone / hertz) / 3600;
        zm = (-timezone / hertz) % 3600;
        daylightsaving = diminuto_time_daylightsaving(after);
        dh = (daylightsaving / hertz) / 3600;
        dm = (daylightsaving / hertz) % 3600;
        juliet = diminuto_time_epoch(jyear, jmonth, jday, jhour, jminute, jsecond, jtick, timezone, daylightsaving);
        process = diminuto_time_process();
        thread = diminuto_time_thread();
        printf("%12lld %12lld %12lld %12lld %12lld %12lld %12lld %12lld %12lld %12lld %12lld %c%1.1d/%2.2d:%2.2d:%2.2d.%9.9d %4.4d-%2.2d-%2.2dT%2.2d:%2.2d:%2.2d.%9.9d-%2.2d:%2.2d+%2.2d:%2.2d %4.4d-%2.2d-%2.2dT%2.2d:%2.2d:%2.2d.%9.9d-%2.2d:%2.2d+%2.2d:%2.2d\n"
            , requested, remaining
            , claimed
            , measured, measured - requested
            , elapsed, elapsed - requested
            , after - zulu
            , after - juliet
            , process, thread
            , ss, dday, dhour, dminute, dsecond, dtick
            , zyear, zmonth, zday, zhour, zminute, zsecond, ztick, 0, 0, 0, 0
            , jyear, jmonth, jday, jhour, jminute, jsecond, jtick, zh, zm, dh, dm
        );
        ASSERT(remaining == 0);
        ASSERT(after == zulu);
        ASSERT(after == juliet);
        diminuto_yield();
    }

    return 0;
}
