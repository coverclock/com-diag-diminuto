/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_SHAPER_
#define _H_COM_DIAG_DIMINUTO_SHAPER_

/**
 * @file
 *
 * Copyright 2014 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 * WORK IN PROGRESS!
 *
 * Use two throttles, a throttle being an implementation of the Generic Cell
 * Rate Algorithm (GCRA) using a Virtual Scheduler, to implement a traffic
 * shaper. The shaper can shape the emission of an event stream to a peak rate
 * with a jitter tolerance, and a sustainable rate with a maximum burst size.
 *
 * REFERENCES
 *
 * ATM Forum, Traffic Management Specification Version 4.1, af-tm-0121.000,
 * March 1999
 *
 * Chip Overclock, "Traffic Management", December 2006,
 * http://coverclock.blogspot.com/2006/12/traffic-management.html
 *
 * Chip Overclock, "Rate Control Using Throttles", January 2007,
 * http://coverclock.blogspot.com/2007/01/rate-control-and-throttles.html
 *
 * Chip Overclock, "Traffic Contracts", January 2007,
 * http://coverclock.blogspot.com/2007/01/traffic-contracts.html
 */

#include "com/diag/diminuto/diminuto_throttle.h"

typedef struct DiminutoShaper {
    diminuto_throttle_t peak;
    diminuto_throttle_t sustained;
} diminuto_shaper_t;

static inline diminuto_shaper_t * diminuto_shaper_reset(diminuto_shaper_t * shaperp, diminuto_ticks_t now)
{
    diminuto_throttle_reset(&(shaperp->peak), now);
    diminuto_throttle_reset(&(shaperp->sustained), now);
    return shaperp;
}

extern diminuto_shaper_t * diminuto_shaper_init(diminuto_shaper_t * shaperp, size_t peakrate, diminuto_ticks_t jittertolerance, size_t sustainedrate, size_t maximumburstsize, diminuto_ticks_t now);

static inline diminuto_ticks_t diminuto_shaper_now(void)
{
    return diminuto_shaper_now();
}

static inline diminuto_ticks_t diminuto_shaper_request(diminuto_shaper_t * shaperp, diminuto_ticks_t now)
{
    diminuto_ticks_t peak;
    diminuto_ticks_t sustained;

    peak = diminuto_throttle_request(&(shaperp->peak), now);
    sustained = diminuto_throttle_request(&(shaperp->sustained), now);

    return (peak > sustained) ? peak : sustained;
}

static inline int diminuto_shaper_commitn(diminuto_shaper_t * shaperp, size_t events)
{
    int peak;
    int sustained;

    peak = diminuto_throttle_commitn(&(shaperp->peak), events);
    sustained = diminuto_throttle_commitn(&(shaperp->sustained), events);

    return (peak || sustained);
}

static inline int diminuto_shaper_commit(diminuto_shaper_t * shaperp)
{
    return diminuto_shaper_commitn(shaperp, 1);
}

static inline int diminuto_shaper_admitn(diminuto_shaper_t * shaperp, diminuto_ticks_t now, size_t events)
{
    int peak;
    int sustained;

    peak = diminuto_throttle_admitn(&(shaperp->peak), now, events);
    sustained = diminuto_throttle_admitn(&(shaperp->sustained), now, events);

    return (peak || sustained);
}

static inline int diminuto_shaper_admit(diminuto_shaper_t * shaperp, diminuto_ticks_t now)
{
    return diminuto_shaper_admitn(shaperp, now, 1);
}

#endif
