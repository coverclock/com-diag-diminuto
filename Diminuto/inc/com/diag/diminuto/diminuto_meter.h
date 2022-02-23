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
 * See also the Throttle and Shaper features. Note that floating point
 * arithmetic is used by the Meter to compute its metrics.
 */

#include "com/diag/diminuto/diminuto_types.h"
#include "com/diag/diminuto/diminuto_time.h"
#include "com/diag/diminuto/diminuto_frequency.h"
#include "com/diag/diminuto/diminuto_minmaxof.h"
#include <math.h> /* For HUGE_VAL. */

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
    size_t count;               /**< Number of measurements taken. */
} diminuto_meter_t;

/**
 * @def DIMINUTO_METER_INITIALIZER
 * This macro generates a static initializer for a Meter object.
 */
#define DIMINUTO_METER_INITIALIZER \
    { \
        0, \
        0, \
        0.0, \
        0, \
        0, \
        0, \
    }

/*******************************************************************************
 * CONSTANTS
 ******************************************************************************/

/**
 * When an event count gets too large, this is the value to which it is set.
 */
static const size_t DIMINUTO_METER_OVERFLOW = diminuto_maximumof(size_t);

/**
 * When a rate cannot be computed, this is the value to which it is set.
 */
static const double DIMINUTO_METER_ERROR = HUGE_VAL;

/*******************************************************************************
 * TIME
 ******************************************************************************/

/**
 * @def COM_DIAG_DIMINUTO_METER_FREQUENCY
 * This manifest constant is the frequency in Hertz at which this feature
 * operates. The inverse of this value is the smallest unit of time in fractions
 * of a second that this feature can express or use. This constant is provided
 * for use in those cases where it is useful to have the value at compile time.
 * However, you chould always prefer to use the inline function when possible.
 */
#define COM_DIAG_DIMINUTO_METER_FREQUENCY (COM_DIAG_DIMINUTO_TIME_FREQUENCY)

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
 * @return the current time from a monotonically increasing clock or <0 of an error occurred.
 */
static inline diminuto_sticks_t diminuto_meter_now()
{
    return diminuto_time_elapsed();
}

/*******************************************************************************
 * RESET
 ******************************************************************************/

/**
 * Reset a Meter to the beginning of time.
 * @param meterp points to the Meter.
 * @param now is the time from which the sustainable rate will be computed.
 * @return a pointer to the Meter or NULL with errno set if an error occurred.
 */
extern diminuto_meter_t * diminuto_meter_reset(diminuto_meter_t * meterp, diminuto_ticks_t now);

/*******************************************************************************
 * INITIALIZATION
 ******************************************************************************/

/**
 * Intialize a Meter.
 * @param meterp points to the Meter.
 * @param now is the time from which the sustainable rate will be computed.
 * @return a pointer to the Meter or NULL with errno set if an error occurred.
 */
static inline diminuto_meter_t * diminuto_meter_init(diminuto_meter_t * meterp, diminuto_ticks_t now)
{
    return diminuto_meter_reset(meterp, now);
}

/**
 * Release any resources held by a Meter.
 * @param meterp points to the Meter.
 * @return NULL for success or a pointer to the Meter with errno set if an
 * error occurred.
 */
static inline diminuto_meter_t * diminuto_meter_fini(diminuto_meter_t * meterp)
{
    return (diminuto_meter_t *)0;
}

/*******************************************************************************
 * STATE CHANGE
 ******************************************************************************/

/**
 * Added a specified number of events starting at a specified time to a Meter.
 * @param meterp points to the Meter.
 * @param now is the time at which the events occurred.
 * @param events is the number of events.
 * @return 0 for success or <0 with errno set if an error occurred.
 */
extern int diminuto_meter_events(diminuto_meter_t * meterp, diminuto_ticks_t now, size_t events);

/**
 * Add a single event at a specified time to a Meter.
 * @param meterp points to the Meter.
 * @param now is the time at which the event occurred.
 * @return 0 for success or <0 with errno set if an error occurred.
 */
static int diminuto_meter_event(diminuto_meter_t * meterp, diminuto_ticks_t now)
{
    return diminuto_meter_events(meterp, now, 1);
}

/**
 * Process zero events to indicate that time has passed without an event
 * occurring, updating the state of the meter.
 * @param shaperp is a pointer to the meter.
 * @param now is the current time on a monotonically increasing clock.
 * @return 0 for success or <0 with errno set if an error occurred.
 */
static inline int diminuto_meter_update(diminuto_meter_t * meterp, diminuto_ticks_t now)
{
    return diminuto_meter_events(meterp, now, 0);
}

/*******************************************************************************
 * GETTORS
 ******************************************************************************/

/**
 * Return the time interval between the last and start events in ticks.
 * @param meterp points to the Meter.
 * @return the time interval in ticks.
 */
static inline diminuto_ticks_t diminuto_meter_interval(const diminuto_meter_t * meterp)
{
    return (meterp->last - meterp->start);
}

/**
 * Return the total number of events.
 * @param meterp points to the Meter.
 * @return the total number of events.
 */
static inline size_t diminuto_meter_total(const diminuto_meter_t * meterp)
{
    return meterp->events;
}

/**
 * Return the current maximum burst size from a Meter.
 * @param meterp points to the Meter.
 * @return the maximum burst size.
 */
static inline size_t diminuto_meter_burst(const diminuto_meter_t * meterp)
{
    return meterp->burst;
}

/**
 * Return the current average block size from a Meter.
 * @param meterp points to the Meter.
 * @return the average block size.
 */
static inline double diminuto_meter_average(const diminuto_meter_t * meterp)
{
    double result = 0.0;

    result = meterp->events;
    result /= meterp->count;

    return result;
}

/**
 * Return the current measurement count from a Meter.
 * @param meterp points to the Meter.
 * @return the current measurement count.
 */
static inline size_t diminuto_meter_count(const diminuto_meter_t * meterp)
{
    return meterp->count;
}

/**
 * Return the peak rate from a Meter.
 * @param meterp points to the Meter.
 * @return the peak rate.
 */
static inline double diminuto_meter_peak(const diminuto_meter_t * meterp)
{
    return meterp->peak;
}

/**
 * Compute the sustained rate from a Meter based on the start time and
 * the last time and the total number of events.
 * @param meterp points to the meter.
 * @return the sustained rate.
 */
extern double diminuto_meter_sustained(const diminuto_meter_t * meterp);

#endif
