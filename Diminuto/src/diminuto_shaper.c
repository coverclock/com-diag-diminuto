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
#include "com/diag/diminuto/diminuto_time.h"

diminuto_shaper_t * diminuto_shaper_init(diminuto_shaper_t * shaperp, size_t peakrate, diminuto_ticks_t jittertolerance, size_t sustainablerate, size_t maximumburstsize, diminuto_ticks_t now)
{
    diminuto_ticks_t frequency;
    diminuto_ticks_t peakincrement;
    diminuto_ticks_t peaklimit;
    diminuto_ticks_t sustainableincrement;
    diminuto_ticks_t sustainablelimit;

    frequency = diminuto_time_frequency();

    peakincrement = (frequency + peakrate - 1) / peakrate;
    peaklimit = jittertolerance;

    sustainableincrement = (frequency + sustainablerate - 1) / sustainablerate;
    sustainablelimit = peaklimit;
    if (maximumburstsize <= 1) {
        /* Do nothing. */
    } else if (sustainableincrement <= peakincrement) {
        /* Do nothing. */
    } else {
        sustainablelimit += (maximumburstsize - 1) * (sustainableincrement - peakincrement);
    }

    diminuto_throttle_init(&(shaperp->peak), peakincrement, peaklimit, now);
    diminuto_throttle_init(&(shaperp->sustained), sustainableincrement, sustainablelimit, now);

    return shaperp;
}
