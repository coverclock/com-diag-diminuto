/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2018 Digital Aggregates Corporation, Colorado USA<BR>
 * Licensed under the terms in LICENSE.txt<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 *
 * USAGE
 *
 * EXAMPLES
 *
 * ABSTRACT
 *
 */

#include "com/diag/diminuto/diminuto_alarm.h"
#include "com/diag/diminuto/diminuto_countof.h"
#include "com/diag/diminuto/diminuto_criticalsection.h"
#include "com/diag/diminuto/diminuto_frequency.h"
#include "com/diag/diminuto/diminuto_interrupter.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_pin.h"
#include "com/diag/diminuto/diminuto_terminator.h"
#include "com/diag/diminuto/diminuto_timer.h"
#include "com/diag/diminuto/diminuto_types.h"

#include <assert.h>
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <unistd.h>

static const diminuto_ticks_t HERTZ = 1000;

static const int PRIMES[] = {
    7, 5, 3, 2
};

static const char * program = (const char *)0;

int main(int argc, char * argv[])
{
    int xc = 0;
    int pin = -1;
    int duty = 0;
    int prime = 0;
    int index = 0;
    int on = 0;
    int off = 0;
    int rc = 0;
    FILE * fp = (FILE *)0;
    diminuto_sticks_t frequency = 0;
    diminuto_sticks_t ticks = 0;
    int cycle = 0;
    int state = 0;
    float percentage = 0.0;
    float ratio = 0.0;

    /*
     * Process arguments from the command line.
     */

    assert(argc == 3);
  
    program = argv[0]; 
    assert(program != (const char *)0);

    pin = atoi(argv[1]);
    assert(pin >= 0);

    duty = atoi(argv[2]);
    if (duty <= 0) {
        duty = 0;
    } else if (duty >= 100) {
        duty = 100;
    } else {
        /* Do nothing. */
    }

    /*
     * Compute the on and off durations from the duty cycle.
     */

    on = duty;
    off = 100 - duty;

    for (index = 0; index < countof(PRIMES); ++index) {
        prime = PRIMES[index];
        while (((on / prime) > 0) && ((on % prime) == 0) && ((off / prime) > 0) && ((off % prime) == 0)) {
            on /= prime;
            off /= prime;
        }
    }

    if (on > 0) {
        percentage = on;
        percentage /= on + off;
        percentage *= 100;
    }

    if (on == 0) {
        ratio = 100.0;
    } else if (off == 0) {
        ratio = 100.0;
    } else if (on > off) {
        ratio = on;
        ratio /= off;
    } else if (off > on) {
        ratio = off;
        ratio /= on;
    } else {
        ratio = 1.0;
    }

    /*
     * Compute the period in ticks based on the frequency in Hertz.
     */

    frequency = diminuto_frequency();
    assert(frequency > 0);

    ticks = frequency / HERTZ;
    assert(ticks > 0);

    printf("%s: pin=%d duty=%d=%.2f=(%d,%d)=%.2f hertz=%lld\n", program, pin, duty, percentage, on, off, ratio, frequency / ticks);

    /*
     * Initialize the output pin.
     */

    fp = diminuto_pin_output(pin);
    assert(fp != (FILE *)0);

    if (on == 0) {
        rc = diminuto_pin_clear(fp);
        state = 0;
    } else if (off == 0) {
        rc = diminuto_pin_set(fp);
        state = !0;
    } else {
        rc = diminuto_pin_clear(fp);
        state = 0;
    }
    assert(rc >= 0);

    /*
     * Set up the work loop.
     */

    rc = diminuto_alarm_install(!0);
    assert(rc >= 0);

    rc = diminuto_terminator_install(!0);
    assert(rc >= 0);

    rc = diminuto_interrupter_install(!0);
    assert(rc >= 0);

    rc = setpriority(PRIO_PROCESS, 0, -20);
    assert(rc >= 0);

    ticks = diminuto_timer_periodic(ticks);
    assert(ticks >= 0);

    /*
     * Enter the work loop.
     */

    while (!0) {

        /*
         * Wait for a signal.
         */

        rc = pause();
        assert(rc == -1);

        /*
         * Process the signal.
         */

        if (diminuto_terminator_check()) {
            break;
        } else if (diminuto_interrupter_check()) {
            break;
        } else if (!diminuto_alarm_check()) {
            continue;
        } else {
            /* Fall through. */
        }

        /*
         * Modulate the output pin.
         */

        if (cycle > 0) {
            cycle -= 1;
        } else if (state && (off > 0)) {
            state = 0;
            rc = diminuto_pin_clear(fp);
            assert(rc >= 0);
            cycle = off - 1;
        } else if (!state && (on > 0)) {
            state = !0;
            rc = diminuto_pin_set(fp);
            assert(rc >= 0);
            cycle = on - 1;
        } else {
            /* Do nothing. */
        }

    }

    /*
     * Tear down the work loop.
     */

    ticks = diminuto_timer_periodic(0);
    assert(ticks >= 0);

    fp = diminuto_pin_unused(fp, pin);
    assert(fp == (FILE *)0);

    /*
     * Exit.
     */

    return xc;
}
