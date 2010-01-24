/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2008-2009 Digital Aggregates Corporation, Arvada CO 80001-0587 USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "diminuto_coreable.h"
#include "diminuto_delay.h"
#include "diminuto_time.h"
#include <stdio.h>
#include <errno.h>

int main(int argc, char ** argv)
{
    uint64_t then;
    uint64_t now;
    int64_t measured;
    uint64_t requested;
    uint64_t remaining;
    int64_t claimed;

    diminuto_coreable();

    printf("%10s %10s %10s %10s %10s\n",
        "requested",
        "remaining",
        "claimed",
        "measured",
        "difference");

    for (requested = 0; requested < 60000000; requested = requested ? requested * 2 : 1) {
        then = diminuto_time();
        remaining = diminuto_delay(requested, 0);
        now = diminuto_time();
        claimed = requested - remaining;
        measured = now - then;
        printf("%10llu %10llu %10lld %10lld %10lld\n",
            requested,
            remaining,
            claimed,
            measured,
            measured - requested);
    }

    return 0;
}
