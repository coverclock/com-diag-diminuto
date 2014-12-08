/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_THROTTLE_
#define _H_COM_DIAG_DIMINUTO_THROTTLE_

/**
 * @file
 *
 * Copyright 2014 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 *
 * Implement a Generic Cell Rate Algorithm (GCRA) using a Virtual Scheduler.
 * This can in turn be used to implement a variety of traffic shaping and rate
 * control algorithms. The VS works by monitoring the inter-arrival interval of
 * events and comparing that interval to the expected value. When the cumulative
 * error in the inter-arrival interval exceeds a threshold, the throttle becomes
 * "alarmed" and the traffic stream is in violation of its contract. In the
 * original TM spec, an event was the emission (if traffic shaping) or arrival
 * (if traffic policing) of an ATM cell, but it could be data blocks, error
 * reports, or any other kind of real-time activity. In this implementation,
 * it can even be variable length data blocks, in which the traffic contract
 * describes the mean bandwidth of the traffic stream, not the instantaneous
 * bandwidth as with ATM. In the original TM spec, the variable "i" was the
 * increment or contracted inter-arrival interval, "l" was the limit or
 * threshold, "x" was the expected inter-arrival interval for the next event,
 * and "x1" was the actual inter-arrival interval of that event.
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

#include "com/diag/diminuto/diminuto_types.h"
#include "com/diag/diminuto/diminuto_time.h"

/**
 * This structure contains the state of a throttle.
 */
typedef struct DiminutoThrottle {
    diminuto_ticks_t now;
    diminuto_ticks_t then;
    diminuto_ticks_t increment;     /* GCRA i */
    diminuto_ticks_t limit;         /* GCRA l */
    diminuto_ticks_t expected;      /* GCRA x */
    diminuto_ticks_t actual;        /* GCRA x1 */
    unsigned int alarmed : 1;
    unsigned int alarming: 1;
} diminuto_throttle_t;

/**
 * Reset a throttle to the beginning of time such that all past sins are
 * forgotten and the traffic stream is in compliance with its contract with
 * no debt on its limit.
 * @param throttlep is a pointer to the throttle.
 * @param now is the current time from a monotonically increasing clock.
 * @return a pointer to the throttle.
 */
static inline diminuto_throttle_t * diminuto_throttle_reset(diminuto_throttle_t * throttlep, diminuto_ticks_t now)
{
    throttlep->now = now;
    throttlep->then = now - throttlep->increment;
    throttlep->expected = throttlep->increment;
    throttlep->actual = 0;
    throttlep->alarmed = 0;
    throttlep->alarming = 0;
    return throttlep;
}

/**
 * Initialize a throttle.
 * @param throttlep is a pointer to the throttle.
 * @param increment is the contracted inter-arrival interval in ticks.
 * @param limit is the contracted threshold in ticks.
 * @param now is the current time on a monotonically increasing clock.
 * @return a pointer to the throttle.
 */
static inline diminuto_throttle_t * diminuto_throttle_init(diminuto_throttle_t * throttlep, diminuto_ticks_t increment, diminuto_ticks_t limit, diminuto_ticks_t now)
{
    throttlep->increment = increment;
    throttlep->limit = limit;
    return diminuto_throttle_reset(throttlep, now);
}

/**
 * Return the current time from a monotonically increasing clock.
 * @return the current time from a monotonically increasing clock in ticks.
 */
static inline diminuto_ticks_t diminuto_throttle_now(void)
{
    return diminuto_time_elapsed();
}

/**
 * Ask if an event emitted now would conform to the contract. If it does, the
 * event is said to be admissable. The throttle answers this question by
 * returning the delay in ticks that would be required for the event to be
 * admissable. A zero means the event is admissable at the current time.
 * @param throttlep is a pointer to the throttle.
 * @param now is the current time on a monotonically increasing clock.
 * @return the requisite delay in ticks for the event to be admissable.
 */
extern diminuto_ticks_t diminuto_throttle_request(diminuto_throttle_t * throttlep, diminuto_ticks_t now);

/**
 * Tell the throttle that the previously requested event has been emitted
 * (whether it was admissable or not). The throttle updates its state. If the
 * traffic stream as exceeded its traffic contract, the state of the throttle
 * becomes alarmed. The throttle remains alarmed until the traffic stream once
 * again conforms to the contract. Although most applications emit a single
 * event, the throttle can update its state to indicate multiple events were
 * emitted. This is useful when data blocks of variable size are being emitted
 * (for example, sent to a socket) and the traffic contract is expressed in
 * bytes per second. In this case, the traffic contract expresses a mean
 * bandwidth, not an instantaneous bandwidth.
 * @param throttlep is a pointer to the throttle.
 * @param events is the number of events (nominally one).
 * @return true if the throttle is alarmed, false otherwise.
 */
extern int diminuto_throttle_commitn(diminuto_throttle_t * throttlep, size_t events);

/**
 * Tell the throttle that the previously requested event has been emitted
 * (whether it was admissable or not). The throttle updates its state. If the
 * traffic stream as exceeded its traffic contract, the state of the throttle
 * becomes alarmed. The throttle remains alarmed until the traffic stream once
 * again conforms to the contract. Only a single event is emitted.
 * @param throttlep is a pointer to the throttle.
 * @return true if the throttle is alarmed, false otherwise.
 */
static inline int diminuto_throttle_commit(diminuto_throttle_t * throttlep)
{
    return diminuto_throttle_commitn(throttlep, 1);
}

/**
 * Tell the throttle that the previously requested event has been emitted after
 * delay at least as long as recommended by the request, so that it is
 * guaranteed to have been admissable even though another requested was not
 * made. This eliminates the need to make another request, as long as the
 * traffic stream is well behaved. (If it isn't, wackiness may ensue.) Multiple
 * events may be emitted.
 * @param throttlep is a pointer to the throttle.
 * @param now is the current time on a monotonically increasing clock.
 * @param events is the number of events (nominally one).
 * @return true if the throttle is alarmed, false otherwise.
 */
int diminuto_throttle_admitn(diminuto_throttle_t * throttlep, diminuto_ticks_t now, size_t events);

/**
 * Tell the throttle that the previously requested event has been emitted after
 * delay at least as long as recommended by the request, so that it is
 * guaranteed to have been admissable even though another requested was not
 * made. This eliminates the need to make another request, as long as the
 * traffic stream is well behaved. (If it isn't, wackiness may ensue.) A single
 * event is emitted.
 * @param throttlep is a pointer to the throttle.
 * @param now is the current time on a monotonically increasing clock.
 * @return true if the throttle is alarmed, false otherwise.
 */
static inline int diminuto_throttle_admit(diminuto_throttle_t * throttlep, diminuto_ticks_t now)
{
    return diminuto_throttle_admitn(throttlep, now, 1);
}

/**
 * Log the state of a throttle.
 * @param throttlep is a pointer to the throttle.
 */
extern void diminuto_throttle_log(diminuto_throttle_t * throttlep);

#endif
