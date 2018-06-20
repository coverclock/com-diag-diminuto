/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2018 Digital Aggregates Corporation, Colorado USA<BR>
 * Licensed under the terms in LICENSE.txt<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 *
 * EXAMPLES
 *
 * ABSTRACT
 *
 */

#include "com/diag/diminuto/diminuto_alarm.h"
#include "com/diag/diminuto/diminuto_criticalsection.h"
#include "com/diag/diminuto/diminuto_frequency.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_pin.h"
#include "com/diag/diminuto/diminuto_timer.h"
#include "com/diag/diminuto/diminuto_types.h"

#include <assert.h>
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static const diminuto_ticks_t HERTZ = 100;

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

typedef struct Modulation {
    int pin;
    int duty;
} modulation_t;

static const char * program = (const char *)0;

static void * modulator(void * ap)
{
    void * xc = (void *)1;
    int rc = 0;
    modulation_t * mp = (modulation_t *)0;
    int pin = -1;
    FILE * fp = (FILE *)0;
    diminuto_sticks_t frequency = 0;
    diminuto_sticks_t ticks = 0;
    int cycle = 0;
    int duty = 0;

    mp = (modulation_t *)ap;
    assert(mp != (modulation_t *)0);

    DIMINUTO_CRITICAL_SECTION_BEGIN(&mutex);
        pin = mp->pin;
        duty = mp->duty;
    DIMINUTO_CRITICAL_SECTION_END;

    (void)diminuto_pin_unexport(pin);

    fp = diminuto_pin_output(pin);
    assert(fp != (FILE *)0);

    rc = diminuto_pin_clear(fp);
    assert(rc >= 0);

    rc = diminuto_alarm_install(!0);
    assert(rc >= 0);

    frequency = diminuto_frequency();
    assert(frequency > 0);

    ticks = frequency / HERTZ;
    assert(ticks > 0);
    printf("%s: pin=%d duty=%d hertz=%lld\n", program, pin, duty, frequency / ticks);

    ticks = diminuto_timer_periodic(ticks);
    assert(ticks >= 0);

    while (!0) {

        rc = pause();
        assert(rc == -1);

        if (duty < 0) {
            break;
        } else if (!diminuto_alarm_check()) {
            continue;
        } else if (duty == 0) {
            /* Do nothing. */
        } else if (cycle == 0) {
            rc = diminuto_pin_set(fp);
            assert(rc >= 0);
            printf("%s: pin=%d duty=%d cycle=%d state=%d\n", pin, duty, cycle, !0);
        } else if (duty >= HERTZ) {
            /* Do nothing. */
        } else if (cycle == duty) {
            rc = diminuto_pin_clear(fp);
            assert(rc >= 0);
            printf("%s: pin=%d duty=%d cycle=%d state=%d\n", pin, duty, cycle, 0);
        } else {
            /* Do nothing. */
        }

        cycle = (cycle + 1) % HERTZ;

        DIMINUTO_CRITICAL_SECTION_BEGIN(&mutex);
            duty = mp->duty;
        DIMINUTO_CRITICAL_SECTION_END;

    }

    ticks = diminuto_timer_periodic(0);
    assert(ticks >= 0);

    fp = diminuto_pin_unused(fp, pin);
    assert(fp == (FILE *)0);

    xc = (void *)0;

    return xc;
}

int main(int argc, char * argv[])
{
    void * xc = (void *)0;
    int rc = 0;
    modulation_t modulation = { 0 };
    pthread_t thread;

    assert(argc == 3);
  
    program = argv[0]; 
    modulation.pin = atoi(argv[1]);
    modulation.duty = atoi(argv[2]);

    rc = pthread_create(&thread, 0, modulator, &modulation);
    assert(rc == 0);

    rc = pthread_join(thread, &xc);
    assert(rc == 0);

    return (int)xc;
}
