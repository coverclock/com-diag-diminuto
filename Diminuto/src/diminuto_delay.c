/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2008-2015 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "com/diag/diminuto/diminuto_delay.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_frequency.h"
#include <sched.h>
#include <errno.h>
#include <time.h>

diminuto_ticks_t diminuto_delay(diminuto_ticks_t ticks, int interruptible)
{
    struct timespec delay;
    struct timespec remaining;
    struct timespec result;

    delay.tv_sec = diminuto_frequency_ticks2wholeseconds(ticks);
    delay.tv_nsec = diminuto_frequency_ticks2fractionalseconds(ticks, diminuto_delay_frequency());

    remaining = delay;

    result.tv_sec = 0;
    result.tv_nsec = 0;

    while (nanosleep(&delay, &remaining) < 0) {
        if (errno != EINTR) {
            diminuto_perror("diminuto_delay: nanosleep");
            result = remaining;
            break;
        } else if (interruptible) {
            result = remaining;
            break;
        } else {
            delay = remaining;
        }
    }

    ticks = diminuto_frequency_seconds2ticks(result.tv_sec, result.tv_nsec, diminuto_delay_frequency());

    return ticks;
}

int diminuto_yield(void) {
    int rc;

    rc = sched_yield();
    if (rc < 0) {
        diminuto_perror("diminuto_yield: sched_yield");
    }

    return rc;
}
