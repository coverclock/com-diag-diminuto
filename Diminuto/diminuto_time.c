/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2008 Digital Aggregates Corporation, Arvada CO 80001-0587 USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "diminuto_time.h"
#include "diminuto_log.h"
#include <stdio.h>
#include <sys/time.h>

diminuto_usec_t diminuto_time()
{
    struct timeval elapsed;
    diminuto_usec_t microseconds = 0ULL;

    if (gettimeofday(&elapsed, (struct timezone *)0) < 0) {
        diminuto_perror("diminuto_time: gettimeofday");
    } else {
        microseconds = elapsed.tv_sec;
        microseconds *= 1000000ULL;
        microseconds += elapsed.tv_usec;
    }

    return microseconds;
}
