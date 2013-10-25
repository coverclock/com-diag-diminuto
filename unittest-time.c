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
    int rc;
    int year;
    int month;
    int day;
    int hour;
    int minute;
    int second;
    int microsecond;
    diminuto_usec_t epoch;

    diminuto_core_enable();

    printf("%10s %10s %10s %10s %10s %10s %10s %10s %26s %10s %10s\n",
        "requested",
        "remaining",
        "claimed",
        "measured",
        "difference",
        "elapsed",
        "difference",
        "error",
        "timestamp",
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
        process = diminuto_time_process();
        thread = diminuto_time_thread();
        rc = diminuto_time_datetime(after, &year,    (int *)0, (int *)0, (int *)0, (int *)0, (int *)0, (int *)0);
        rc = diminuto_time_datetime(after, (int *)0, &month,   (int *)0, (int *)0, (int *)0, (int *)0, (int *)0);
        rc = diminuto_time_datetime(after, (int *)0, (int *)0, &day,     (int *)0, (int *)0, (int *)0, (int *)0);
        rc = diminuto_time_datetime(after, (int *)0, (int *)0, (int *)0, &hour,    (int *)0, (int *)0, (int *)0);
        rc = diminuto_time_datetime(after, (int *)0, (int *)0, (int *)0, (int *)0, &minute,  (int *)0, (int *)0);
        rc = diminuto_time_datetime(after, (int *)0, (int *)0, (int *)0, (int *)0, (int *)0, &second,  (int *)0);
        rc = diminuto_time_datetime(after, (int *)0, (int *)0, (int *)0, (int *)0, (int *)0, (int *)0, &microsecond);
        epoch = diminuto_time_epoch(year, month, day, hour, minute, second, microsecond);
        printf("%10lld %10lld %10lld %10lld %10lld %10lld %10lld %10lld %4.4d-%2.2d-%2.2dT%2.2d:%2.2d:%2.2d.%6.6d %10lld %10lld\n",
            requested,
            remaining,
            claimed,
            measured,
            measured - requested,
            elapsed,
            elapsed - requested,
            after - epoch,
            year,
            month,
            day,
            hour,
            minute,
            second,
            microsecond,
            process,
            thread
        );
    }

    return 0;
}
