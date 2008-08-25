/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2008 Digital Aggregates Corporation, Arvada CO 80001-0587 USA<BR>
 * Licensed under the GNU GPL V2<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "diminuto_time.h"
#include "diminuto_log.h"
#include <stdio.h>
#include <sys/time.h>

uint64_t diminuto_time()
{
    struct timeval elapsed;

    if (gettimeofday(&elapsed, (struct timezone *)0) < 0) {
        diminuto_perror("diminuto_time: gettimeofday");
        return 0ULL;
    }

    return (elapsed.tv_sec * 1000000ULL) + elapsed.tv_usec;
}
