/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2020 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is a unit test of the Timer feature with One Shot conditions.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is a unit test of the Timer feature with One Shot conditions.
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_delay.h"
#include "com/diag/diminuto/diminuto_time.h"
#include "com/diag/diminuto/diminuto_timer.h"
#include "com/diag/diminuto/diminuto_frequency.h"
#include "com/diag/diminuto/diminuto_condition.h"
#include <errno.h>
#include <stdint.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>

static diminuto_condition_t condition;
static diminuto_sticks_t when;

static void * callback(void * cp)
{
    int rc = 0;

    DIMINUTO_CONDITION_BEGIN(&condition);
        ASSERT((when = diminuto_time_elapsed()) >= 0);
        ASSERT((rc = diminuto_condition_signal(&condition)) == 0);
    DIMINUTO_CONDITION_END;

    return (void *)(intptr_t)rc;
}

int main(int argc, char ** argv)
{
    diminuto_timer_t timer;
    diminuto_sticks_t value;
    diminuto_sticks_t result;
    diminuto_ticks_t hertz;
    diminuto_ticks_t frequency;
    diminuto_ticks_t then;
    diminuto_ticks_t now;
    diminuto_ticks_t measured;
    diminuto_ticks_t requested;
    diminuto_ticks_t actual;
    diminuto_ticks_t claimed;
    diminuto_sticks_t difference;
    char rs;
    char cs;
    char ms;
    int rday;
    int rhour;
    int rminute;
    int rsecond;
    diminuto_ticks_t rtick;
    int cday;
    int chour;
    int cminute;
    int csecond;
    diminuto_ticks_t ctick;
    int mday;
    int mhour;
    int mminute;
    int msecond;
    diminuto_ticks_t mtick;
    double delta;

    SETLOGMASK();

    TEST();

    ASSERT(diminuto_condition_init(&condition) == &condition);
    ASSERT(diminuto_timer_init_oneshot(&timer, callback) == &timer);

    diminuto_core_enable();

    hertz = diminuto_frequency();

    frequency = diminuto_timer_frequency();
    DIMINUTO_LOG_INFORMATION("timer frequency %llu Hz\n", frequency);

    DIMINUTO_LOG_INFORMATION("%21s %21s %21s %11s\n",
        "requested", "claimed", "measured", "error");

    for (requested = hertz / 1000; requested <= (hertz * 9 * 60); requested *= 2) {
        ASSERT((result = diminuto_time_elapsed()) != (diminuto_sticks_t)-1);
        then = result;
        DIMINUTO_CONDITION_BEGIN(&condition);
            ASSERT(diminuto_timer_error(&timer) == 0);
            ASSERT(diminuto_timer_start(&timer, requested, (void *)0) != (diminuto_sticks_t)-1);
            ASSERT(diminuto_condition_wait_until(&condition, diminuto_condition_clock() + (requested * 2)) == 0);
            ASSERT(diminuto_timer_error(&timer) == 0);
            ASSERT(diminuto_timer_stop(&timer) != (diminuto_sticks_t)-1);
            ASSERT(diminuto_timer_error(&timer) == 0);
            ASSERT((result = diminuto_time_elapsed()) != (diminuto_sticks_t)-1);
            now = result;
            actual = when;
        DIMINUTO_CONDITION_END;
        ASSERT(actual >= then);
        ASSERT(now >= then);
        ASSERT(now >= actual);
        claimed = actual - then;
        measured = now - then;
        difference = measured - requested;
        delta = (100.0 * difference) / requested;
        rs = (diminuto_time_duration(requested, &rday, &rhour, &rminute, &rsecond, &rtick) < 0) ? '-' : '+';
        cs = (diminuto_time_duration(claimed,  &cday, &chour, &cminute, &csecond, &ctick) < 0) ? '-' : '+';
        ms = (diminuto_time_duration(measured,  &mday, &mhour, &mminute, &msecond, &mtick) < 0) ? '-' : '+';
        DIMINUTO_LOG_INFORMATION("%c%1.1d/%2.2d:%2.2d:%2.2d.%9.9llu %c%1.1d/%2.2d:%2.2d:%2.2d.%9.9llu %c%1.1d/%2.2d:%2.2d:%2.2d.%9.9llu %10.3lf%%\n"
            , rs, rday, rhour, rminute, rsecond, rtick
            , cs, cday, chour, cminute, csecond, ctick
            , ms, mday, mhour, mminute, msecond, mtick
            , delta
        );
    }

    ASSERT(diminuto_timer_fini(&timer) == (diminuto_timer_t *)0);
    ASSERT(diminuto_condition_fini(&condition) == (diminuto_condition_t *)0);

    STATUS();

    EXIT();
}
