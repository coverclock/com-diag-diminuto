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
    diminuto_ticks_t sustainedlimit;

    sustainedlimit = jittertolerance;
    if (maximumburstsize <= 1) {
        /* Do nothing. */
    } else if (sustainedincrement <= peakincrement) {
        /* Do nothing. */
    } else {
    	sustainedlimit += (maximumburstsize - 1) * (sustainedincrement - peakincrement);
    }

    return sustainedlimit;
}

diminuto_shaper_t * diminuto_shaper_init(diminuto_shaper_t * shaperp, diminuto_ticks_t peakincrement, diminuto_ticks_t jittertolerance, diminuto_ticks_t sustainedincrement, diminuto_ticks_t bursttolerance, diminuto_ticks_t now)
{
    diminuto_throttle_init(&(shaperp->peak), peakincrement, jittertolerance, now);
    diminuto_throttle_init(&(shaperp->sustained), sustainedincrement, bursttolerance, now);

    return shaperp;
}

void diminuto_shaper_log(diminuto_shaper_t * shaperp)
{
    if (shaperp != (diminuto_shaper_t *)0) {
        DIMINUTO_LOG_DEBUG("diminuto_shaper_t@%p[%zu]:\n", shaperp, sizeof(*shaperp));
        diminuto_throttle_log(&(shaperp->peak));
        diminuto_throttle_log(&(shaperp->sustained));
    } else {
        DIMINUTO_LOG_DEBUG("diminuto_shaper_t@%p[%zu]\n", shaperp, sizeof(*shaperp));
    }
}
