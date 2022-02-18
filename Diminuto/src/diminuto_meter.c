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
#include "com/diag/diminuto/diminuto_frequency.h"
#include "com/diag/diminuto/diminuto_typeof.h"
#include <errno.h>

diminuto_meter_t * diminuto_meter_reset(diminuto_meter_t * mp, diminuto_sticks_t now)
{
    diminuto_meter_t * result = (diminuto_meter_t *)0;

    if (now >= 0) {
        mp->start = now;
        mp->last = now;
        mp->peak = 0.0;
        mp->events = 0;
        result = mp;
    }

    return result;
}

int diminuto_meter_events(diminuto_meter_t * mp, diminuto_sticks_t now, size_t events)
{
    int result = 0;
    static const typeof(mp->events) LIMIT = maximumof(typeof(mp->events));

    if (now < 0) {
        errno = EINVAL;
        result = -1;
    } else if (now < mp->last) {
        errno = ERANGE;
        result = -1;
    } else if (events == 0) {
        mp->last = now;
    } else if ((LIMIT - mp->events) < events) {
        mp->last = now;
        mp->events = LIMIT;
        errno = EOVERFLOW;
        result = -1;
    } else if (mp->events == 0) {
        mp->last = now;
        mp->events = events;
        mp->burst = events;
    } else if (now == mp->last) {
        mp->peak = HUGE_VAL;
        mp->last = now;
        mp->events += events;
        if (events > mp->burst) {
            mp->burst = events;
        }
    } else {
        diminuto_ticks_t interarrival = 0;
        double rate = 0.0;
        interarrival = now - mp->last;
        rate = events;
        rate *= diminuto_frequency();
        rate /= interarrival;
        if (rate > mp->peak) {
            mp->peak = rate;
        }
        mp->last = now;
        mp->events += events;
        if (events > mp->burst) {
            mp->burst = events;
        }
    }

    return result;
}

double diminuto_meter_peak(const diminuto_meter_t * mp)
{
    return (mp->events > 0) ? mp->peak : 0.0;;
}

double diminuto_meter_sustained(const diminuto_meter_t * mp)
{
    double result = 0.0;

    if (mp->events == 0) {
        /* Do nothing. */
    } else if (mp->last < mp->start) {
        result = HUGE_VAL;
    } else {
        result = mp->events;
        result *= diminuto_frequency();
        result /= (mp->last - mp->start);
    }

    return result;
}
