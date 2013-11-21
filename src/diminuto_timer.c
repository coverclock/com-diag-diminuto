/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2009-2013 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "com/diag/diminuto/diminuto_timer.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_frequency.h"
#include <sys/time.h>

#define COM_DIAG_DIMINUTO_TIMER_FREQUENCY 1000000LL

diminuto_ticks_t diminuto_timer_frequency()
{
	return COM_DIAG_DIMINUTO_TIMER_FREQUENCY;
}

static diminuto_ticks_t diminuto_timer(int which, diminuto_ticks_t ticks, int periodic)
{
    struct itimerval timer;
    struct itimerval remaining;

    timer.it_value.tv_sec = diminuto_frequency_ticks2wholeseconds(ticks);
    timer.it_value.tv_usec = diminuto_frequency_ticks2fractionalseconds(ticks, COM_DIAG_DIMINUTO_TIMER_FREQUENCY);

    if (periodic) {
        timer.it_interval = timer.it_value;
    } else {
        timer.it_interval.tv_sec = 0;
        timer.it_interval.tv_usec = 0;
    }

    remaining = timer;

    if (setitimer(which, &timer, &remaining) < 0) {
        diminuto_perror("diminuto_timer: setitimer");
    }

    ticks = diminuto_frequency_seconds2ticks(remaining.it_value.tv_sec, remaining.it_value.tv_usec, COM_DIAG_DIMINUTO_TIMER_FREQUENCY);

    return ticks;
}

diminuto_ticks_t diminuto_timer_oneshot(diminuto_ticks_t ticks)
{
    return diminuto_timer(ITIMER_REAL, ticks, 0);
}

diminuto_ticks_t diminuto_timer_periodic(diminuto_ticks_t ticks)
{
    return diminuto_timer(ITIMER_REAL, ticks, !0);
}
