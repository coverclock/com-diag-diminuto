/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2008 Digital Aggregates Corporation, Arvada CO 80001-0587 USA<BR>
 * Licensed under the GNU GPL V2<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "diminuto_delay.h"
#include "diminuto_log.h"
#include <stdio.h>
#include <errno.h>
/* #define _POSIX_C_SOURCE 199309 */
#include <time.h>

uint32_t diminuto_delay(uint32_t microseconds, int interruptable)
{
    struct timespec delay;
    struct timespec remaining;
    struct timespec result;

    delay.tv_sec = microseconds / 1000000U;
    delay.tv_nsec = (microseconds % 1000000U) * 1000U;

    remaining = delay;

    result.tv_sec = 0;
    result.tv_nsec = 0;

    while (nanosleep(&delay, &remaining) < 0) {
        if (errno != EINTR) {
            diminuto_perror("diminuto_delay: nanosleep");
            result = remaining;
            break;
        }
        if (interruptable) {
            result = remaining;
            break;
        }
        delay = remaining;
    }

    return (result.tv_sec * 1000000UL) + (result.tv_nsec / 1000UL);
}
