/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2014 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
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
        throttlep->alarming = 0;
        throttlep->clearing = !0;
        delay = 0;
    } else if ((throttlep->actual = throttlep->expected - elapsed) <= throttlep->limit) {
        throttlep->alarming = 0;
        throttlep->clearing = 0;
        delay = 0;
    } else {
        throttlep->alarming = !0;
        throttlep->clearing = 0;
        delay = throttlep->actual - throttlep->limit;
    }

    return delay;
}

int diminuto_throttle_commitn(diminuto_throttle_t * throttlep, size_t events)
{
    throttlep->then = throttlep->now;
    throttlep->expected = throttlep->actual + (events * throttlep->increment);
    throttlep->alarmed = throttlep->alarming;
    throttlep->cleared = throttlep->clearing;
    return throttlep->alarmed;
}

void diminuto_throttle_log(diminuto_throttle_t * throttlep)
{
    if (throttlep != (diminuto_throttle_t *)0) {
        DIMINUTO_LOG_DEBUG("diminuto_throttle_t@%p[%zu]: { (now-then)=%lld increment=%lld limit=%lld expected=%lld actual=%lld alarmed=%d alarming=%d cleared=%d clearing=%d}\n", throttlep, sizeof(*throttlep), throttlep->now - throttlep->then, throttlep->increment, throttlep->limit, throttlep->expected, throttlep->actual, throttlep->alarmed, throttlep->alarming, throttlep->cleared, throttlep->clearing);
    } else {
        DIMINUTO_LOG_DEBUG("diminuto_throttle_t@%p[%zu]\n", throttlep, sizeof(*throttlep));
    }
}
