/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_SHAPER_
#define _H_COM_DIAG_DIMINUTO_SHAPER_

/**
 * @file
 * @copyright Copyright 2014-2022 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief Implements a traffic Shaper using a peak and sustained rate throttle.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 *
 * The Shaper feature uses two instances of the Throttle feature, a throttle
 * being an implementation of the Generic Cell Rate Algorithm (GCRA) using a
 * Virtual Scheduler, to implement a traffic shaper. The shaper can shape the
 * emission of an event stream to a peak rate with a jitter tolerance, and
 * a sustained rate with a maximum burst size.
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
    diminuto_throttle_t peak;           /**< The peak throttle. */
    diminuto_throttle_t sustained;      /**< The sustainable throttle. */
} diminuto_shaper_t;

/*******************************************************************************
 * TIME
 ******************************************************************************/

/**
 * @def COM_DIAG_DIMINUTO_SHAPER_FREQUENCY
 * This manifest constant is the frequency in Hertz at which this feature
 * operates. The inverse of this value is the smallest unit of time in fractions
 * of a second that this feature can express or use. This constant is provided
 * for use in those cases where it is useful to have the value at compile time.
 * However, you chould always prefer to use the inline function when possible.
 */
#define COM_DIAG_DIMINUTO_SHAPER_FREQUENCY (COM_DIAG_DIMINUTO_THROTTLE_FREQUENCY)

/**
 * Return the resolution of the Diminuto shaper time units in ticks per second
 * (Hertz). Time intervals smaller than the equivalent period in ticks will
 * not yield the expected results.
 * @return the resolution in ticks per second or <0 if an error occurred.
 */
static inline diminuto_sticks_t diminuto_shaper_frequency(void)
{
    return diminuto_throttle_frequency();
}

/**
 * Return the current time from a monotonically increasing clock.
 * @return the current time from a monotonically increasing clock in ticks or <0 if an error occurred.
 */
static inline diminuto_sticks_t diminuto_shaper_now(void)
{
    return diminuto_throttle_now();
}

/*******************************************************************************
 * RESET
 ******************************************************************************/

/**
 * Reset a shaper to the beginning of time such that all past sins are
 * forgotten and the traffic stream is in compliance with its contract with
 * no debt on its limit.
 * @param shaperp is a pointer to the shaper.
 * @param now is the current time from a monotonically increasing clock.
 * @return a pointer to the shaper or NULL if an error occurred.
 */
extern diminuto_shaper_t * diminuto_shaper_reset(diminuto_shaper_t * shaperp, diminuto_ticks_t now);

/*******************************************************************************
 * INITIALIZATION
 ******************************************************************************/

/**
 * Initialize a shaper.
 * @param shaperp is a pointer to the shaper.
 * @param peakincrement is the minimum interarrival time in ticks.
 * @param jittertolerance is the jitter tolerance in ticks for the peak rate.
 * @param sustainedincrement is the average interarrival time in ticks.
 * @param bursttolerance is the burst tolerance in ticks for the sustained rate.
 * @param now is the current time on a monotonically increasing clock.
 * @return a pointer to the shaper or NULL if an error occurred.
 */
extern diminuto_shaper_t * diminuto_shaper_init(diminuto_shaper_t * shaperp, diminuto_ticks_t peakincrement, diminuto_ticks_t jittertolerance, diminuto_ticks_t sustainedincrement, diminuto_ticks_t bursttolerance, diminuto_ticks_t now);

/**
 * Release any resources held by the shaper object.
 * @param shaperp points to the object.
 * @return NULL or a pointer to the shaper if an error occurred.
 */
static inline diminuto_shaper_t * diminuto_shaper_fini(diminuto_shaper_t * shaperp) {
    return (diminuto_shaper_t *)0;
}

/*******************************************************************************
 * STATE CHANGE
 ******************************************************************************/

/**
 * Ask if an event emitted now would conform to the contract. If it does, the
 * event is said to be admissable. The shaper answers this question by
 * returning the delay in ticks that would be required for the event to be
 * admissable. A zero means the event is admissable at the current time.
 * @param shaperp is a pointer to the shaper.
 * @param now is the current time on a monotonically increasing clock.
 * @return the requisite delay in ticks for the event to be admissable.
 */
