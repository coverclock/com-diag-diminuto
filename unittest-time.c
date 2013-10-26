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
    int year;
    int month;
    int day;
    int hour;
    int minute;
    int second;
    int tick;
    int zh;
    int zm;
    int dh;
    int dm;

    diminuto_core_enable();

    hertz = diminuto_time_resolution();

    printf("%10s %10s %10s %10s %10s %10s %10s %10s %10s %10s %10s %38s\n",
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
        "iso8601"
    );

    for (requested = 0; requested < 60000000; requested = requested ? requested * 2 : 1) {
        then = diminuto_time_elapsed();
        before = diminuto_time_clock();
        remaining = diminuto_delay(requested, 0);
        now = diminuto_time_elapsed();
        after = diminuto_time_clock();
        claimed = requested - remaining;
        measured = now - then;
        elapsed = after - before;
        year = -1;
        month = -1;
        day = -1;
        hour = -1;
        minute = -1;
        second = -1;
        tick = -1;
        rc = diminuto_time_zulu(after, &year,    (int *)0, (int *)0, (int *)0, (int *)0, (int *)0, (int *)0);
        rc = diminuto_time_zulu(after, (int *)0, &month,   (int *)0, (int *)0, (int *)0, (int *)0, (int *)0);
        rc = diminuto_time_zulu(after, (int *)0, (int *)0, &day,     (int *)0, (int *)0, (int *)0, (int *)0);
        rc = diminuto_time_zulu(after, (int *)0, (int *)0, (int *)0, &hour,    (int *)0, (int *)0, (int *)0);
        rc = diminuto_time_zulu(after, (int *)0, (int *)0, (int *)0, (int *)0, &minute,  (int *)0, (int *)0);
        rc = diminuto_time_zulu(after, (int *)0, (int *)0, (int *)0, (int *)0, (int *)0, &second,  (int *)0);
        rc = diminuto_time_zulu(after, (int *)0, (int *)0, (int *)0, (int *)0, (int *)0, (int *)0, &tick);
        zulu = diminuto_time_epoch(year, month, day, hour, minute, second, tick, 0, 0);
        rc = diminuto_time_juliet(after, &year,    (int *)0, (int *)0, (int *)0, (int *)0, (int *)0, (int *)0);
        rc = diminuto_time_juliet(after, (int *)0, &month,   (int *)0, (int *)0, (int *)0, (int *)0, (int *)0);
        rc = diminuto_time_juliet(after, (int *)0, (int *)0, &day,     (int *)0, (int *)0, (int *)0, (int *)0);
        rc = diminuto_time_juliet(after, (int *)0, (int *)0, (int *)0, &hour,    (int *)0, (int *)0, (int *)0);
        rc = diminuto_time_juliet(after, (int *)0, (int *)0, (int *)0, (int *)0, &minute,  (int *)0, (int *)0);
        rc = diminuto_time_juliet(after, (int *)0, (int *)0, (int *)0, (int *)0, (int *)0, &second,  (int *)0);
        rc = diminuto_time_juliet(after, (int *)0, (int *)0, (int *)0, (int *)0, (int *)0, (int *)0, &tick);
        timezone = diminuto_time_timezone(after);
        zh = (-timezone / hertz) / 3600;
        zm = (-timezone / hertz) % 3600;
        daylightsaving = diminuto_time_daylightsaving(after);
        dh = (daylightsaving / hertz) / 3600;
        dm = (daylightsaving / hertz) % 3600;
        juliet = diminuto_time_epoch(year, month, day, hour, minute, second, tick, timezone, daylightsaving);
        process = diminuto_time_process();
        thread = diminuto_time_thread();
        printf("%10lld %10lld %10lld %10lld %10lld %10lld %10lld %10lld %10lld %10lld %10lld %4.4d-%2.2d-%2.2dT%2.2d:%2.2d:%2.2d.%6.6d-%2.2d:%2.2d+%2.2d:%2.2d\n",
            requested,
            remaining,
            claimed,
            measured,
            measured - requested,
            elapsed,
            elapsed - requested,
            after - zulu,
            after - juliet,
            process,
            thread,
            year,
            month,
            day,
            hour,
            minute,
            second,
            tick,
            zh,
            zm,
            dh,
            dm
        );
        ASSERT(remaining == 0);
        ASSERT(after == zulu);
        ASSERT(after == juliet);
        diminuto_yield();
    }

    return 0;
}
