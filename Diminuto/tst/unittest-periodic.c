/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2009-2014 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
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

int main(int argc, char ** argv)
{
	diminuto_ticks_t hertz;
    diminuto_ticks_t then;
    diminuto_ticks_t now;
    int64_t measured;
    diminuto_ticks_t remaining;
    int64_t computed;
    diminuto_ticks_t requested;
    int ii;
    char rs;
    char cs;
    char ms;
    int rday;
    int rhour;
    int rminute;
    int rsecond;
    int rtick;
    int cday;
    int chour;
    int cminute;
    int csecond;
    int ctick;
    int mday;
    int mhour;
    int mminute;
    int msecond;
    int mtick;
    double delta;

    SETLOGMASK();

    diminuto_core_enable();

    hertz = diminuto_frequency();

    diminuto_alarm_install(0);

    DIMINUTO_LOG_INFORMATION("%21s %21s %21s %11s\n",
        "requested", "computed", "measured", "error");

    for (requested = hertz / 8; requested < (16 * hertz); requested *= 2) {
        diminuto_timer_periodic(requested);
        then = diminuto_time_elapsed();
        for (ii = 0; ii < 5; ++ii) {
            EXPECT(!diminuto_alarm_check());
            remaining = diminuto_delay(requested * 2, !0);
            EXPECT(diminuto_alarm_check());
            EXPECT(!diminuto_alarm_check());
            now = diminuto_time_elapsed();
            computed = (requested * 2) - remaining;
            measured = now - then;
            delta = (100.0 * (measured - requested)) / requested;
            rs = (diminuto_time_duration(requested, &rday, &rhour, &rminute, &rsecond, &rtick) < 0) ? '-' : '+';
            cs = (diminuto_time_duration(computed,  &cday, &chour, &cminute, &csecond, &ctick) < 0) ? '-' : '+';
            ms = (diminuto_time_duration(measured,  &mday, &mhour, &mminute, &msecond, &mtick) < 0) ? '-' : '+';
            DIMINUTO_LOG_INFORMATION("%c%1.1d/%2.2d:%2.2d:%2.2d.%9.9d %c%1.1d/%2.2d:%2.2d:%2.2d.%9.9d %c%1.1d/%2.2d:%2.2d:%2.2d.%9.9d %10.3lf%%\n"
            	, rs, rday, rhour, rminute, rsecond, rtick
            	, cs, cday, chour, cminute, csecond, ctick
            	, ms, mday, mhour, mminute, msecond, mtick
            	, delta
            );
            then = now;
        }
        diminuto_timer_periodic(0);
    }

    EXIT();
}

