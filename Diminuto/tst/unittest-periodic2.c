/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2020 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is a unit test of the Timer feature with Periodic callbacks.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is a unit test of the Timer feature with Periodic callbacks.
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_core.h"
#include "com/diag/diminuto/diminuto_delay.h"
#include "com/diag/diminuto/diminuto_alarm.h"
#include "com/diag/diminuto/diminuto_time.h"
#include "com/diag/diminuto/diminuto_timer.h"
#include "com/diag/diminuto/diminuto_frequency.h"
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

static void * callback(void * cp)
{
    int sig = -1;
    pid_t pid = -1;
    int rc = -1;

    sig = (int)(intptr_t)cp;
    pid = getpid();
    rc = kill(pid, sig);
    if (rc < 0) {
        diminuto_perror("kill");
    }

    return (void *)(intptr_t)rc;
}

int main(int argc, char ** argv)
{
    diminuto_timer_t timer;
    diminuto_sticks_t result;
    diminuto_ticks_t hertz;
    diminuto_ticks_t frequency;
    diminuto_ticks_t then;
    diminuto_ticks_t now;
    diminuto_sticks_t measured;
    diminuto_ticks_t remaining;
    diminuto_sticks_t claimed;
    diminuto_sticks_t requested;
    diminuto_sticks_t difference;
    int ii;
    char rs;
    char cs;
    char ms;
    int rday;
    int rhour;
    int rminute;
    int rsecond;
    diminuto_ticks_t rtick;
    int cday;
    int chour;
    int cminute;
    int csecond;
    diminuto_ticks_t ctick;
    int mday;
    int mhour;
    int mminute;
    int msecond;
    diminuto_ticks_t mtick;
    double delta;

    SETLOGMASK();

    TEST();

    diminuto_core_enable();

    hertz = diminuto_frequency();

    frequency = diminuto_timer_frequency();
    CHECKPOINT("timer frequency %llu Hz\n", frequency);

    diminuto_alarm_install(0);

    CHECKPOINT("%21s %21s %21s %11s\n",
        "requested", "claimed", "measured", "error");

    ASSERT(diminuto_timer_init_periodic(&timer, callback) == &timer);

    for (requested = hertz / 8; requested < (16 * hertz); requested *= 2) {
        ASSERT(diminuto_timer_error(&timer) == 0);
        ASSERT(diminuto_timer_start(&timer, requested, (void *)SIGALRM) >= 0);
        ASSERT((result = diminuto_time_elapsed()) != (diminuto_sticks_t)-1);
        then = result;
        for (ii = 0; ii < 5; ++ii) {
            EXPECT(!diminuto_alarm_check());
            remaining = diminuto_delay(requested * 2, !0);
            EXPECT(remaining >= 0);
            ASSERT(diminuto_timer_error(&timer) == 0);
            EXPECT(diminuto_alarm_check());
            EXPECT(!diminuto_alarm_check());
            ASSERT((result = diminuto_time_elapsed()) != (diminuto_sticks_t)-1);
            now = result;
            claimed = (requested * 2) - remaining;
            measured = now - then;
            ASSERT(measured > 0);
            difference = measured - requested;
            delta = (100.0 * difference) / requested;
            rs = (diminuto_time_duration(requested, &rday, &rhour, &rminute, &rsecond, &rtick) < 0) ? '-' : '+';
            cs = (diminuto_time_duration(claimed,  &cday, &chour, &cminute, &csecond, &ctick) < 0) ? '-' : '+';
            ms = (diminuto_time_duration(measured,  &mday, &mhour, &mminute, &msecond, &mtick) < 0) ? '-' : '+';
            CHECKPOINT("%c%1.1d/%2.2d:%2.2d:%2.2d.%9.9llu %c%1.1d/%2.2d:%2.2d:%2.2d.%9.9llu %c%1.1d/%2.2d:%2.2d:%2.2d.%9.9llu %10.3lf%%\n"
                , rs, rday, rhour, rminute, rsecond, rtick
                , cs, cday, chour, cminute, csecond, ctick
                , ms, mday, mhour, mminute, msecond, mtick
                , delta
            );
            then = now;
        }
        ASSERT(diminuto_timer_stop(&timer) >= 0);
        ASSERT(diminuto_timer_error(&timer) == 0);
    }

    ASSERT(diminuto_timer_fini(&timer) == (diminuto_timer_t *)0);

    STATUS();

    EXIT();
}


