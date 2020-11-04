/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2018-2020 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is the implementation of the Modulator feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is the implementation of the Modulator feature.
 */

#include "com/diag/diminuto/diminuto_modulator.h"
#include "com/diag/diminuto/diminuto_timer.h"
#include "com/diag/diminuto/diminuto_frequency.h"
#include "com/diag/diminuto/diminuto_coherentsection.h"
#include "com/diag/diminuto/diminuto_countof.h"
#include "com/diag/diminuto/diminuto_log.h"
#include <errno.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

int diminuto_modulator_set(diminuto_modulator_t * mp, diminuto_modulator_cycle_t duty)
{
    int rc = -1;
    diminuto_modulator_cycle_t on = 0;
    diminuto_modulator_cycle_t off = 0;
    diminuto_modulator_cycle_t prime = 0;
    diminuto_modulator_cycle_t on0 = 0;
    diminuto_modulator_cycle_t off0 = 0;
    static const diminuto_modulator_cycle_t PRIMES[] = { 17,  13,  11,   7,   5,   3,   2, };
    int ii = 0;

    /*
     * We don't have to check for the range of the duty cycle, since the
     * duty cycle can assume the full range of allowable integer values.
     */

    /*
     * 0% (0) <= on  <= 100% (255)
     * 0% (0) <= off <= 100% (255)
     * on = duty cycle
     * off = 100% - duty cycle
     * on + off == 100%
     */

    on = duty;
    off = DIMINUTO_MODULATOR_DUTY_MAX - duty;

    /*
     * Remove the common prime factors from the on and off cycles.
     * This smooths out the on versus off cycles and reduces visible flicker.
     */

    if (on == 0) {
        /* Fully off. */
    } else if (off == 0) {
        /* Fully on. */
    } else {
        for (ii = 0; ii < countof(PRIMES); ++ii) {
            prime = PRIMES[ii];
            while (((on0 = (on / prime)) > 0) && ((on % prime) == 0) && ((off0 = (off / prime)) > 0) && ((off % prime) == 0)) {
                on = on0;
                off = off0;
            }
        }
    }

    /*
     * Update the duty cycle in the fields shared with the callback.
     * If the callback hasn't used the prior duty cycle, return an
     * error.
     */

    DIMINUTO_CONDITION_BEGIN(&(mp->condition));

        mp->duty = duty;
        mp->ton = on;
        mp->toff = off;
        if (!(mp->set)) { 
            mp->set = !0;
            do {
                if ((rc = diminuto_condition_wait(&(mp->condition))) != 0) {
                    errno = rc;
                    break;
                }
            } while (mp->set);
        } else {
            rc = 0;
        }

    DIMINUTO_CONDITION_END;

    return rc;
}

unsigned int diminuto_modulator_flicker(const diminuto_modulator_t * mp)
{
    double score;
    if (mp->toff == 0) {
        score = 0.0;
    } else if (mp->ton == 0) {
        score = 0.0;
    } else {
        double accum;
        accum = abs(mp->toff - mp->ton);
        accum /= 255.0;
        score = accum;
        accum = abs(mp->toff + mp->ton);
        accum /= 255.0;
        score += accum;
        score /= 2.0;
        score *= 100.0;
    }
    return (unsigned int)score;
}

