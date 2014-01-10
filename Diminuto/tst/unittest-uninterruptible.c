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
#include "com/diag/diminuto/diminuto_alarm.h"
#include "com/diag/diminuto/diminuto_timer.h"
#include "com/diag/diminuto/diminuto_time.h"
#include "com/diag/diminuto/diminuto_delay.h"
#include "com/diag/diminuto/diminuto_uninterruptiblesection.h"
#include <stdio.h>
#include <errno.h>

int main(int argc, char ** argv)
{
	diminuto_ticks_t hertz;
    diminuto_ticks_t remaining;
    int alarmed;

    diminuto_alarm_install(0);
    hertz = diminuto_time_frequency();

	diminuto_timer_periodic(hertz / 20);
	remaining = diminuto_delay(hertz, !0);
	alarmed = diminuto_alarm_check();
	diminuto_timer_periodic(0);
	(void)diminuto_alarm_check();

    EXPECT(remaining > 0);
    EXPECT(alarmed);

    DIMINUTO_UNINTERRUPTIBLE_SECTION_BEGIN(SIGALRM);

    	diminuto_timer_periodic(hertz / 20);
    	remaining = diminuto_delay(hertz, !0);
    	alarmed = diminuto_alarm_check();
    	diminuto_timer_periodic(0);
    	(void)diminuto_alarm_check();

    DIMINUTO_UNINTERRUPTIBLE_SECTION_END;

    EXPECT(remaining == 0);
    EXPECT(!alarmed);

    EXIT();
}
