/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2009 Digital Aggregates Corporation, Arvada CO 80001-0587 USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "diminuto/diminuto_unittest.h"
#include "diminuto/diminuto_core.h"
#include "diminuto/diminuto_delay.h"
#include "diminuto/diminuto_alarm.h"
#include "diminuto/diminuto_time.h"
#include "diminuto/diminuto_timer.h"
#include <stdio.h>
#include <errno.h>

int main(int argc, char ** argv)
{
    diminuto_usec_t then;
    diminuto_usec_t now;
    int64_t measured;
    diminuto_usec_t remaining;
    int64_t computed;
    diminuto_usec_t requested;
    int ii;

    diminuto_core_enable();
    diminuto_alarm_install(0);

    printf("%10s %10s %10s\n",
        "requested", "computed", "measured");

    for (requested = 1000; requested < 12000000; requested *= 2) {

        diminuto_timer_periodic(requested);
        then = diminuto_time();

        for (ii = 0; ii < 5; ++ii) {
            EXPECT(!diminuto_alarm_check());
            remaining = diminuto_delay(requested * 2, !0);
            EXPECT(diminuto_alarm_check());
            EXPECT(!diminuto_alarm_check());
            now = diminuto_time();
            computed = (requested * 2) - remaining;
            measured = now - then;
            printf("%10llu %10llu %10lld\n",
                requested, computed, measured);
            then = now;
        }

        diminuto_timer_periodic(0);
    }

    return 0;
}

