/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_TIMER_PRIVATE_
#define _H_COM_DIAG_DIMINUTO_TIMER_PRIVATE_

/**
 * @file
 * @copyright Copyright 2022 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is the private API for the Timer feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is the private API for the Timer feature.
 */

#include "com/diag/diminuto/diminuto_types.h"
#include "com/diag/diminuto/diminuto_frequency.h"

/**
 * Compute the timeout window in ticks for a timer interval in ticks. This
 * is done because the Delay and Condition features used by Timer (including
 * its unit tests) have latencies different from that of timers (on at least
 * some target processors and platforms they are longer) and so cannot support
 * the same resolution as timers. This causes unit tests to fail. The minimum
 * window is somewhat arbitarily set to one second; smaller values would
 * probably work, but this interval will only take effect in the event of a
 * timer failure.
 * @param interval is the timer interval in ticks.
 * @return the number of ticks to wait before deciding the timer has failed.
 */
static inline diminuto_ticks_t diminuto_timer_window(diminuto_ticks_t interval)
{
    diminuto_ticks_t minimum = 0;
    diminuto_ticks_t result = 0;

    minimum = diminuto_frequency();
    result = interval * 2;
    if (result < minimum) {
        result = minimum;
    }

    return result;
}

#endif
