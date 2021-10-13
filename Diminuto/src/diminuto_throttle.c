/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2014-2021 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is the implementation of the Throttle feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is the implementation of the Throttle feature.
 */

#include "com/diag/diminuto/diminuto_throttle.h"
#include "com/diag/diminuto/diminuto_frequency.h"
#include "com/diag/diminuto/diminuto_log.h"

diminuto_ticks_t diminuto_throttle_interarrivaltime(size_t numerator, size_t denominator, diminuto_ticks_t frequency)
{
    diminuto_ticks_t increment;

    /* i = f / (n / d) = f * d / n */

    increment = frequency;
    if (denominator > 1) {
        increment *= denominator;
    }
    if (numerator <= 1) {
        /* Do nothing. */
    } else if ((increment % numerator) > 0) {
        increment /= numerator;
        increment += 1;
    } else {
        increment /= numerator;
    }

    return increment;
}

diminuto_ticks_t diminuto_throttle_jittertolerance(diminuto_ticks_t increment, size_t maximumburstsize)
{
    diminuto_ticks_t limit = 0;

    if (maximumburstsize > 1) {
        limit = maximumburstsize - 1;
        limit *= increment;
    }

    return limit;
}

diminuto_throttle_t * diminuto_throttle_reset(diminuto_throttle_t * throttlep, diminuto_ticks_t now)
{
    throttlep->now = now;
    throttlep->then = now - throttlep->increment;
    throttlep->expected = 0;
    throttlep->actual = 0;
    throttlep->full0 = throttlep->full1 = throttlep->full2 = 0;
    throttlep->empty0 = throttlep->empty1 = throttlep->empty2 = !0;
    throttlep->alarmed1 = throttlep->alarmed2 = 0;

    return throttlep;
}

diminuto_throttle_t * diminuto_throttle_init(diminuto_throttle_t * throttlep, diminuto_ticks_t increment, diminuto_ticks_t limit, diminuto_ticks_t now)
{
    throttlep->increment = increment;
    throttlep->limit = limit;

    return diminuto_throttle_reset(throttlep, now);
}

diminuto_ticks_t diminuto_throttle_request(diminuto_throttle_t * throttlep, diminuto_ticks_t now)
{
    diminuto_ticks_t delay;
    diminuto_ticks_t elapsed;

    elapsed = (throttlep->now = now) - throttlep->then;
    if (throttlep->expected <= elapsed) {
        throttlep->actual = 0;
        throttlep->full0 = 0;
        throttlep->empty0 = !0;
        delay = 0;
    } else if ((throttlep->actual = throttlep->expected - elapsed) <= throttlep->limit) {
        throttlep->full0 = 0;
        throttlep->empty0 = 0;
        delay = 0;
    } else {
        throttlep->full0 = !0;
        throttlep->empty0 = 0;
        delay = throttlep->actual - throttlep->limit;
    }

    return delay;
}

int diminuto_throttle_commitn(diminuto_throttle_t * throttlep, size_t events)
{
    throttlep->then = throttlep->now;
    throttlep->expected = throttlep->actual + (events * throttlep->increment);
    throttlep->full2 = throttlep->full1;
    throttlep->full1 = throttlep->full0;
    throttlep->empty2 = throttlep->empty1;
    throttlep->empty1 = throttlep->empty0;
    throttlep->alarmed2 = throttlep->alarmed1;
    if (diminuto_throttle_emptied(throttlep)) {
        throttlep->alarmed1 = 0;
    } else if (diminuto_throttle_filled(throttlep)) {
        throttlep->alarmed1 = !0;
    } else {
        /* Do nothing. */
    }

    return throttlep->alarmed1;
}

void diminuto_throttle_log(diminuto_throttle_t * throttlep)
{
    if (throttlep != (diminuto_throttle_t *)0) {
        DIMINUTO_LOG_DEBUG("diminuto_throttle_t@%p[%zu]: { iat=%llu i=%llu l=%llu x=%llu x1=%llu f=%d,%d,%d e=%d,%d,%d a=%d,%d }\n", throttlep, sizeof(*throttlep), (long long unsigned int)(throttlep->now - throttlep->then), (long long unsigned int)throttlep->increment, (long long unsigned int)throttlep->limit, (long long unsigned int)throttlep->expected, (long long unsigned int)throttlep->actual, throttlep->full0, throttlep->full1, throttlep->full2, throttlep->empty0, throttlep->empty1, throttlep->empty2, throttlep->alarmed1, throttlep->alarmed2);
    } else {
        DIMINUTO_LOG_DEBUG("diminuto_throttle_t@%p[%zu]\n", throttlep, sizeof(*throttlep));
    }
}
