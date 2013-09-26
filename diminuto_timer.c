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
#include <sys/time.h>

static diminuto_usec_t diminuto_timer(int which, diminuto_usec_t microseconds, int periodic)
{
    struct itimerval timer;
    struct itimerval remaining;

    timer.it_value.tv_sec = microseconds / 1000000UL;
    timer.it_value.tv_usec = microseconds % 1000000UL;

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

    microseconds = remaining.it_value.tv_sec;
    microseconds *= 1000000ULL;
    microseconds += remaining.it_value.tv_usec;

    return microseconds;
}

diminuto_usec_t diminuto_timer_oneshot(diminuto_usec_t microseconds)
{
    return diminuto_timer(ITIMER_REAL, microseconds, 0);
}

diminuto_usec_t diminuto_timer_periodic(diminuto_usec_t microseconds)
{
    return diminuto_timer(ITIMER_REAL, microseconds, !0);
}
