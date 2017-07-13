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
 *
 * Use two throttles, a throttle being an implementation of the Generic Cell
 * Rate Algorithm (GCRA) using a Virtual Scheduler, to implement a traffic
 * shaper. The shaper can shape the emission of an event stream to a peak rate
 * with a jitter tolerance, and a sustained rate with a maximum burst size.
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

/**
 * This structure contains the state of a shaper, which consists of a throttle
 * for the peak contract, and another throttle for the sustained contract.
 */
typedef struct DiminutoShaper {
    diminuto_throttle_t peak;
    diminuto_throttle_t sustained;
} diminuto_shaper_t;

/**
 * Reset a shaper to the beginning of time such that all past sins are
 * forgotten and the traffic stream is in compliance with its contract with
 * no debt on its limit.
 * @param shaperp is a pointer to the shaper.
 * @param now is the current time from a monotonically increasing clock.
 * @return a pointer to the shaper.
 */
static inline diminuto_shaper_t * diminuto_shaper_reset(diminuto_shaper_t * shaperp, diminuto_ticks_t now)
{
    diminuto_throttle_reset(&(shaperp->peak), now);
    diminuto_throttle_reset(&(shaperp->sustained), now);
    return shaperp;
}

/**
 * Initialize a shaper.
 * @param shaperp is a pointer to the shaper.
 * @param peakrate is the peak rate in events per second.
 * @param jittertolerance is the jitter tolerance in ticks.
 * @param sustainedrate is the sustained rate in events per second.
 * @param maximumburstsize is the maximum burst size in events per burst.
 * @param now is the current time on a monotonically increasing clock.
 * @return a pointer to the shaper.
 */
extern diminuto_shaper_t * diminuto_shaper_init(diminuto_shaper_t * shaperp, size_t peakrate, diminuto_ticks_t jittertolerance, size_t sustainedrate, size_t maximumburstsize, diminuto_ticks_t now);

/**
 * Return the current time from a monotonically increasing clock.
 * @return the current time from a monotonically increasing clock in ticks.
 */
static inline diminuto_ticks_t diminuto_shaper_now(void)
{
    return diminuto_throttle_now();
}

/**
 * Ask if an event emitted now would conform to the contract. If it does, the
 * event is said to be admissable. The shaper answers this question by
 * returning the delay in ticks that would be required for the event to be
 * admissable. A zero means the event is admissable at the current time.
 * @param shaperp is a pointer to the shaper.
 * @param now is the current time on a monotonically increasing clock.
 * @return the requisite delay in ticks for the event to be admissable.
 */
static inline diminuto_ticks_t diminuto_shaper_request(diminuto_shaper_t * shaperp, diminuto_ticks_t now)
{
    diminuto_ticks_t peak;
    diminuto_ticks_t sustained;

    peak = diminuto_throttle_request(&(shaperp->peak), now);
    sustained = diminuto_throttle_request(&(shaperp->sustained), now);

    return (peak > sustained) ? peak : sustained;
}

/**
 * Tell the shaper that the previously requested event has been emitted
 * (whether it was admissable or not). The shaper updates its state. If the
 * traffic stream as exceeded its traffic contract, the state of the shaper
 * becomes alarmed. The shaper remains alarmed until the traffic stream once
 * again conforms to the contract. Although most applications emit a single
 * event, the shaper can update its state to indicate multiple events were
 * emitted. This is useful when data blocks of variable size are being emitted
 * (for example, sent to a socket) and the traffic contract is expressed in
 * bytes per second. In this case, the traffic contract expresses a mean
 * bandwidth, not an instantaneous bandwidth.
 * @param shaperp is a pointer to the shaper.
 * @param events is the number of events (nominally one).
 * @return true if the shaper is alarmed, false otherwise.
 */
static inline int diminuto_shaper_commitn(diminuto_shaper_t * shaperp, size_t events)
{
    int peak;
    int sustained;

    peak = diminuto_throttle_commitn(&(shaperp->peak), events);
    sustained = diminuto_throttle_commitn(&(shaperp->sustained), events);

    return (peak || sustained);
}

/**
 * Tell the shaper that the previously requested event has been emitted
 * (whether it was admissable or not). The shaper updates its state. If the
 * traffic stream as exceeded its traffic contract, the state of the shaper
 * becomes alarmed. The shaper remains alarmed until the traffic stream once
 * again conforms to the contract. Only a single event is emitted.
 * @param shaperp is a pointer to the shaper.
 * @return true if the shaper is alarmed, false otherwise.
 */
static inline int diminuto_shaper_commit(diminuto_shaper_t * shaperp)
{
    return diminuto_shaper_commitn(shaperp, 1);
}

/**
 * Tell the shaper that the previously requested event has been emitted after
 * delay at least as long as recommended by the request, so that it is
 * guaranteed to have been admissable even though another requested was not
 * made. This eliminates the need to make another request, as long as the
 * traffic stream is well behaved. (If it isn't, wackiness may ensue.) Multiple
 * events may be emitted.
 * @param shaperp is a pointer to the shaper.
 * @param now is the current time on a monotonically increasing clock.
 * @param events is the number of events (nominally one).
 * @return true if the shaper is alarmed, false otherwise.
 */
static inline int diminuto_shaper_admitn(diminuto_shaper_t * shaperp, diminuto_ticks_t now, size_t events)
{
    int peak;
    int sustained;

    peak = diminuto_throttle_admitn(&(shaperp->peak), now, events);
    sustained = diminuto_throttle_admitn(&(shaperp->sustained), now, events);

    return (peak || sustained);
}

/**
 * Tell the shaper that the previously requested event has been emitted after
 * delay at least as long as recommended by the request, so that it is
 * guaranteed to have been admissable even though another requested was not
 * made. This eliminates the need to make another request, as long as the
 * traffic stream is well behaved. (If it isn't, wackiness may ensue.) A single
 * event is emitted.
 * @param shaperp is a pointer to the shaper.
 * @param now is the current time on a monotonically increasing clock.
 * @return true if the shaper is alarmed, false otherwise.
 */
static inline int diminuto_shaper_admit(diminuto_shaper_t * shaperp, diminuto_ticks_t now)
{
    return diminuto_shaper_admitn(shaperp, now, 1);
}

/**
 * Log the state of a shaper.
 * @param shaperp is a pointer to the shaper.
 */
extern void diminuto_shaper_log(diminuto_shaper_t * shaperp);

#endif
