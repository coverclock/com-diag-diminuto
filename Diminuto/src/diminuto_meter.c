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
#include <errno.h>

diminuto_meter_t * diminuto_meter_reset(diminuto_meter_t * meterp, diminuto_ticks_t now)
{
    meterp->start = now;
    meterp->last = now;
    meterp->peak = 0.0;
    meterp->events = 0;
    meterp->burst = 0;
    meterp->count = 0;

    return meterp;
}

int diminuto_meter_events(diminuto_meter_t * meterp, diminuto_ticks_t now, size_t events)
{
    int result = 0;

    if (now < meterp->last) {
        errno = ERANGE;
        result = -1;
    } else if (events == 0) {
        meterp->last = now;
    } else if ((DIMINUTO_METER_OVERFLOW - meterp->events) < events) {
        meterp->last = now;
        meterp->events = DIMINUTO_METER_OVERFLOW;
        errno = EOVERFLOW;
        result = -1;
    } else if ((DIMINUTO_METER_OVERFLOW - meterp->count) < 1) {
        meterp->last = now;
        meterp->count = DIMINUTO_METER_OVERFLOW;
        errno = EOVERFLOW;
        result = -1;
    } else if (meterp->events == 0) {
        meterp->last = now;
        meterp->events = events;
        meterp->burst = events;
        meterp->count += 1;
    } else if (now == meterp->last) {
        meterp->peak = DIMINUTO_METER_ERROR;
        meterp->last = now;
        meterp->events += events;
        if (events > meterp->burst) {
            meterp->burst = events;
        }
        meterp->count += 1;
    } else {
        diminuto_ticks_t interarrival = 0;
        double rate = 0.0;
        interarrival = now - meterp->last;
        rate = events;
        rate *= diminuto_frequency();
        rate /= interarrival;
        if (rate > meterp->peak) {
            meterp->peak = rate;
        }
        meterp->last = now;
        meterp->events += events;
        if (events > meterp->burst) {
            meterp->burst = events;
        }
        meterp->count += 1;
    }

    return result;
}

double diminuto_meter_sustained(const diminuto_meter_t * meterp)
{
    double result = 0.0;

    if (meterp->events == 0) {
        /* Do nothing. */
    } else if (meterp->last <= meterp->start) {
        result = DIMINUTO_METER_ERROR;
    } else {
        result = meterp->events;
        result *= diminuto_frequency();
        result /= (meterp->last - meterp->start);
    }

    return result;
}
