/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2020-2022 Digital Aggregates Corposcoren, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is a unit test of the Modulator feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is a unit test of the Modulator feature.
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_types.h"
#include "com/diag/diminuto/diminuto_frequency.h"
#include "com/diag/diminuto/diminuto_modulator.h"
#include "com/diag/diminuto/diminuto_delay.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_timer.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

static bool state = false;

static int callback(void * vp, bool value)
{
    *((bool *)vp) = value;
    return 1;
}

int main(int argc, char ** argv)
{
    diminuto_modulator_t modulator;

    SETLOGMASK();

    {
        TEST();

        ASSERT((int)DIMINUTO_MODULATOR_STATE_IDLE == (int)DIMINUTO_TIMER_STATE_IDLE);
        ASSERT((int)DIMINUTO_MODULATOR_STATE_ARM == (int)DIMINUTO_TIMER_STATE_ARM);
        ASSERT((int)DIMINUTO_MODULATOR_STATE_DISARM == (int)DIMINUTO_TIMER_STATE_DISARM);
        ASSERT((int)DIMINUTO_MODULATOR_STATE_UNKNOWN == (int)DIMINUTO_TIMER_STATE_UNKNOWN);

        STATUS();
    }

    {
        diminuto_ticks_t system;
        diminuto_ticks_t timer;
        diminuto_ticks_t modulator;
        diminuto_ticks_t period;
        diminuto_ticks_t nanoseconds;

        TEST();

        system = diminuto_frequency();
        timer = diminuto_timer_frequency();
        modulator = diminuto_modulator_frequency();
        period = system / modulator;
        nanoseconds = diminuto_frequency_ticks2units(period, 1000000000LLU);

        COMMENT("system=%lluhertz\n", (diminuto_llu_t)system);
        COMMENT("timer=%lluhertz\n", (diminuto_llu_t)timer);
        COMMENT("modulator=%lluhertz\n", (diminuto_llu_t)modulator);
        COMMENT("period=%lluticks=%lluns\n", (diminuto_llu_t)period, (diminuto_llu_t)nanoseconds);

        ASSERT(system > 0);
        ASSERT(timer > 0);
        ASSERT(modulator > 0);
        ASSERT(period > 0);

        STATUS();
    }

    {
        unsigned int duty;
        diminuto_modulator_cycle_t on;
        diminuto_modulator_cycle_t off;
        diminuto_modulator_cycle_t fon;
        diminuto_modulator_cycle_t foff;
        float percent;
        int rc;
        unsigned int flicker;
        unsigned int fflicker;

        TEST();

        ASSERT(diminuto_modulator_flicker(0, 255) == 0);
        ASSERT(diminuto_modulator_flicker(255, 0) == 0);

        for (duty = 0; duty < 256; ++duty) {
            on = fon = duty;
            percent = on;
            percent /= 255;
            percent *= 100.0;
            off = foff = 255 - duty;
            rc = diminuto_modulator_factor(&fon, &foff);
            flicker = diminuto_modulator_flicker(on, off);
            fflicker = diminuto_modulator_flicker(fon, foff);
            COMMENT("duty=%u percent=%.1f%% on=%u off=%u rc=%d fon=%u foff=%u flicker=%u:%u\n", duty, percent, on, off, rc, fon, foff, flicker, fflicker);
            ASSERT(((!rc) && (fon == on) && (foff == off)) || (rc && (fon != on) && (foff != off)));
            ASSERT(fon < 256);
            ASSERT(foff < 256);
            ASSERT((fon + foff) < 256);
            ASSERT(fon <= on);
            ASSERT(foff <= off);
            ASSERT(((on > off) && (fon > foff)) || ((on < off) && (fon < foff)));
            ASSERT((fon == 0) || ((on % fon) == 0));
            ASSERT((foff == 0) || ((off % foff) == 0));
            ASSERT((0 <= flicker) && (flicker <= 100));
            ASSERT((0 <= fflicker) && (fflicker <= 100));
            ASSERT(fflicker <= flicker);
        }

        STATUS();
    }

    {
        int duty;

        TEST();

        COMMENT("INIT");

        for (duty = 255; duty >= 0; --duty) {
            ASSERT(diminuto_modulator_init(&modulator, &callback, &state, duty) == &modulator);
            ASSERT(modulator.duty == duty);
            ASSERT(0 <= modulator.ton);
            ASSERT(modulator.ton <= 255);
            ASSERT(0 <= modulator.toff);
            ASSERT(modulator.toff <= 255);
            ASSERT(0 <= (modulator.ton + modulator.toff));
            ASSERT((modulator.ton + modulator.toff) <= 255);
            ASSERT((255 % (modulator.ton + modulator.toff)) == 0);
            if (duty > 0) {
                ASSERT(diminuto_modulator_fini(&modulator) == (diminuto_modulator_t *)0);
            }
        }

        STATUS();
    }

    {
        TEST();

        COMMENT("START");

        ASSERT(diminuto_modulator_state(&modulator) == DIMINUTO_MODULATOR_STATE_IDLE);
        ASSERT(diminuto_modulator_error(&modulator) == 0);
        ASSERT(diminuto_modulator_set(&modulator, 0) == 0);
        ASSERT(diminuto_modulator_start(&modulator) >= 0);
        ASSERT(diminuto_modulator_state(&modulator) == DIMINUTO_MODULATOR_STATE_ARM);
        diminuto_delay(diminuto_frequency(), 0);


        STATUS();
    }

    {
        int duty;

        TEST();

        COMMENT("SET");

        for (duty = 0; duty <= 255; ++duty) {
            ASSERT(diminuto_modulator_set(&modulator, duty) == 0);
            ASSERT(0 <= modulator.ton);
            ASSERT(modulator.ton <= 255);
            ASSERT(0 <= modulator.toff);
            ASSERT(modulator.toff <= 255);
            ASSERT(0 <= (modulator.ton + modulator.toff));
            ASSERT((modulator.ton + modulator.toff) <= 255);
            ASSERT((255 % (modulator.ton + modulator.toff)) == 0);
            diminuto_delay(diminuto_frequency(), 0);
        }

        STATUS();
    }

    {
        TEST();

        COMMENT("STOP");

        ASSERT(diminuto_modulator_state(&modulator) == DIMINUTO_MODULATOR_STATE_ARM);
        ASSERT(diminuto_modulator_set(&modulator, 255) == 0);
        ASSERT(diminuto_modulator_stop(&modulator) >= 0);
        ASSERT(diminuto_modulator_state(&modulator) == DIMINUTO_MODULATOR_STATE_IDLE);
        ASSERT(diminuto_modulator_error(&modulator) == 0);

        STATUS();
    }

    {
        TEST();

        COMMENT("FINI");

        ASSERT(diminuto_modulator_fini(&modulator) == (diminuto_modulator_t *)0);

        STATUS();
    }

    EXIT();
}
