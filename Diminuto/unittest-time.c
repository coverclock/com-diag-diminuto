/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2008-2013 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "com/diag/diminuto/diminuto_core.h"
#include "com/diag/diminuto/diminuto_delay.h"
#include "com/diag/diminuto/diminuto_time.h"
#include <stdio.h>
#include <errno.h>
#include <assert.h>

int main(int argc, char ** argv)
{
    diminuto_usec_t then;
    diminuto_usec_t now;
    diminuto_usec_t measured;
    diminuto_usec_t requested;
    diminuto_usec_t remaining;
    diminuto_usec_t claimed;
    diminuto_usec_t before;
    diminuto_usec_t after;
    diminuto_usec_t elapsed;
    diminuto_usec_t process;
    diminuto_usec_t thread;
    diminuto_usec_t rc;
    diminuto_usec_t zulu;
    diminuto_usec_t juliet;
    diminuto_usec_t timezone;
    diminuto_usec_t daylightsaving;
    diminuto_usec_t hertz;
    int year;
    int month;
    int day;
    int hour;
    int minute;
    int second;
    int microsecond;
    int zh;
    int zm;
    int dh;
    int dm;

    diminuto_core_enable();

    hertz = diminuto_time_hertz();

    printf("%10s %10s %10s %10s %10s %10s %10s %10s %10s %10s %10s %38s\n",
        "requested",
        "remaining",
        "claimed",
        "measured",
        "difference",
        "elapsed",
        "difference",
        "zulu",
        "juliet",
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
        microsecond = -1;
        rc = diminuto_time_zulu(after, &year,    (int *)0, (int *)0, (int *)0, (int *)0, (int *)0, (int *)0);
        rc = diminuto_time_zulu(after, (int *)0, &month,   (int *)0, (int *)0, (int *)0, (int *)0, (int *)0);
        rc = diminuto_time_zulu(after, (int *)0, (int *)0, &day,     (int *)0, (int *)0, (int *)0, (int *)0);
        rc = diminuto_time_zulu(after, (int *)0, (int *)0, (int *)0, &hour,    (int *)0, (int *)0, (int *)0);
        rc = diminuto_time_zulu(after, (int *)0, (int *)0, (int *)0, (int *)0, &minute,  (int *)0, (int *)0);
        rc = diminuto_time_zulu(after, (int *)0, (int *)0, (int *)0, (int *)0, (int *)0, &second,  (int *)0);
        rc = diminuto_time_zulu(after, (int *)0, (int *)0, (int *)0, (int *)0, (int *)0, (int *)0, &microsecond);
        zulu = diminuto_time_epoch(year, month, day, hour, minute, second, microsecond, 0, 0);
        rc = diminuto_time_juliet(after, &year,    (int *)0, (int *)0, (int *)0, (int *)0, (int *)0, (int *)0);
        rc = diminuto_time_juliet(after, (int *)0, &month,   (int *)0, (int *)0, (int *)0, (int *)0, (int *)0);
        rc = diminuto_time_juliet(after, (int *)0, (int *)0, &day,     (int *)0, (int *)0, (int *)0, (int *)0);
        rc = diminuto_time_juliet(after, (int *)0, (int *)0, (int *)0, &hour,    (int *)0, (int *)0, (int *)0);
        rc = diminuto_time_juliet(after, (int *)0, (int *)0, (int *)0, (int *)0, &minute,  (int *)0, (int *)0);
        rc = diminuto_time_juliet(after, (int *)0, (int *)0, (int *)0, (int *)0, (int *)0, &second,  (int *)0);
        rc = diminuto_time_juliet(after, (int *)0, (int *)0, (int *)0, (int *)0, (int *)0, (int *)0, &microsecond);
        timezone = diminuto_time_timezone(after);
        zh = (-timezone / hertz) / 3600;
        zm = (-timezone / hertz) % 3600;
        daylightsaving = diminuto_time_daylightsaving(after);
        dh = (daylightsaving / hertz) / 3600;
        dm = (daylightsaving / hertz) % 3600;
        juliet = diminuto_time_epoch(year, month, day, hour, minute, second, microsecond, timezone, daylightsaving);
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
            microsecond,
            zh,
            zm,
            dh,
            dm
        );
        assert(remaining == 0);
        assert(after == zulu);
        assert(after == juliet);
        diminuto_yield();
    }

    return 0;
}
