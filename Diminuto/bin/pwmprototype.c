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

static const char * program = (const char *)0;

int main(int argc, char * argv[])
{
    int xc = 0;
    int pin = -1;
    int duty = 0;
    int rc = 0;
    FILE * fp = (FILE *)0;
    diminuto_sticks_t frequency = 0;
    diminuto_sticks_t ticks = 0;
    int cycle = 0;
    int was = 0;
    int now = 0;

    assert(argc == 3);
  
    program = argv[0]; 
    pin = atoi(argv[1]);
    duty = atoi(argv[2]);

    fp = diminuto_pin_output(pin);
    assert(fp != (FILE *)0);

    rc = diminuto_pin_clear(fp);
    assert(rc >= 0);

    rc = diminuto_alarm_install(!0);
    assert(rc >= 0);

    rc = diminuto_terminator_install(!0);
    assert(rc >= 0);

    rc = diminuto_interrupter_install(!0);
    assert(rc >= 0);

    frequency = diminuto_frequency();
    assert(frequency > 0);

    ticks = frequency / HERTZ;
    assert(ticks > 0);
    printf("%s: pin=%d duty=%d hertz=%lld\n", program, pin, duty, frequency / ticks);

    rc = setpriority(PRIO_PROCESS, 0, -20);
    assert(rc >= 0);

    ticks = diminuto_timer_periodic(ticks);
    assert(ticks >= 0);

    while (!0) {

        rc = pause();
        assert(rc == -1);

        if (diminuto_terminator_check()) {
            break;
        } else if (diminuto_interrupter_check()) {
            break;
        } else if (!diminuto_alarm_check()) {
            continue;
        } else {
            now = (duty > 0) && ((cycle % duty) == 0);
            if (was != now) {
                rc = diminuto_pin_put(fp, now);
                assert(rc >= 0);
                was = now;
            }
        }

        cycle = (cycle + 1) % HERTZ;

    }

    ticks = diminuto_timer_periodic(0);
    assert(ticks >= 0);

    fp = diminuto_pin_unused(fp, pin);
    assert(fp == (FILE *)0);

    return xc;
}
