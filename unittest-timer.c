/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2009 Digital Aggregates Corporation, Arvada CO 80001-0587 USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "diminuto_coreable.h"
#include "diminuto_delay.h"
#include "diminuto_alarm.h"
#include "diminuto_time.h"
#include <stdio.h>
#include <errno.h>

int main(int argc, char ** argv)
{
    uint64_t then;
    uint64_t now;
    int64_t measured;
    uint32_t requested;
    uint32_t remaining;
    int32_t computed;

    diminuto_coreable();
    diminuto_alarmable();

    printf("%10s %10s %10s\n",
        "requested", "computed", "measured");

    for (requested = 1; requested < 60000000; requested *= 2) {
        diminuto_timer(requested);
        then = diminuto_time();
        remaining = diminuto_delay(requested * 2, !0);
        now = diminuto_time();
        diminuto_timer(0);
        computed = (requested * 2) - remaining;
        measured = now - then;
        printf("%10lu %10lu %10lld\n",
            requested, computed, measured);
    }

    return 0;
}
