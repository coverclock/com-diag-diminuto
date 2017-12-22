/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2016 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 *
 * USAGE
 *
 * elapsedsleep DELAY [ UNITS ]<BR>
 *
 * EXAMPLES
 *
 * elapsedsleep 1<BR>
 * elapsedsleep 1 1<BR>
 * elapsedsleep 1000 1000<BR>
 * elapsedsleep 1000000 1000000<BR>
 * elapsedsleep 1000000000 1000000000<BR>
 *
 * ABSTRACT
 *
 * Sleeps for the specified time in the specified units. Defaults to seconds.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_delay.h"
#include "com/diag/diminuto/diminuto_frequency.h"

int main(int argc, char ** argv)
{
    diminuto_sticks_t delay;
    diminuto_sticks_t units;
    diminuto_sticks_t ticks;
    char * endptr;

    endptr = (char *)0;
    if (argc < 2) {
        delay = 0;
    } else if ((delay = strtoll(argv[1], &endptr, 0)) < 0) {
        errno = EINVAL;
        diminuto_perror(argv[1]);
        return 1;
    } else if ((endptr == (char *)0) || (*endptr != '\0')) {
        errno = EINVAL;
        diminuto_perror(argv[1]);
        return 1;
    } else {
        /* Do nothing. */
    }

    endptr = (char *)0;
    if (argc < 3) {
        units = 1;
    } else if ((units = strtoll(argv[2], &endptr, 0)) <= 0) {
        errno = EINVAL;
        diminuto_perror(argv[2]);
        return 1;
    } else if ((endptr == (char *)0) || (*endptr != '\0')) {
        errno = EINVAL;
        diminuto_perror(argv[2]);
        return 1;
    } else {
        /* Do nothing. */
    }

    ticks = (delay == 0) ? 0 : diminuto_frequency_units2ticks(delay, units);

    if (ticks <= 0) {
        diminuto_yield();
    } else {
        diminuto_delay_interruptible(ticks);
    }

    return 0;
}
