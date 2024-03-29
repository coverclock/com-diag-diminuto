/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2014-2022 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is the implementation of the Shaper feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is the implementation of the Shaper feature.
 */

#include "com/diag/diminuto/diminuto_shaper.h"
#include "com/diag/diminuto/diminuto_frequency.h"
#include "com/diag/diminuto/diminuto_log.h"

diminuto_ticks_t diminuto_shaper_bursttolerance(diminuto_ticks_t peakincrement, diminuto_ticks_t jittertolerance, diminuto_ticks_t sustainedincrement, size_t maximumburstsize)
{
    diminuto_ticks_t limit = 0;
    diminuto_ticks_t increment = 0;

    if (maximumburstsize <= 1) {
        /* Do nothing. */
    } else if (sustainedincrement <= peakincrement) {
        /* Do nothing. */
    } else {
        limit = maximumburstsize - 1;
        increment = sustainedincrement;
        increment -= peakincrement;
        limit *= increment;
    }
    limit += jittertolerance;

    return limit;
}

diminuto_shaper_t * diminuto_shaper_init(diminuto_shaper_t * shaperp, diminuto_ticks_t peakincrement, diminuto_ticks_t jittertolerance, diminuto_ticks_t sustainedincrement, diminuto_ticks_t bursttolerance, diminuto_ticks_t now)
{
    diminuto_shaper_t * result = (diminuto_shaper_t *)0;

    if (diminuto_throttle_init(&(shaperp->peak), peakincrement, jittertolerance, now) == (diminuto_throttle_t *)0) {
        /* Do nothing. */
    } else if (diminuto_throttle_init(&(shaperp->sustained), sustainedincrement, bursttolerance, now) == (diminuto_throttle_t *)0) {
        /* Do nothing. */
    } else {
        result = shaperp;
    }

    return result;
}

diminuto_shaper_t * diminuto_shaper_reset(diminuto_shaper_t * shaperp, diminuto_ticks_t now)
{
    diminuto_shaper_t * result = (diminuto_shaper_t *)0;

    if (diminuto_throttle_reset(&(shaperp->peak), now) == (diminuto_throttle_t *)0) {
        /* Do nothing. */
    } else if (diminuto_throttle_reset(&(shaperp->sustained), now) == (diminuto_throttle_t *)0) {
        /* Do nothing. */
    } else {
        result = shaperp;
    }

    return result;
}

diminuto_ticks_t diminuto_shaper_request(diminuto_shaper_t * shaperp, diminuto_ticks_t now)
{
    diminuto_ticks_t peak = 0;
    diminuto_ticks_t sustained = 0;

    peak = diminuto_throttle_request(&(shaperp->peak), now);
    sustained = diminuto_throttle_request(&(shaperp->sustained), now);

    return (peak > sustained) ? peak : sustained;
}

int diminuto_shaper_commitn(diminuto_shaper_t * shaperp, size_t events)
{
    int peak = 0;
    int sustained = 0;

    peak = diminuto_throttle_commitn(&(shaperp->peak), events);
    sustained = diminuto_throttle_commitn(&(shaperp->sustained), events);

    return (peak || sustained);
}

int diminuto_shaper_admitn(diminuto_shaper_t * shaperp, diminuto_ticks_t now, size_t events)
{
    int peak = 0;
    int sustained = 0;

    peak = diminuto_throttle_admitn(&(shaperp->peak), now, events);
    sustained = diminuto_throttle_admitn(&(shaperp->sustained), now, events);

    return (peak || sustained);
}

diminuto_ticks_t diminuto_shaper_getexpected(const diminuto_shaper_t * shaperp)
{
    diminuto_ticks_t peak = 0;
    diminuto_ticks_t sustained = 0;

    peak = diminuto_throttle_getexpected(&(shaperp->peak));
    sustained = diminuto_throttle_getexpected(&(shaperp->sustained));

    return (peak > sustained) ? peak : sustained;
}

int diminuto_shaper_isempty(const diminuto_shaper_t * shaperp)
{
    int peak = 0;
    int sustained = 0;

    peak = diminuto_throttle_isempty(&(shaperp->peak));
    sustained = diminuto_throttle_isempty(&(shaperp->sustained));

    return peak && sustained;
}

int diminuto_shaper_isfull(const diminuto_shaper_t * shaperp)
{
    int peak = 0;
    int sustained = 0;

    peak = diminuto_throttle_isfull(&(shaperp->peak));
    sustained = diminuto_throttle_isfull(&(shaperp->sustained));

    return peak || sustained;
}

int diminuto_shaper_isalarmed(const diminuto_shaper_t * shaperp)
{
    int peak = 0;
    int sustained = 0;

    peak = diminuto_throttle_isalarmed(&(shaperp->peak));
    sustained = diminuto_throttle_isalarmed(&(shaperp->sustained));

    return peak || sustained;
}

int diminuto_shaper_emptied(const diminuto_shaper_t * shaperp)
{
    int peak = 0;
    int sustained = 0;

    peak = diminuto_throttle_emptied(&(shaperp->peak));
    sustained = diminuto_throttle_emptied(&(shaperp->sustained));

    return peak || sustained;
}

int diminuto_shaper_filled(const diminuto_shaper_t * shaperp)
{
    int peak = 0;
    int sustained = 0;

    peak = diminuto_throttle_filled(&(shaperp->peak));
    sustained = diminuto_throttle_filled(&(shaperp->sustained));

    return peak || sustained;
}

int diminuto_shaper_alarmed(const diminuto_shaper_t * shaperp)
{
    int peak = 0;
    int sustained = 0;

    peak = diminuto_throttle_alarmed(&(shaperp->peak));
    sustained = diminuto_throttle_alarmed(&(shaperp->sustained));

    return peak || sustained;
}

int diminuto_shaper_cleared(const diminuto_shaper_t * shaperp)
{
    int peak = 0;
    int sustained = 0;

    peak = diminuto_throttle_cleared(&(shaperp->peak));
    sustained = diminuto_throttle_cleared(&(shaperp->sustained));

    return peak || sustained;
}

void diminuto_shaper_log(const diminuto_shaper_t * shaperp)
{
    if (shaperp != (diminuto_shaper_t *)0) {
        diminuto_log_emit("diminuto_shaper_t@%p[%zu]: peak@%p\n", shaperp, sizeof(*shaperp), &(shaperp->peak));
        diminuto_throttle_log(&(shaperp->peak));
        diminuto_log_emit("diminuto_shaper_t@%p[%zu]: sustained@%p\n", shaperp, sizeof(*shaperp), &(shaperp->sustained));
        diminuto_throttle_log(&(shaperp->sustained));
    } else {
        diminuto_log_emit("diminuto_shaper_t@%p[%zu]\n", shaperp, sizeof(*shaperp));
    }
}
