/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_METER_
#define _H_COM_DIAG_DIMINUTO_METER_

/**
 * @file
 * @copyright Copyright 2022 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief Implements a Meter with a peak and sustainable rate.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * Implements a Meter that measures a peak and sustainable rate.
 */

#include "com/diag/diminuto/diminuto_types.h"
#include "com/diag/diminuto/diminuto_time.h"

typedef struct DiminutoMeter {
    diminuto_ticks_t start;
    diminuto_ticks_t last;
    double shortest;
    size_t events;
    size_t burst;
} diminuto_meter_t;

static inline diminuto_sticks_t diminuto_meter_now()
{
    return diminuto_time_elapsed();
}

extern diminuto_meter_t * diminuto_meter_reset(diminuto_meter_t * mp, diminuto_sticks_t now);

extern int diminuto_meter_events(diminuto_meter_t * mp, diminuto_sticks_t now, size_t events);

static inline diminuto_meter_t * diminuto_meter_init(diminuto_meter_t * mp)
{
    return diminuto_meter_reset(mp, diminuto_meter_now());
}

static inline diminuto_meter_t * diminuto_meter_start(diminuto_meter_t * mp)
{
    return diminuto_meter_reset(mp, diminuto_meter_now());
}

static inline diminuto_meter_t * diminuto_meter_fini(diminuto_meter_t * mp)
{
    return (diminuto_meter_t *)0;
}

static int diminuto_meter_event(diminuto_meter_t * mp, diminuto_sticks_t now)
{
    return diminuto_meter_events(mp, now, 1);
}

static inline int diminuto_meter_events_now(diminuto_meter_t * mp, size_t events)
{
    return diminuto_meter_events(mp, diminuto_meter_now(), events);
}

static inline int diminuto_meter_event_now(diminuto_meter_t * mp)
{
    return diminuto_meter_events_now(mp, 1);
}

static double diminuto_meter_peak(const diminuto_meter_t * mp)
{
    return 0;
}

static double diminuto_meter_sustained(const diminuto_meter_t * mp)
{
    return 0;
}

#endif
