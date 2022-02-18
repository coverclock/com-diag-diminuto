/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_METER_
#define _H_COM_DIAG_DIMINUTO_METER_

/**
 * @file
 * @copyright Copyright 2022 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief Implements a Meter with the peak and sustainable rate.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * Implements a Meter that measures the peak and sustainable rate.
 * See also the Throttle and Shaper features.
 */

#include "com/diag/diminuto/diminuto_types.h"
#include "com/diag/diminuto/diminuto_time.h"
#include "com/diag/diminuto/diminuto_frequency.h"
#include <math.h>

/*******************************************************************************
 * TYPES
 ******************************************************************************/

/**
 * This structure contains the state of the Meter, which measures the peak
 * (maximum instantaneous) and sustainable (long term mean) rates and the
 * maximum burst size.
 */
typedef struct DiminutoMeter {
    diminuto_ticks_t start;     /**< Monotonic time of start of meassurement. */
    diminuto_ticks_t last;      /**< Monotonic time of most recent event. */
    double peak;                /**< Highest instananeous rate. */
    size_t events;              /**< Total number of events. */
    size_t burst;               /**< Largest burst size of events. */
} diminuto_meter_t;

/*******************************************************************************
 * INITIALIZATION
 ******************************************************************************/

/**
 * @def COM_DIAG_DIMINUTO_METER_FREQUENCY
 * This manifest constant is the frequency in Hertz at which this feature
 * operates. The inverse of this value is the smallest unit of time in fractions
 * of a second that this feature can express or use. This constant is provided
 * for use in those cases where it is useful to have the value at compile time.
 * However, you chould always prefer to use the inline function when possible.
 */
#define COM_DIAG_DIMINUTO_METER_FREQUENCY (COM_DIAG_DIMINUTO_FREQUENCY)

/**
 * Return the resolution of the Diminuto Meter time units in ticks per second
 * (Hertz). Time intervals smaller than the equivalent period in ticks will
 * not yield the expected results.
 * @return the resolution in ticks per second.
 */
static inline diminuto_sticks_t diminuto_meter_frequency(void)
{
    return diminuto_time_frequency();
}

/**
 * Return the current time from a monotonically increasing clock.
 * @return the current time from a monotonically increasing clock.
 */
static inline diminuto_sticks_t diminuto_meter_now()
{
    return diminuto_time_elapsed();
}

/**
 * Reset a Meter to the beginning of time.
 * @param mp points to the Meter.
 * @param now is the time from which the sustainable rate will be computed.
 * @return a pointer to the Meter or NULL with errno set if an error occurred.
 */
extern diminuto_meter_t * diminuto_meter_reset(diminuto_meter_t * mp, diminuto_sticks_t now);

/**
 * Reset a Meter so that the beginning of time is taken from a monotonically
 * increasing clock.
 * @param mp points to the Meter.
 * @return a pointer to the Meter or NULL with errno set if an error occurred.
 */
static inline diminuto_meter_t * diminuto_meter_reset_now(diminuto_meter_t * mp)
{
    return diminuto_meter_reset(mp, diminuto_meter_now());
}

/**
 * Intialize a Meter.
 * @param mp points to the Meter.
 * @param now is the time from which the sustainable rate will be computed.
 * @return a pointer to the Meter or NULL with errno set if an error occurred.
 */
static inline diminuto_meter_t * diminuto_meter_init(diminuto_meter_t * mp, diminuto_sticks_t now)
{
    return diminuto_meter_reset(mp, now);
}

/**
 * Intialize a Meter. The sustainable rate will be computed from the current
 * time of a monotonically increasing clock.
 * @param mp points to the Meter.
 * @return a pointer to the Meter or NULL with errno set if an error occurred.
 */
static inline diminuto_meter_t * diminuto_meter_init_now(diminuto_meter_t * mp)
{
    return diminuto_meter_init(mp, diminuto_meter_now());
}

/**
 * Release any resources held by a Meter.
 * @param mp points to the Meter.
 * @return NULL for success or a pointer to the Meter with errno set if an
 * error occurred.
 */
static inline diminuto_meter_t * diminuto_meter_fini(diminuto_meter_t * mp)
{
    return (diminuto_meter_t *)0;
}

/*******************************************************************************
 * STATE CHANGE
 ******************************************************************************/

/**
 * Added a specified number of events starting at a specified time to a Meter.
 * @param mp points to the Meter.
 * @param now is the time at which the events occurred.
 * @param events is the number of events.
 * @return 0 for success or <0 with errno set if an error occurred.
 */
extern int diminuto_meter_events(diminuto_meter_t * mp, diminuto_sticks_t now, size_t events);

/**
 * Add a single event at a specified time to a Meter.
 * @param mp points to the Meter.
 * @param now is the time at which the event occurred.
 * @return 0 for success or <0 with errno set if an error occurred.
 */
static int diminuto_meter_event(diminuto_meter_t * mp, diminuto_sticks_t now)
{
    return diminuto_meter_events(mp, now, 1);
}

/**
 * Added a specified number of events starting at the time from a monotonically
 * increasing clock.
 * @param mp points to the Meter.
 * @param events is the number of events.
 * @return 0 for success or <0 with errno set if an error occurred.
 */
static inline int diminuto_meter_events_now(diminuto_meter_t * mp, size_t events)
{
    return diminuto_meter_events(mp, diminuto_meter_now(), events);
}

/**
 * Add a single event at a specified time to a Meter at the time from ta
 * monotonically increasing clock.
 * @param mp points to the Meter.
 * @return 0 for success or <0 with errno set if an error occurred.
 */
static inline int diminuto_meter_event_now(diminuto_meter_t * mp)
{
    return diminuto_meter_events_now(mp, 1);
}

/*******************************************************************************
 * GETTORS
 ******************************************************************************/

/**
 * Return the current maximum burst size from a Meter.
 * @param mp points to the Meter.
 * @return the maximum burst size.
 */
static inline size_t diminuto_meter_burst(const diminuto_meter_t * mp)
{
    return mp->burst;
}

/**
 * Return the peak rate from a Meter.
 * @param mp points to the Meter.
 * @return the peak rate.
 */
static inline double diminuto_meter_peak(const diminuto_meter_t * mp)
{
    return mp->peak;
}

/**
 * Compute the sustained rate from a Meter based on the start time and
 * the last time and the total number of events.
 * @param mp points to the meter.
 * @return the sustained rate.
 */
extern double diminuto_meter_sustained(const diminuto_meter_t * mp);

#endif
