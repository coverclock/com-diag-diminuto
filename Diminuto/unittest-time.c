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
    int year;
    int month;
    int day;
    int hour;
    int minute;
    int second;
    int microsecond;
    diminuto_usec_t zulu;
    diminuto_usec_t juliet;
    diminuto_usec_t offset;
    int hh;
    int mm;
    int daylightsaving;

    diminuto_core_enable();

    printf("%10s %10s %10s %10s %10s %10s %10s %10s %10s %31s %3s %10s %10s\n",
        "requested",
        "remaining",
        "claimed",
        "measured",
        "difference",
        "elapsed",
        "difference",
        "zulu",
        "juliet",
        "iso8601",
        "dst",
        "process",
        "thread"
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
        offset = diminuto_time_offset();
        daylightsaving = diminuto_time_daylightsaving();
        rc = diminuto_time_juliet(after, &year,    (int *)0, (int *)0, (int *)0, (int *)0, (int *)0, (int *)0);
        rc = diminuto_time_juliet(after, (int *)0, &month,   (int *)0, (int *)0, (int *)0, (int *)0, (int *)0);
        rc = diminuto_time_juliet(after, (int *)0, (int *)0, &day,     (int *)0, (int *)0, (int *)0, (int *)0);
        rc = diminuto_time_juliet(after, (int *)0, (int *)0, (int *)0, &hour,    (int *)0, (int *)0, (int *)0);
        rc = diminuto_time_juliet(after, (int *)0, (int *)0, (int *)0, (int *)0, &minute,  (int *)0, (int *)0);
        rc = diminuto_time_juliet(after, (int *)0, (int *)0, (int *)0, (int *)0, (int *)0, &second,  (int *)0);
        rc = diminuto_time_juliet(after, (int *)0, (int *)0, (int *)0, (int *)0, (int *)0, (int *)0, &microsecond);
        juliet = diminuto_time_epoch(year, month, day, hour, minute, second, microsecond, offset, daylightsaving);
        hh = (-offset / 1000000) / 3600;
        mm = (-offset / 1000000) % 3600;
        process = diminuto_time_process();
        thread = diminuto_time_thread();
        printf("%10lld %10lld %10lld %10lld %10lld %10lld %10lld %10lld %10lld %4.4d-%2.2d-%2.2dT%2.2d:%2.2d:%2.2d.%6.6d-%2.2d%2.2d %3d %10lld %10lld\n",
            requested,
            remaining,
            claimed,
            measured,
            measured - requested,
            elapsed,
            elapsed - requested,
            after - zulu,
            after - juliet,
            year,
            month,
            day,
            hour,
            minute,
            second,
            microsecond,
            hh,
            mm,
            daylightsaving,
            process,
            thread
        );
        diminuto_yield();
    }

    return 0;
}
