/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2018-2020 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in LICENSE.txt<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 */

#include "com/diag/diminuto/diminuto_modulator.h"
#include "com/diag/diminuto/diminuto_timer.h"
#include "com/diag/diminuto/diminuto_frequency.h"
#include "com/diag/diminuto/diminuto_coherentsection.h"
#include "com/diag/diminuto/diminuto_countof.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "../src/diminuto_timer.h"
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sched.h>
#include <stdint.h>

/*
 * N.B. casting mp->function to void * below produces an error. But
 * NOT casting mp->function to void * produces another error when
 * using %p. In effect, there is no way to print a function pointer
 * using %p without causing a warning. Hence the weird casting below.
 */

void diminuto_modulator_print(FILE * fp, const diminuto_modulator_t * mp)
{
    fprintf(fp,	"modulator@%p[%zu]:"
                    " function=%p"
                    " fp=%p"
                    " pin=%d"
                    " duty=%d"
                    " on=%d"
                    " off=%d"
                    " set=%d"
                    " timer=%p"
                    " initialized=%d"
                    " total=%d"
                    " cycle=%d"
                    " ton=%d"
                    " toff=%d"
                    " condition=%d"
                    " data=%p"
                    "\n",
        (void *)mp,
        sizeof(*mp),
        (void *)(uintptr_t)mp->function,
        (void *)(mp->fp),
        mp->pin,
        mp->duty,
        mp->on,
        mp->off,
        mp->set,
        (void *)(mp->timer),
        mp->initialized,
        mp->total,
        mp->cycle,
        mp->ton,
        mp->toff,
        mp->condition,
        mp->data
    );
}

int diminuto_modulator_set(diminuto_modulator_t * mp, diminuto_modulator_cycle_t duty)
{
    int rc = -1;
    diminuto_modulator_cycle_t on = 0;
    diminuto_modulator_cycle_t off = 0;
    diminuto_modulator_cycle_t prime = 0;
    static const diminuto_modulator_cycle_t PRIMES[] = { 7, 5, 3, 2 };
    int ii = 0;

    if (duty <= DIMINUTO_MODULATOR_MINIMUM_DUTY) {
        duty = DIMINUTO_MODULATOR_MINIMUM_DUTY;
    } else if (duty >= DIMINUTO_MODULATOR_MAXIMUM_DUTY) {
        duty = DIMINUTO_MODULATOR_MAXIMUM_DUTY;
    } else {
       /* Do nothing. */
    }

    /*
     * 0% <= on <=  100%
     * 0% <= off <=  100%
     * on = duty cycle
     * off = 100% - duty cycle
     * on + off == 100%
     */

    on = duty;
    off = DIMINUTO_MODULATOR_MAXIMUM_DUTY - duty;

    /*
     * Remove the common prime factors from the on and off cycles.
     * This smooths out the on versus off cycles and reduces visible flicker.
     * This code is so counter intuitive that a year after writing it
     * I had no idea what I was doing at first.
     */

    for (ii = 0; ii < countof(PRIMES); ++ii) {
        prime = PRIMES[ii];
        while (((on / prime) > 0) && ((on % prime) == 0) && ((off / prime) > 0) && ((off % prime) == 0)) {
            on /= prime;
            off /= prime;
        }
    }

    /*
     * Yeah, this is sketchy. But blocking the modulator interrupt service routine on a mutex
     * messes up what little real-time goodness we have.
     */

    DIMINUTO_COHERENT_SECTION_BEGIN;

        if (!mp->set) {
            mp->duty = duty;
            mp->ton = on;
            mp->toff = off;
            mp->set = !0;
            rc = 0;
        }

    DIMINUTO_COHERENT_SECTION_END;

    return rc;
}

