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
#include "com/diag/diminuto/diminuto_coherentsection.h"
#include "com/diag/diminuto/diminuto_countof.h"
#include "com/diag/diminuto/diminuto_criticalsection.h"
#include "com/diag/diminuto/diminuto_frequency.h"
#include "com/diag/diminuto/diminuto_interrupter.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_modulator.h"
#include "com/diag/diminuto/diminuto_pin.h"
#include "com/diag/diminuto/diminuto_terminator.h"
#include "com/diag/diminuto/diminuto_timer.h"
#include "com/diag/diminuto/diminuto_types.h"

#include <assert.h>
#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <unistd.h>

static const diminuto_ticks_t HERTZ = 10000; /* 100us */

static const char * program = (const char *)0;

int main(int argc, char * argv[])
{
    int xc = 0;
    int pin = -1;
    int duty = 0;
    int rc = 0;
    diminuto_modulator_t modulator = { 0 };
    diminuto_sticks_t frequency = 0;
    diminuto_sticks_t ticks = 0;
    unsigned int seconds = 0;

    /*
     * Process arguments from the command line.
     */

    assert(argc == 3);
  
    program = argv[0]; 
    assert(program != (const char *)0);

    pin = atoi(argv[1]);
    assert(pin >= 0);

    seconds = atoi(argv[2]);
    assert(seconds > 0);

    /*
     * Install signal handlers.
     */

    rc = diminuto_alarm_install(!0);
    assert(rc >= 0);

    rc = diminuto_terminator_install(!0);
    assert(rc >= 0);

    rc = diminuto_interrupter_install(!0);
    assert(rc >= 0);

    /*
     * Initialize the modulator.
     */

    rc = diminuto_modulator_init(&modulator, pin, duty);

    /*
     * Start up the periodic timer.
     */

    frequency = diminuto_frequency();
    assert(frequency > 0);

    ticks = frequency / HERTZ;
    assert(ticks > 0);

    ticks = diminuto_timer_periodic(ticks);
    assert(ticks >= 0);

    /*
     * Start the modulator.
     */

    rc = diminuto_modulator_start(&modulator);
    assert(rc == 0);

    /*
     * Enter the work loop.
     */

    for (duty = 1; duty <= 100; ++duty) {

        sleep(seconds);

        if (diminuto_terminator_check()) {
            break;
        } else if (diminuto_interrupter_check()) {
            break;
        } else {
            /* Fall through. */
        }

        diminuto_modulator_set(&modulator, duty);

        printf("%s: state='%c' pin=%d fp=%p duty=%d on=%d off=%d\n", program, modulator.state, modulator.pin, modulator.fp, modulator.duty, modulator.on, modulator.off);

        assert(rc == 0);
        assert(modulator.pin == pin);
        assert(modulator.fp != (FILE *)0);
        assert(modulator.state == DIMINUTO_MODULATOR_RUNNING);
        assert(modulator.duty == duty);
        assert((100 % (modulator.on + modulator.off)) == 0);

    }

    /*
     * Stop the modulator.
     */

    rc = diminuto_modulator_stop(&modulator);
    assert(rc == 0);

    /*
     * Tear down the modulator and stop the timer.
     */

    rc = diminuto_modulator_fini(&modulator);
    assert(rc == 0);

    ticks = diminuto_timer_periodic(0);
    assert(ticks >= 0);

    /*
     * Exit.
     */

    return xc;
}