extern diminuto_ticks_t diminuto_shaper_request(diminuto_shaper_t * shaperp, diminuto_ticks_t now);

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
extern int diminuto_shaper_commitn(diminuto_shaper_t * shaperp, size_t events);

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
extern int diminuto_shaper_admitn(diminuto_shaper_t * shaperp, diminuto_ticks_t now, size_t events);

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
 * Combine a request with a commit of zero events to indicate that time has
 * passed without an event occurring, updating the state of the shaper.
 * @param shaperp is a pointer to the shaper.
 * @param now is the current time on a monotonically increasing clock.
 * @return true if the shaper is alarmed, false otherwise.
 */
static inline int diminuto_shaper_update(diminuto_shaper_t * shaperp, diminuto_ticks_t now)
{
    return diminuto_shaper_admitn(shaperp, now, 0);
}

/*******************************************************************************
 * STABLE STATE
 ******************************************************************************/

/**
 * Returns the delay in ticks that would be required for the event to be
 * admissable without relying on the limit. This is simply the expected
 * interarrival time of the next event, and is extracted directly from the
 * corresponding throttle field without any state change in the throttle.
 * @param shaperp is a pointer to the throttle.
 * @return the requisite delay in ticks for the event to be expected.
 */
extern diminuto_ticks_t diminuto_shaper_getexpected(const diminuto_shaper_t * shaperp);

/**
 * Returns true if the leaky bucket is empty.
 * @param shaperp is a pointer to the throttle.
 * @return true if the leaky bucket is empty.
 */
extern int diminuto_shaper_isempty(const diminuto_shaper_t * shaperp);

/**
 * Returns true if the leaky bucket is full.
 * @param shaperp is a pointer to the throttle.
 * @return true if the leaky bucket is full.
 */
extern int diminuto_shaper_isfull(const diminuto_shaper_t * shaperp);

/**
 * Returns true if the throttle is alarmed.
 * @param shaperp is a pointer to the throttle.
 * @return true if the throttle is alarmed.
 */
extern int diminuto_shaper_isalarmed(const diminuto_shaper_t * shaperp);

/*******************************************************************************
 * TRANSITION STATE
 ******************************************************************************/

/*
 * These functions aren't very useful with a shaper since it consists of
 * two throttles (GCRAs) run in parallel. It is unlikely that *both* throttles
 * would transition states at the same time, so these are coded as ORs instead
 * of ANDs.
 */

/**
 * Returns true if the leaky bucket just filled.
 * @param shaperp is a pointer to the throttle.
 * @return true if the leaky bucket just filled.
 */
extern int diminuto_shaper_emptied(const diminuto_shaper_t * shaperp);

/**
 * Returns true if the leaky bucket just filled.
 * @param shaperp is a pointer to the throttle.
 * @return true if the leaky bucket just filled.
 */
extern int diminuto_shaper_filled(const diminuto_shaper_t * shaperp);

/**
 * Returns true if the throttle just alarmed.
 * @param shaperp is a pointer to the throttle.
 * @return true if the throttle just alarmed.
 */
extern int diminuto_shaper_alarmed(const diminuto_shaper_t * shaperp);

/**
 * Returns true if the throttle just cleared.
 * @param shaperp is a pointer to the throttle.
 * @return true if the throttle just cleared.
 */
extern int diminuto_shaper_cleared(const diminuto_shaper_t * shaperp);

/*******************************************************************************
 * ANCILLARY
 ******************************************************************************/

/**
 * Compute the burst tolerance given the peak increment (minimum interarrival
 * time) in ticks, the jitter tolerance in ticks, the sustained increment
 * (mean interarrival time), and the maximum burst size (maximum packet size)
 * in events (e.g bytes).
 * @param peakincrement is the minimum interarrival in ticks.
 * @param jittertolerance is the jitter tolerance in ticks.
 * @param sustainedincrement in mean interarrival time in ticks.
 * @param maximumburstsize is the maximum burst or packet size in events.
 */
extern diminuto_ticks_t diminuto_shaper_bursttolerance(diminuto_ticks_t peakincrement, diminuto_ticks_t jittertolerance, diminuto_ticks_t sustainedincrement, size_t maximumburstsize);

/**
 * Log the state of a shaper.
 * @param shaperp is a pointer to the shaper.
 */
extern void diminuto_shaper_log(const diminuto_shaper_t * shaperp);

#endif
