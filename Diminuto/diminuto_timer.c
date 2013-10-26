/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2009 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "com/diag/diminuto/diminuto_timer.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "diminuto_frequency.h"
#include <sys/time.h>

static diminuto_ticks_t diminuto_timer(int which, diminuto_ticks_t ticks, int periodic)
{
    struct itimerval timer;
    struct itimerval remaining;

    timer.it_value.tv_sec = COM_DIAG_DIMINUTO_TICKS_FROM(ticks, 1);
    timer.it_value.tv_usec = COM_DIAG_DIMINUTO_TICKS_FROM(ticks % COM_DIAG_DIMINUTO_FREQUENCY, 1000000);

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

    ticks = COM_DIAG_DIMINUTO_TICKS_TO(remaining.it_value.tv_sec, 1);
    ticks += COM_DIAG_DIMINUTO_TICKS_TO(remaining.it_value.tv_usec, 1000000);

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
