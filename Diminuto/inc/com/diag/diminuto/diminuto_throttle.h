/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_THROTTLE_
#define _H_COM_DIAG_DIMINUTO_THROTTLE_

/**
 * @file
 * @copyright Copyright 2014-2022 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief Implements a GCRA using a virtual scheduler.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 *
 * The Throttle feature implements a Generic Cell Rate Algorithm (GCRA) using
 * a Virtual Scheduler. This can in turn be used to implement a variety of
 * traffic shaping and rate control algorithms. The VS works by monitoring the
 * inter-arrival interval of events and comparing that interval to the expected
 * value. When the cumulative error in the inter-arrival interval exceeds a
 * threshold, the throttle becomes "alarmed" and the traffic stream is in
 * violation of its contract. In the original TM spec, an event was the
 * emission (if traffic shaping) or arrival (if traffic policing) of an ATM
 * cell, but it could be data blocks, error reports, or any other kind of
 * real-time activity. In this implementation, it can even be variable length
 * data blocks, in which the traffic contract describes the mean bandwidth of
 * the traffic stream, not the instantaneous bandwidth as with ATM. In the
 * original TM spec, the variable "i" was the increment or contracted
 * inter-arrival interval, "l" was the limit or threshold, "x" was the
 * expected inter-arrival interval for the next event, and "x1" was the
 * actual inter-arrival interval of that event. A throttle can be used to
 * smooth out low frequency events over a long duration, or to implement a
 * leaky bucket algorithm.
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

/*******************************************************************************
 * TYPES
 ******************************************************************************/

/**
 * This structure contains the state of a throttle.
 */
typedef struct DiminutoThrottle {
    diminuto_ticks_t now;           /**< Time of the most recent event. */
    diminuto_ticks_t then;          /**< Time of the previous event. */
    diminuto_ticks_t increment;     /**< GCRA increment a.k.a. i. */
    diminuto_ticks_t limit;         /**< GCRA limit a.k.a. l. */
    diminuto_ticks_t expected;      /**< GCRA expected IAT a.k.a. x. */
    diminuto_ticks_t actual;        /**< GCRA actual IAT a.k.a x1. */
    unsigned int full0 : 1;         /**< The leaky bucket will fill. */
    unsigned int full1 : 1;         /**< The leaky bucket is filling. */
    unsigned int full2 : 1;         /**< The leaky bucket was filled. */
    unsigned int empty0 : 1;        /**< The leaky bucket will empty. */
    unsigned int empty1 : 1;        /**< The leaky bucket is emptying. */
    unsigned int empty2 : 1;        /**< The leaky bucket was emptied. */
    unsigned int alarmed1 : 1;      /**< The throttle is alarmed. */
    unsigned int alarmed2 : 1;      /**< The throttle was alarmed. */
} diminuto_throttle_t;

/*******************************************************************************
 * INITIALIZATION
 ******************************************************************************/

/**
 * @def COM_DIAG_DIMINUTO_THROTTLE_FREQUENCY
 * This manifest constant is the frequency in Hertz at which this feature
 * operates. The inverse of this value is the smallest unit of time in fractions
 * of a second that this feature can express or use. This constant is provided
 * for use in those cases where it is useful to have the value at compile time.
 * However, you chould always prefer to use the inline function when possible.
 */
#define COM_DIAG_DIMINUTO_THROTTLE_FREQUENCY (COM_DIAG_DIMINUTO_FREQUENCY)

/**
 * Return the resolution of the Diminuto throttle time units in ticks per second
 * (Hertz). Time intervals smaller than the equivalent period in ticks will
 * not yield the expected results.
 * @return the resolution in ticks per second.
 */
