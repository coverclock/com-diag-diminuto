/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2014 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 * WORK IN PROGRESS!
 */

#include "com/diag/diminuto/diminuto_throttle.h"

diminuto_throttle_t * diminuto_throttle_reset(diminuto_throttle_t * throttlep, diminuto_ticks_t ticks)
{
    throttlep->expected = 0;
    throttlep->actual = 0;
    throttlep->then = ticks - throttlep->increment;
    throttlep->now = ticks;
    throttlep->alarmed = 0;
    throttlep->alarming = 0;
    return throttlep;
}

diminuto_throttle_t * diminuto_throttle_init(diminuto_throttle_t * throttlep, diminuto_ticks_t increment, diminuto_ticks_t limit)
{
    throttlep->increment = increment;
    throttlep->limit = limit;
    return diminuto_throttle_reset(throttlep);
}

diminuto_ticks_t diminuto_throttle_request(diminuto_throttle_t * throttlep, diminuto_ticks_t now)
{
    diminuto_ticks_t delay = 0;
    diminuto_ticks_t elapsed;

    elapsed = (throttlep->now = now) - throttlep->then;
    if (throttlep->expected <= elapsed) {
        throttlep->actual = 0;
        throttlep->alarming = 0;
    } else if ((throttlep->actual = throttlep->expected - elapsed) > throttlep->limit) {
        delay = throttlep->actual - throttlep->limit;
        throttlep->alarming = !0;
    } else {
        throttlep->alarming = 0;
    }

    return delay;
}

int diminuto_throttle_commit(diminuto_throttle_t * throttlep)
{
    throttlep->then = throttlep->now;
    throttlep->expected = throttlep->actual + throttlep->increment;
    throttlep->alarmed = throttlep->alarming;
    return throttlep->alarmed;
}
