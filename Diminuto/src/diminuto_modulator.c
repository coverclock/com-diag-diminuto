/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2018-2022 Digital Aggregates Corporation, Colorado, USA.
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

unsigned int diminuto_modulator_flicker(diminuto_modulator_cycle_t on, diminuto_modulator_cycle_t off)
{
    double score;

    if (off == 0) {
        score = 0.0;
    } else if (on == 0) {
        score = 0.0;
    } else {

        double accum;

        accum = abs(off - on);
        accum /= 255.0;
        score = accum;
        accum = abs(off + on);
        accum /= 255.0;
        score += accum;
        score /= 2.0;
        score *= 100.0;

    }

    return (unsigned int)score;
}

int diminuto_modulator_factor(diminuto_modulator_cycle_t * onp, diminuto_modulator_cycle_t * offp) {
    static const diminuto_modulator_cycle_t PRIME[] = { 2, 3, 5, 7, 11, 13, };
    int rc = 0;
    int ii = 0;

    /*
     * This would be a lot faster if it were table-driven.
     */

    for (ii = 0; ii < countof(PRIME); ++ii) {
        if (PRIME[ii] > *onp) {
            break;
        } else if (PRIME[ii] > *offp) {
            break;
        } else if ((*onp % PRIME[ii]) > 0) {
            continue;
        } else if ((*offp % PRIME[ii]) > 0) {
            continue;
        } else {
            *onp /= PRIME[ii];
            *offp /= PRIME[ii];
            rc = !0;
        }
    }

    return rc;
}

diminuto_modulator_state_t diminuto_modulator_state(diminuto_modulator_t * mp) {
    diminuto_modulator_state_t result = DIMINUTO_MODULATOR_STATE_UNKNOWN;

    DIMINUTO_CONDITION_BEGIN(&(mp->condition));
        result = (diminuto_modulator_state_t)diminuto_timer_state(&(mp->timer));
    DIMINUTO_CONDITION_END;

    return result;
}

int diminuto_modulator_set(diminuto_modulator_t * mp, diminuto_modulator_cycle_t duty)
{
    int rc = -1;
    diminuto_modulator_cycle_t on0 = 0;
    diminuto_modulator_cycle_t off0 = 0;
    diminuto_modulator_cycle_t on1 = 0;
    diminuto_modulator_cycle_t off1 = 0;

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

    on0 = duty;
    off0 = DIMINUTO_MODULATOR_DUTY_MAX - duty;

    /*
     * Remove the common prime factors from the on and off cycles.
     * This smooths out the on versus off cycles and reduces visible flicker.
     * N.B. sqrt(255) < 16, and 13 is the largest prime < 16.
     */

    on1 = on0;
    off1 = off0;

    if (on1 == 0) {
        /* Do nothing: fully off. */
    } else if (off1 == 0) {
        /* Do nothing: fully on. */
    } else {
        (void)diminuto_modulator_factor(&on1, &off1);
    }

    /*
     * Update the duty cycle in the fields shared with the callback.
     * (We use the timer state instead of the modulator state because
     * we are already inside a critical section.)
     */

    DIMINUTO_CONDITION_BEGIN(&(mp->condition));

        mp->duty = duty;
        mp->ton = on1;
        mp->toff = off1;
        mp->set = !0;

        if (diminuto_timer_state(&(mp->timer)) == DIMINUTO_TIMER_STATE_ARM) {
            while (mp->set) {
                if ((rc = diminuto_condition_wait(&(mp->condition))) != 0) {
                    errno = rc;
                    break;
                }
            }
        } else {
            rc = 0;
        }

        DIMINUTO_LOG_DEBUG("diminuto_modulator@%p: set pin=%d error=%d duty=%d on0=%d off0=%d on=%d off=%d cycle=%d ton=%d toff=%d state=%d set=%d flicker=%u\n", mp, mp->pin, mp->error, mp->duty, on0, off0, mp->on, mp->off, mp->cycle, mp->ton, mp->toff, mp->state, mp->set, diminuto_modulator_flicker(mp->ton, mp->toff));

    DIMINUTO_CONDITION_END;

    return rc;
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
         * We update the duty cycle only at the beginning of
         * the next on/off cycle. This code may occasionally
         * block, but it will do so only after updating the
         * output state for the prior settings, which
         * occurs as close to the beginning of the timer
         * firing as we can get it.
         */

        if (!mp->state) {

            DIMINUTO_CONDITION_BEGIN(&(mp->condition));

                if (mp->set) {
                    mp->on = mp->ton;
                    mp->off = mp->toff;
                    mp->set = 0;
                    rc = diminuto_condition_signal(&(mp->condition));
                    if (rc != 0) {
                        mp->error = rc;
                    }
                }

            DIMINUTO_CONDITION_END;

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
                if (rc < 0) {
                    mp->error = errno;
                }
                mp->cycle = mp->off;
                mp->state = 0;
            } else {
                mp->cycle = mp->on; /* 100% */
            }
        } else {
            if (mp->on > 0) {
                rc = diminuto_pin_set(mp->fp);
                if (rc < 0) {
                    mp->error = errno;
                }
                mp->cycle = mp->on;
                mp->state = !0;
            } else {
                mp->cycle = mp->off; /* 0% */
            }
        }

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
        mp->set = 0;
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
