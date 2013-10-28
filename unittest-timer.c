/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2009-2013 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_core.h"
#include "com/diag/diminuto/diminuto_delay.h"
#include "com/diag/diminuto/diminuto_alarm.h"
#include "com/diag/diminuto/diminuto_time.h"
#include "com/diag/diminuto/diminuto_timer.h"
#include <stdio.h>
#include <errno.h>

int main(int argc, char ** argv)
{
    diminuto_ticks_t then;
    diminuto_ticks_t now;
    diminuto_ticks_t measured;
    diminuto_ticks_t requested;
    diminuto_ticks_t remaining;
    diminuto_ticks_t computed;
    diminuto_ticks_t hertz;
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

    diminuto_core_enable();
    diminuto_alarm_install(0);

    hertz = diminuto_time_resolution();

    printf("%21s %21s %21s\n",
        "requested", "computed", "measured");

    for (requested = 1; requested <= (hertz * 240); requested *= 2) {
        EXPECT(!diminuto_alarm_check());
        diminuto_timer_oneshot(requested);
        then = diminuto_time_elapsed();
        remaining = diminuto_delay(requested * 2, !0);
        now = diminuto_time_elapsed();
        diminuto_timer_oneshot(0);
        EXPECT(diminuto_alarm_check());
        EXPECT(!diminuto_alarm_check());
        computed = (requested * 2) - remaining;
        measured = now - then;
        rs = (diminuto_time_duration(requested, &rday, &rhour, &rminute, &rsecond, &rtick) < 0) ? '-' : '+';
        cs = (diminuto_time_duration(computed,  &cday, &chour, &cminute, &csecond, &ctick) < 0) ? '-' : '+';
        ms = (diminuto_time_duration(measured,  &mday, &mhour, &mminute, &msecond, &mtick) < 0) ? '-' : '+';
        printf("%c%1.1d/%2.2d:%2.2d:%2.2d.%9.9d %c%1.1d/%2.2d:%2.2d:%2.2d.%9.9d %c%1.1d/%2.2d:%2.2d:%2.2d.%9.9d\n"
        	, rs, rday, rhour, rminute, rsecond, rtick
        	, cs, cday, chour, cminute, csecond, ctick
        	, ms, mday, mhour, mminute, msecond, mtick
        );
    }

    EXIT();
}