static void * callback(void * vp)
{
    diminuto_modulator_t * mp = (diminuto_modulator_t *)0;
    int rc = 0;

    mp = (diminuto_modulator_t *)vp;

    do {

        /*
         * Complete the current on or off cycle.
         */

        if (mp->cycle > 0) {
            mp->cycle -= 1;
            break;
        }

        /*
         * We change the pin state only at the end
         * of a complete cycle. We check for the edge
         * cases of 100% (always on) and 0% (always
         * off).
         */

        if (mp->state) {
            if (mp->off > 0) {
                rc = diminuto_pin_clear(mp->fp);
                if (rc < 0) { mp->error = errno; }
                mp->cycle = mp->off;
                mp->state = 0;
            } else {
                mp->cycle = mp->on; /* 100% */
            }
        } else {
            if (mp->on > 0) {
                rc = diminuto_pin_set(mp->fp);
                if (rc < 0) { mp->error = errno; }
                mp->cycle = mp->on;
                mp->state = !0;
            } else {
                mp->cycle = mp->off; /* 0% */
            }
        }

        /*
         * Complete the current 100% period, after which
         * the output will be in the off state.
         */

        if (mp->period > 0) {
            mp->period -= 1;
            break;
        }

        mp->period = DIMINUTO_MODULATOR_DUTY_MAX;

        /*
         * We update the duty cycle only at the end of
         * a complete period. This code may occasionally
         * block, but it will do so after updating the
         * output state for the prior settings, which
         * occurs as close to the beginning of the timer
         * firing as we can get it.
         */

        DIMINUTO_CONDITION_BEGIN(&(mp->condition));

            if (mp->set) {
                mp->on = mp->ton;
                mp->off = mp->toff;
                mp->set = 0;
                rc = diminuto_condition_signal(&(mp->condition));
                if (rc != 0) { mp->error = rc; }
            }

        DIMINUTO_CONDITION_END;

    } while (0);

    return (void *)(uintptr_t)(mp->cycle);
}

diminuto_modulator_t * diminuto_modulator_init(diminuto_modulator_t * mp, int pin, diminuto_modulator_cycle_t duty)
{
    diminuto_modulator_t * result = (diminuto_modulator_t *)0;
    diminuto_timer_t * tp = (diminuto_timer_t *)0;
    diminuto_condition_t * cp = (diminuto_condition_t *)0;
    int rc = -1;

    do {

        /*
         * Set up the modulator to be ready to immediately
         * transition to an on state from what appears to be
         * an off state.
         */

        mp->pin = pin;
        mp->error = 0;
        mp->state = 0;
        mp->duty = duty;
        mp->on = DIMINUTO_MODULATOR_DUTY_MIN;
        mp->off = DIMINUTO_MODULATOR_DUTY_MAX;
        mp->cycle = 0;
        mp->period = 0;
        mp->set = !0;
        mp->ton = DIMINUTO_MODULATOR_DUTY_MIN;
        mp->toff = DIMINUTO_MODULATOR_DUTY_MAX;

        rc = diminuto_pin_unexport_ignore(pin);
        if (rc < 0) {
            break;
        }

        tp = diminuto_timer_init_periodic(&(mp->timer), callback);
        if (tp == (diminuto_timer_t *)0) {
            break;
        }

        cp = diminuto_condition_init(&(mp->condition));
        if (cp == (diminuto_condition_t *)0) {
            break;
        }

        mp->fp = diminuto_pin_output(pin);
        if (mp->fp == (FILE *)0) {
            break;
        }

        rc = diminuto_modulator_set(mp, duty);
        if (rc < 0) {
            break;
        }

#if 0
        DIMINUTO_LOG_DEBUG("%s[%d]: pin=%d state=%d duty=%d on=%d off=%d cycle=%d period=%d set=%d ton=%d toff=%d\n", __FILE__, __LINE__, mp->pin, mp->state, mp->duty, mp->on, mp->off, mp->cycle, mp->period, mp->set, mp->ton, mp->toff);
#endif

        result = mp;

    } while (0);

    return result;
}

int diminuto_modulator_start(diminuto_modulator_t * mp)
{
    diminuto_sticks_t ticks = -1;

    mp->error = 0;

    ticks = diminuto_frequency() / diminuto_modulator_frequency();
    ticks = diminuto_timer_start(&(mp->timer), ticks, mp);

    return (ticks >= 0) ? 0 : -1;
}

int diminuto_modulator_stop(diminuto_modulator_t * mp)
{
    diminuto_sticks_t ticks = -1;

    ticks = diminuto_timer_stop(&(mp->timer));

    return (ticks >= 0) ? 0 : -1;
}

diminuto_modulator_t * diminuto_modulator_fini(diminuto_modulator_t * mp)
{
    diminuto_modulator_t * result = (diminuto_modulator_t *)0;

    if (diminuto_condition_fini(&(mp->condition)) != (diminuto_condition_t *)0) {
        result = mp;
    }

    if (diminuto_timer_fini(&(mp->timer)) != (diminuto_timer_t *)0) {
        result = mp;
    }

    if ((mp->fp = diminuto_pin_unused(mp->fp, mp->pin)) != (FILE *)0) {
        result = mp;
    }

    return result;
}
