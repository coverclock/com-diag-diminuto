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

#include "com/diag/diminuto/diminuto_shaper.h"
#include "com/diag/diminuto/diminuto_frequency.h"

diminuto_shaper_t * diminuto_shaper_init(diminuto_shaper_t * shaperp, size_t peakrate, diminuto_ticks_t jittertolerance, size_t sustainedrate, size_t maximumburstsize, diminuto_ticks_t now)
{
    diminuto_ticks_t peakincrement;
    diminuto_ticks_t peaklimit;
    diminuto_ticks_t sustainedincrement;
    diminuto_ticks_t sustainedlimit;

    peakincrement = (diminuto_frequency() + peakrate - 1) / peakrate;
    peaklimit = jittertolerance;

    sustainedincrement = (diminuto_frequency() + sustainedrate - 1) / sustainedrate;
    sustainedlimit = peaklimit;
    if (maximumburstsize < 1) {
        /* Do nothing. */
    } else if (sustainedincrement < peakincrement) {
        /* Do nothing. */
    } else {
    	sustainedlimit += (maximumburstsize - 1) * (sustainedincrement - peakincrement);
    }

    diminuto_throttle_init(&(shaperp->peak), peakincrement, peaklimit, now);
    diminuto_throttle_init(&(shaperp->sustained), sustainedincrement, sustainedlimit, now);

    return shaperp;
}
