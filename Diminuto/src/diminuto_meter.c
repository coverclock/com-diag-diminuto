/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2022 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief Implements a Meter with peak and sustainable rates.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * Implements a Meter that measures peak and sustainable rates.
 */

#include "com/diag/diminuto/diminuto_meter.h"
#include "com/diag/diminuto/diminuto_minmaxof.h"
#include <math.h>
#include <errno.h>

diminuto_meter_t * diminuto_meter_reset(diminuto_meter_t * mp, diminuto_sticks_t now)
{
    diminuto_meter_t * result = (diminuto_meter_t *)0;

    if (now >= 0) {
        mp->start = now;
        mp->last = now;
        mp->shortest = HUGE_VAL;
        mp->events = 0;
        result = mp;
    }

    return result;
}

int diminuto_meter_events(diminuto_meter_t * mp, diminuto_sticks_t now, size_t events)
{
    int result = 0;
    double iat = 0;

    if (now < 0) {
        errno = EINVAL;
        result = -1;
    } else if (events == 0) {
        /* Do nothing. */
    } else if (mp->events == 0) {
        mp->last = now;
        mp->events = events;
        mp->burst = events;
    } else if (now < mp->last) {
        errno = ERANGE;
        result = -1;
    } else if ((maximumof(typeof(mp->events)) - mp->events) < events) {
        errno = EOVERFLOW;
        result = -1;
    } else {
        iat = now - mp->last;
        iat /= events;
        if (iat < mp->shortest) {
            mp->shortest = iat;
        }
        mp->last = now;
        mp->events += events;
        if (events > mp->burst) {
            mp->burst = events;
        }
    }

    return result;
}
