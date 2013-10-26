/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2008-2013 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "com/diag/diminuto/diminuto_delay.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "diminuto_frequency.h"
#include <sched.h>
#include <errno.h>
/* #define _POSIX_C_SOURCE 199309 */
#include <time.h>

diminuto_ticks_t diminuto_delay(diminuto_ticks_t ticks, int interruptable)
{
    struct timespec delay;
    struct timespec remaining;
    struct timespec result;

    delay.tv_sec = COM_DIAG_DIMINUTO_TICKS_FROM(ticks , 1);
    delay.tv_nsec = COM_DIAG_DIMINUTO_TICKS_FROM((ticks % COM_DIAG_DIMINUTO_FREQUENCY), 1000000000);

    remaining = delay;

    result.tv_sec = 0;
    result.tv_nsec = 0;

    while (nanosleep(&delay, &remaining) < 0) {
        if (errno != EINTR) {
            diminuto_perror("diminuto_delay: nanosleep");
            result = remaining;
            break;
        } else if (interruptable) {
            result = remaining;
            break;
        } else {
            delay = remaining;
        }
    }

    ticks = COM_DIAG_DIMINUTO_TICKS_TO(result.tv_sec, 1);
    ticks += COM_DIAG_DIMINUTO_TICKS_TO(result.tv_nsec, 1000000000);

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