void diminuto_modulator_function(union sigval arg)
{
    diminuto_modulator_t * mp = (diminuto_modulator_t *)0;

    mp = (diminuto_modulator_t *)(arg.sival_ptr);

    do {

        if (mp->cycle > 0) {
            mp->cycle -= 1;
            continue;
        }

        if (mp->condition) {
            if (mp->off > 0) {
                (void)diminuto_pin_clear(mp->fp);
                mp->condition = 0;
                mp->cycle = mp->off;
            } else {
                mp->cycle = mp->on; /* 100% */
            }
        } else {
            if (mp->on > 0) {
                (void)diminuto_pin_set(mp->fp);
                mp->condition = !0;
                mp->cycle = mp->on;
            } else {
                mp->cycle = mp->off; /* 0% */
            }
        }

        mp->total += 1;
        if (mp->total < DIMINUTO_MODULATOR_MAXIMUM_DUTY) {
            continue;
        }

        DIMINUTO_COHERENT_SECTION_BEGIN;

            if (mp->set) {
                mp->on = mp->ton;
                mp->off = mp->toff;
                mp->set = 0;
            }

        DIMINUTO_COHERENT_SECTION_END;

        mp->total = 0;

    } while (0);

    return;
}

int diminuto_modulator_init_generic(diminuto_modulator_t * mp, diminuto_modulator_function_t * funp, int pin, diminuto_modulator_cycle_t duty)
{
    int rc = -1;

    do {

        memset(mp, 0, sizeof(*mp));

        mp->function = funp;
        mp->pin = pin;
        mp->toff = DIMINUTO_MODULATOR_MAXIMUM_DUTY;

        (void)diminuto_pin_unexport_ignore(pin);

        mp->fp = diminuto_pin_output(pin);
        if (mp->fp == (FILE *)0) {
            break;
        }

        diminuto_modulator_set(mp, duty);

        rc = 0;

    } while (0);

    return rc;
}

int diminuto_modulator_start(diminuto_modulator_t * mp)
{
    diminuto_sticks_t ticks = 0;
    int rc = 0;
    struct sched_param param = { 0 };
    struct sigevent event = { 0 };

    if ((rc = pthread_attr_init(&(mp->attributes))) != 0) {
        errno = rc;
        diminuto_perror("diminuto_modulator_stop: pthread_attr_init");
    } else if ((rc = pthread_attr_setschedpolicy(&(mp->attributes), SCHED_FIFO)) != 0) {
        errno = rc;
        diminuto_perror("diminuto_modulator_stop: pthread_attr_setsched_policy");
    } else if ((param.sched_priority = sched_get_priority_max(SCHED_FIFO)) < 0) {
        diminuto_perror("diminuto_modulator_stop: sched_get_priority_max");
        param.sched_priority = 0;
    } else if ((rc = pthread_attr_setschedparam(&(mp->attributes), &param)) != 0) {
        errno = rc;
        diminuto_perror("diminuto_modulator_stop: pthread_attr_setschedparam");
    } else {
        /* Do nothing. */
    }

    event.sigev_notify = SIGEV_THREAD;
    event.sigev_value.sival_ptr = (void *)mp;
    event.sigev_notify_function = mp->function;
    event.sigev_notify_attributes = &(mp->attributes);

    ticks = diminuto_frequency() / diminuto_modulator_frequency();
    ticks = diminuto_timer_generic(&(mp->initialized), &(mp->timer), &event, ticks, !0);

    return (ticks >= 0) ? 0 : -1;
}

int diminuto_modulator_stop(diminuto_modulator_t * mp)
{
    diminuto_sticks_t ticks = 0;
    struct sigevent event = { 0 };
    int rc = 0;

    ticks = diminuto_timer_generic(&(mp->initialized), &(mp->timer), &event, ticks, !0);

    rc = pthread_attr_destroy(&(mp->attributes));
    if (rc != 0) {
        diminuto_perror("diminuto_modulator_stop: pthread_attr_init");
    }

    return (ticks >= 0) ? 0 : -1;
}

int diminuto_modulator_fini(diminuto_modulator_t * mp)
{
    int rc = 0;

    mp->fp = diminuto_pin_unused(mp->fp, mp->pin);
    if (mp->fp != (FILE *)0) {
        rc = -1;
    }

    return rc;
}
