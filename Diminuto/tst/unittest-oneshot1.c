/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2009-2022 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is a unit test of the Timer feature using setitimer One Shots.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is a unit test of the Timer feature using setitimer One Shots.
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_delay.h"
#include "com/diag/diminuto/diminuto_alarm.h"
#include "com/diag/diminuto/diminuto_time.h"
#include "com/diag/diminuto/diminuto_timer.h"
#include "com/diag/diminuto/diminuto_frequency.h"
#include "../src/diminuto_timer.h"
#include <errno.h>

int main(int argc, char ** argv)
{
    diminuto_sticks_t value;
    diminuto_sticks_t result;
    diminuto_ticks_t hertz;
    diminuto_ticks_t frequency;
    diminuto_ticks_t then;
    diminuto_ticks_t now;
    diminuto_ticks_t measured;
    diminuto_ticks_t requested;
    diminuto_ticks_t delayed;
    diminuto_ticks_t remaining;
    diminuto_ticks_t claimed;
    diminuto_sticks_t difference;
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
    diminuto_alarm_install(0);

    hertz = diminuto_frequency();

    frequency = diminuto_timer_frequency();
    CHECKPOINT("timer frequency %llu Hz\n", (long long unsigned int)frequency);

    CHECKPOINT("%21s %21s %21s %11s\n", "requested", "claimed", "measured", "error");

    for (requested = hertz / 1000; requested <= (hertz * 9 * 60); requested *= 2) {
        EXPECT(!diminuto_alarm_check());
        ASSERT(diminuto_timer_oneshot(requested) != (diminuto_sticks_t)-1);
        ASSERT((result = diminuto_time_elapsed()) != (diminuto_sticks_t)-1);
        then = result;
	    delayed = diminuto_timer_window(requested);
        remaining = diminuto_delay(delayed, !0);
        ASSERT((result = diminuto_time_elapsed()) != (diminuto_sticks_t)-1);
        now = result;
        ASSERT(now >= then);
        ASSERT(diminuto_timer_oneshot(0) != (diminuto_sticks_t)-1);
        claimed = delayed - remaining;
        measured = now - then;
        difference = measured - requested;
        delta = (100.0 * difference) / requested;
        rs = (diminuto_time_duration(requested, &rday, &rhour, &rminute, &rsecond, &rtick) < 0) ? '-' : '+';
        cs = (diminuto_time_duration(claimed,  &cday, &chour, &cminute, &csecond, &ctick) < 0) ? '-' : '+';
        ms = (diminuto_time_duration(measured,  &mday, &mhour, &mminute, &msecond, &mtick) < 0) ? '-' : '+';
        CHECKPOINT("%c%1.1d/%2.2d:%2.2d:%2.2d.%9.9llu %c%1.1d/%2.2d:%2.2d:%2.2d.%9.9llu %c%1.1d/%2.2d:%2.2d:%2.2d.%9.9llu %10.3lf%%\n"
            , rs, rday, rhour, rminute, rsecond, (long long unsigned int)rtick
            , cs, cday, chour, cminute, csecond, (long long unsigned int)ctick
            , ms, mday, mhour, mminute, msecond, (long long unsigned int)mtick
            , delta
        );
        EXPECT(diminuto_alarm_check());
        EXPECT(!diminuto_alarm_check());
    }

    STATUS();

    EXIT();
}
