/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2009-2017 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_alarm.h"
#include "com/diag/diminuto/diminuto_timer.h"
#include "com/diag/diminuto/diminuto_time.h"
#include "com/diag/diminuto/diminuto_delay.h"
#include "com/diag/diminuto/diminuto_frequency.h"
#include "com/diag/diminuto/diminuto_uninterruptiblesection.h"
#include <stdio.h>
#include <errno.h>

int main(int argc, char ** argv)
{
    diminuto_ticks_t hertz;
    diminuto_ticks_t remaining;
    int alarmed;

    diminuto_alarm_install(0);
    hertz = diminuto_frequency();

    EXPECT(diminuto_timer_periodic(hertz / 20) >= 0);
    remaining = diminuto_delay(hertz, !0);
    alarmed = diminuto_alarm_check();
    EXPECT(diminuto_timer_periodic(0) >= 0);
    (void)diminuto_alarm_check();

    EXPECT(remaining > 0);
    EXPECT(alarmed);

    DIMINUTO_UNINTERRUPTIBLE_SECTION_BEGIN(SIGTERM, SIGALRM, SIGCHLD);

        EXPECT(diminuto_timer_periodic(hertz / 20) >= 0);
        remaining = diminuto_delay(hertz, !0);
        alarmed = diminuto_alarm_check();
        EXPECT(diminuto_timer_periodic(0) >= 0);
        (void)diminuto_alarm_check();

    DIMINUTO_UNINTERRUPTIBLE_SECTION_END;

    EXPECT(remaining == 0);
    EXPECT(!alarmed);

    EXIT();
}