static inline diminuto_sticks_t diminuto_throttle_frequency(void)
{
    return diminuto_time_frequency();
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
 * Reset a throttle to the beginning of time such that all past sins are
 * forgotten and the traffic stream is in compliance with its contract with
 * no debt on its limit. This is especially useful for cases where no event has
 * occurred in longer than the accumulated debt such that any future event will
 * be admissable (because otherwise the throttle will not change state from
 * either alarmed or not cleared unless an event arrives).
 * @param throttlep is a pointer to the throttle.
 * @param now is the current time from a monotonically increasing clock.
 * @return a pointer to the throttle.
 */
extern diminuto_throttle_t * diminuto_throttle_reset(diminuto_throttle_t * throttlep, diminuto_ticks_t now);

/**
 * Initialize a throttle.
 * @param throttlep is a pointer to the throttle.
 * @param increment is the contracted inter-arrival interval in ticks.
 * @param limit is the contracted threshold in ticks.
 * @param now is the current time on a monotonically increasing clock.
 * @return a pointer to the throttle.
 */
extern diminuto_throttle_t * diminuto_throttle_init(diminuto_throttle_t * throttlep, diminuto_ticks_t increment, diminuto_ticks_t limit, diminuto_ticks_t now);

/**
 * Release any resources held by the throttle object.
 * @param throttlep points to the object.
 * @return NULL if successful, a pointer to the object otherwise.
 */
static inline diminuto_throttle_t * diminuto_throttle_fini(diminuto_throttle_t * throttlep) {
    return (diminuto_throttle_t *)0;
}

/*******************************************************************************
 * STATE CHANGE
 ******************************************************************************/

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
 * @return true if the leaky bucket is full, false otherwise.
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
 * Combine a request with a commit. No matter what the request returns, no delay
 * is performed. So if the event(s) are not admissable, wackiness may ensue.
 * This function can be used after the caller has delayed following a request.
 * @param throttlep is a pointer to the throttle.
 * @param now is the current time on a monotonically increasing clock.
 * @param events is the number of events (nominally one).
 * @return true if the throttle is alarmed, false otherwise.
 */
static inline int diminuto_throttle_admitn(diminuto_throttle_t * throttlep, diminuto_ticks_t now, size_t events)
{
    diminuto_throttle_request(throttlep, now);

    return diminuto_throttle_commitn(throttlep, events);
}

/**
 * Combine a request with a commit. No matter what the request returns, no delay
 * is performed. So if the event is not admissable, wackiness may ensue.
 * This function can be used after the caller has delayed following a request.
 * @param throttlep is a pointer to the throttle.
 * @param now is the current time on a monotonically increasing clock.
 * @return true if the throttle is alarmed, false otherwise.
 */
static inline int diminuto_throttle_admit(diminuto_throttle_t * throttlep, diminuto_ticks_t now)
{
    return diminuto_throttle_admitn(throttlep, now, 1);
}

/**
 * Combine a request with a commit of zero events to indicate that time has
 * passed without an event occurring, updating the state of the throttle.
 * @param throttlep is a pointer to the throttle.
 * @param now is the current time on a monotonically increasing clock.
 * @return true if the throttle is alarmed, false otherwise.
 */
static inline int diminuto_throttle_update(diminuto_throttle_t * throttlep, diminuto_ticks_t now)
{
    return diminuto_throttle_admitn(throttlep, now, 0);
}

/*******************************************************************************
 * STABLE STATE
 ******************************************************************************/

/**
 * Returns the delay in ticks that would be required for the event to be
 * admissable without relying on the limit. This is simply the expected
 * interarrival time of the next event, and is extracted directly from the
 * corresponding throttle field without any state change in the throttle.
 * @param throttlep is a pointer to the throttle.
 * @return the requisite delay in ticks for the event to be expected.
 */
static inline diminuto_ticks_t diminuto_throttle_getexpected(diminuto_throttle_t * throttlep)
{
    return throttlep->expected;
}

/**
 * Returns true if the leaky bucket is empty.
 * @param throttlep is a pointer to the throttle.
 * @return true if the throttle is clear.
 */
static inline int diminuto_throttle_isempty(diminuto_throttle_t * throttlep)
{
    return (throttlep->empty1 != 0);
}

/**
 * Returns true if the leaky bucket is full.
 * @param throttlep is a pointer to the throttle.
 * @return true if the leaky bucket is full.
 */
static inline int diminuto_throttle_isfull(diminuto_throttle_t * throttlep)
{
    return (throttlep->full1 != 0);
}

/**
 * Returns true if the throttle is alarmed.
 * @param throttlep is a pointer to the throttle.
 * @return true if the throttle is alarmed.
 */
static inline int diminuto_throttle_isalarmed(diminuto_throttle_t * throttlep)
{
    return (throttlep->alarmed1 != 0);
}

/*******************************************************************************
 * TRANSITION STATE
 ******************************************************************************/

/**
 * Returns true if the leaky bucket just filled.
 * @param throttlep is a pointer to the throttle.
 * @return true if the leaky bucket just filled.
 */
static inline int diminuto_throttle_emptied(diminuto_throttle_t * throttlep)
{
    return (throttlep->empty1 != 0) && (throttlep->empty2 == 0);
}

/**
 * Returns true if the leaky bucket just filled.
 * @param throttlep is a pointer to the throttle.
 * @return true if the leaky bucket just filled.
 */
static inline int diminuto_throttle_filled(diminuto_throttle_t * throttlep)
{
    return (throttlep->full1 != 0) && (throttlep->full2 == 0);
}

/**
 * Returns true if the throttle just alarmed.
 * @param throttlep is a pointer to the throttle.
 * @return true if the throttle just alarmed.
 */
static inline int diminuto_throttle_alarmed(diminuto_throttle_t * throttlep)
{
    return (throttlep->alarmed1 != 0) && (throttlep->alarmed2 == 0);
}

/**
 * Returns true if the throttle just cleared.
 * @param throttlep is a pointer to the throttle.
 * @return true if the throttle just cleared.
 */
static inline int diminuto_throttle_cleared(diminuto_throttle_t * throttlep)
{
    return (throttlep->alarmed1 == 0) && (throttlep->alarmed2 != 0);
}

/*******************************************************************************
 * ANCILLARY
 ******************************************************************************/

/**
 * Compute an interarrival time in ticks given the rate as a numerator and a
 * denominator. The use of a fraction allows rates of less than one event per
 * second, or fractional rates, to be specified.
 * @param numerator is the numerator portion of the rate.
 * @param denominator is the denominator portion of the rate.
 * @param frequency is the value returned by diminuto_frequency().
 * @return the interarrival time in ticks.
 */
extern diminuto_ticks_t diminuto_throttle_interarrivaltime(size_t numerator, size_t denominator, diminuto_ticks_t frequency);

/**
 * Compute a jitter tolerance in ticks given the interarrival time (increment),
 * and the largest packet size (maximum burst size).
 * @param increment is the interarrival time in ticks corresponding to the rate.
 * @param maximumburstsize is the largest packet size in events (e.g. bytes).
 * @return the jitter tolerance in ticks.
 */
extern diminuto_ticks_t diminuto_throttle_jittertolerance(diminuto_ticks_t increment, size_t maximumburstsize);

/**
 * Log the state of a throttle.
 * @param throttlep is a pointer to the throttle.
 */
extern void diminuto_throttle_log(diminuto_throttle_t * throttlep);

#endif
