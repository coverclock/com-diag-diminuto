/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2014-2015 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 */

#include "com/diag/diminuto/diminuto_throttle.h"
#include "com/diag/diminuto/diminuto_log.h"

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

    return throttlep->full1;
}

void diminuto_throttle_log(diminuto_throttle_t * throttlep)
{
    if (throttlep != (diminuto_throttle_t *)0) {
        DIMINUTO_LOG_DEBUG("diminuto_throttle_t@%p[%zu]: { iat=%lld i=%lld l=%lld x=%lld x1=%lld f=%d,%d,%d e=%d,%d,%d a=%d,%d }\n", throttlep, sizeof(*throttlep), throttlep->now - throttlep->then, throttlep->increment, throttlep->limit, throttlep->expected, throttlep->actual, throttlep->full0, throttlep->full1, throttlep->full2, throttlep->empty0, throttlep->empty1, throttlep->empty2, throttlep->alarmed1, throttlep->alarmed2);
    } else {
        DIMINUTO_LOG_DEBUG("diminuto_throttle_t@%p[%zu]\n", throttlep, sizeof(*throttlep));
    }
}
