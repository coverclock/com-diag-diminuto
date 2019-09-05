/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2014 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in LICENSE.txt<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 */

#include "com/diag/diminuto/diminuto_shaper.h"
#include "com/diag/diminuto/diminuto_frequency.h"
#include "com/diag/diminuto/diminuto_log.h"

diminuto_ticks_t diminuto_shaper_bursttolerance(diminuto_ticks_t peakincrement, diminuto_ticks_t jittertolerance, diminuto_ticks_t sustainedincrement, size_t maximumburstsize)
{
    diminuto_ticks_t limit = 0;
    diminuto_ticks_t increment;

    if (maximumburstsize <= 1) {
        /* Do nothing. */
    } else if (sustainedincrement <= peakincrement) {
        /* Do nothing. */
    } else {
        limit = maximumburstsize - 1;
        increment = sustainedincrement;
        increment -= peakincrement;
        limit *= increment;
    }
    limit += jittertolerance;

    return limit;
}

diminuto_shaper_t * diminuto_shaper_init(diminuto_shaper_t * shaperp, diminuto_ticks_t peakincrement, diminuto_ticks_t jittertolerance, diminuto_ticks_t sustainedincrement, diminuto_ticks_t bursttolerance, diminuto_ticks_t now)
{
    diminuto_throttle_init(&(shaperp->peak), peakincrement, jittertolerance, now);
    diminuto_throttle_init(&(shaperp->sustained), sustainedincrement, bursttolerance, now);

    return shaperp;
}

diminuto_shaper_t * diminuto_shaper_reset(diminuto_shaper_t * shaperp, diminuto_ticks_t now)
{
    diminuto_throttle_reset(&(shaperp->peak), now);
    diminuto_throttle_reset(&(shaperp->sustained), now);

    return shaperp;
}

diminuto_ticks_t diminuto_shaper_request(diminuto_shaper_t * shaperp, diminuto_ticks_t now)
{
    diminuto_ticks_t peak;
    diminuto_ticks_t sustained;

    peak = diminuto_throttle_request(&(shaperp->peak), now);
    sustained = diminuto_throttle_request(&(shaperp->sustained), now);

    return (peak > sustained) ? peak : sustained;
}

int diminuto_shaper_commitn(diminuto_shaper_t * shaperp, size_t events)
{
    int peak;
    int sustained;

    peak = diminuto_throttle_commitn(&(shaperp->peak), events);
    sustained = diminuto_throttle_commitn(&(shaperp->sustained), events);

    return (peak || sustained);
}

int diminuto_shaper_admitn(diminuto_shaper_t * shaperp, diminuto_ticks_t now, size_t events)
{
    int peak;
    int sustained;

    peak = diminuto_throttle_admitn(&(shaperp->peak), now, events);
    sustained = diminuto_throttle_admitn(&(shaperp->sustained), now, events);

    return (peak || sustained);
}

diminuto_ticks_t diminuto_shaper_getexpected(diminuto_shaper_t * shaperp)
{
    diminuto_ticks_t peak;
    diminuto_ticks_t sustained;

    peak = diminuto_throttle_getexpected(&(shaperp->peak));
    sustained = diminuto_throttle_getexpected(&(shaperp->sustained));

    return (peak > sustained) ? peak : sustained;
}

int diminuto_shaper_isempty(diminuto_shaper_t * shaperp)
{
    int peak;
    int sustained;

    peak = diminuto_throttle_isempty(&(shaperp->peak));
    sustained = diminuto_throttle_isempty(&(shaperp->sustained));

    return peak && sustained;
}

int diminuto_shaper_isfull(diminuto_shaper_t * shaperp)
{
    int peak;
    int sustained;

    peak = diminuto_throttle_isfull(&(shaperp->peak));
    sustained = diminuto_throttle_isfull(&(shaperp->sustained));

    return peak || sustained;
}

int diminuto_shaper_isalarmed(diminuto_shaper_t * shaperp)
{
    int peak;
    int sustained;

    peak = diminuto_throttle_isalarmed(&(shaperp->peak));
    sustained = diminuto_throttle_isalarmed(&(shaperp->sustained));

    return peak || sustained;
}

int diminuto_shaper_emptied(diminuto_shaper_t * shaperp)
{
    int peak;
    int sustained;

    peak = diminuto_throttle_emptied(&(shaperp->peak));
    sustained = diminuto_throttle_emptied(&(shaperp->sustained));

    return peak || sustained;
}

int diminuto_shaper_filled(diminuto_shaper_t * shaperp)
{
    int peak;
    int sustained;

    peak = diminuto_throttle_filled(&(shaperp->peak));
    sustained = diminuto_throttle_filled(&(shaperp->sustained));

    return peak || sustained;
}

int diminuto_shaper_alarmed(diminuto_shaper_t * shaperp)
{
    int peak;
    int sustained;

    peak = diminuto_throttle_alarmed(&(shaperp->peak));
    sustained = diminuto_throttle_alarmed(&(shaperp->sustained));

    return peak || sustained;
}

int diminuto_shaper_cleared(diminuto_shaper_t * shaperp)
{
    int peak;
    int sustained;

    peak = diminuto_throttle_cleared(&(shaperp->peak));
    sustained = diminuto_throttle_cleared(&(shaperp->sustained));

    return peak || sustained;
}

void diminuto_shaper_log(diminuto_shaper_t * shaperp)
{
    if (shaperp != (diminuto_shaper_t *)0) {
        DIMINUTO_LOG_DEBUG("diminuto_shaper_t@%p[%zu]: peak:\n", shaperp, sizeof(*shaperp)); diminuto_throttle_log(&(shaperp->peak));
        DIMINUTO_LOG_DEBUG("diminuto_shaper_t@%p[%zu]: sustained:\n", shaperp, sizeof(*shaperp)); diminuto_throttle_log(&(shaperp->sustained));
    } else {
        DIMINUTO_LOG_DEBUG("diminuto_shaper_t@%p[%zu]\n", shaperp, sizeof(*shaperp));
    }
}
