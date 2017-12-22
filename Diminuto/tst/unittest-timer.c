/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2009-2015 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
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
    diminuto_ticks_t remaining;
    diminuto_ticks_t computed;
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
    void * is;
    void * was;

    SETLOGMASK();

    diminuto_core_enable();
    diminuto_alarm_install(0);

    hertz = diminuto_frequency();

    frequency = diminuto_timer_frequency();
    DIMINUTO_LOG_INFORMATION("timer frequency %llu Hz\n", frequency);

    ASSERT((is = diminuto_ptimer_get()) == (void *)-1);

    DIMINUTO_LOG_INFORMATION("%21s %21s %21s %11s\n",
        "requested", "computed", "measured", "error");

    for (requested = hertz / 1000; requested <= (hertz * 9 * 60); requested *= 2) {
        EXPECT(!diminuto_alarm_check());
        ASSERT(diminuto_timer_oneshot(requested) != (diminuto_sticks_t)-1);
        ASSERT((was = diminuto_ptimer_get()) != (void *)-1);
        ASSERT((result = diminuto_time_elapsed()) != (diminuto_sticks_t)-1);
        then = result;
        remaining = diminuto_delay(requested * 2, !0);
        ASSERT((result = diminuto_time_elapsed()) != (diminuto_sticks_t)-1);
        now = result;
        ASSERT(now >= then);
        ASSERT((is = diminuto_ptimer_get()) != (void *)-1);
        ASSERT(is == was);
        ASSERT(diminuto_timer_oneshot(0) != (diminuto_sticks_t)-1);
        ASSERT((is = diminuto_ptimer_get()) == (void *)-1);
        EXPECT(diminuto_alarm_check());
        EXPECT(!diminuto_alarm_check());
        computed = (requested * 2) - remaining;
        measured = now - then;
        difference = measured - requested;
        delta = (100.0 * difference) / requested;
        rs = (diminuto_time_duration(requested, &rday, &rhour, &rminute, &rsecond, &rtick) < 0) ? '-' : '+';
        cs = (diminuto_time_duration(computed,  &cday, &chour, &cminute, &csecond, &ctick) < 0) ? '-' : '+';
        ms = (diminuto_time_duration(measured,  &mday, &mhour, &mminute, &msecond, &mtick) < 0) ? '-' : '+';
        DIMINUTO_LOG_INFORMATION("%c%1.1d/%2.2d:%2.2d:%2.2d.%9.9llu %c%1.1d/%2.2d:%2.2d:%2.2d.%9.9llu %c%1.1d/%2.2d:%2.2d:%2.2d.%9.9llu %10.3lf%%\n"
        	, rs, rday, rhour, rminute, rsecond, rtick
        	, cs, cday, chour, cminute, csecond, ctick
        	, ms, mday, mhour, mminute, msecond, mtick
        	, delta
        );
    }

    ASSERT((is = diminuto_ptimer_get()) == (void *)-1);

    EXIT();
}
