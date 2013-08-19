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
    int64_t measured;
    diminuto_usec_t requested;
    diminuto_usec_t remaining;
    int64_t claimed;
    diminuto_usec_t before;
    diminuto_usec_t after;
    int64_t elapsed;

    diminuto_core_enable();

    printf("%10s %10s %10s %10s %10s %10s %10s %10s %10s\n",
        "requested",
        "remaining",
        "claimed",
        "measured",
        "difference",
        "elapsed",
        "difference",
        "process",
        "thread");

    for (requested = 0; requested < 60000000; requested = requested ? requested * 2 : 1) {
        then = diminuto_time_elapsed();
        before = diminuto_time_clock();
        remaining = diminuto_delay(requested, 0);
        now = diminuto_time_elapsed();
        after = diminuto_time_clock();
        claimed = requested - remaining;
        measured = now - then;
        elapsed = after - before;
        printf("%10llu %10llu %10lld %10lld %10lld %10llu %10lld %10lld %10lld\n",
            requested,
            remaining,
            claimed,
            measured,
            measured - requested,
            elapsed,
            elapsed - requested,
            diminuto_time_process(),
            diminuto_time_thread());
    }

    return 0;
}
