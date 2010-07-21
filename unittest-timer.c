/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2009 Digital Aggregates Corporation, Arvada CO 80001-0587 USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "diminuto_unittest.h"
#include "diminuto_core.h"
#include "diminuto_delay.h"
#include "diminuto_alarm.h"
#include "diminuto_time.h"
#include "diminuto_timer.h"
#include <stdio.h>
#include <errno.h>

int main(int argc, char ** argv)
{
    uint64_t then;
    uint64_t now;
    int64_t measured;
    uint64_t requested;
    uint64_t remaining;
    int64_t computed;

    diminuto_core_enable();
    diminuto_alarm_install(0);

    printf("%10s %10s %10s\n",
        "requested", "computed", "measured");

    for (requested = 1; requested < 60000000; requested *= 2) {
        EXPECT(!diminuto_alarm_check());
        diminuto_timer_oneshot(requested);
        then = diminuto_time();
        remaining = diminuto_delay(requested * 2, !0);
        now = diminuto_time();
        diminuto_timer_oneshot(0);
        EXPECT(diminuto_alarm_check());
        EXPECT(!diminuto_alarm_check());
        computed = (requested * 2) - remaining;
        measured = now - then;
        printf("%10llu %10llu %10lld\n",
            requested, computed, measured);
    }

    EXIT();
}
