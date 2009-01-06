/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2009 Digital Aggregates Corporation, Arvada CO 80001-0587 USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "diminuto_periodic.h"
#include "diminuto_log.h"
#include <errno.h>
#include <sys/time.h>

uint32_t diminuto_periodic(uint32_t microseconds)
{
    struct itimerval timer;
    struct itimerval remaining;

    timer.it_value.tv_sec = microseconds / 1000000;
    timer.it_value.tv_usec = microseconds % 1000000;

    timer.it_interval = timer.it_value;

    remaining = timer;

    if (setitimer(ITIMER_REAL, &timer, &remaining) < 0) {
        diminuto_perror("diminuto_periodic: setitimer");
    }

    return (remaining.it_value.tv_sec * 1000000UL) + remaining.it_value.tv_usec;
}
