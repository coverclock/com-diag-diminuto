/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2018 Digital Aggregates Corporation, Colorado USA<BR>
 * Licensed under the terms in LICENSE.txt<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
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

static const char * program = (const char *)0;

int main(int argc, char * argv[])
{
    int xc = 0;
    int pin[4] = { -1, -1, -1, -1 };
    int duty[4] = { 0 };
    int rc = 0;
    diminuto_modulator_t modulator[4] = { { 0 } };
    diminuto_sticks_t frequency = 0;
    diminuto_sticks_t ticks = 0;
    unsigned int seconds = 0;
    int reverse = 0;
    int ii = 0;

    /*
     * Process arguments from the command line.
     */

    program = argv[0];
    assert(program != (const char *)0);

    if (argc < 3) {
    	fprintf(stderr, "usage: %s SECONDS PIN1 [ PIN2 [ PIN3 [ PIN4 ] ] ]\n", program);
    	exit(1);
    }

    seconds = atoi(argv[1]);
    assert(seconds > 0);

    pin[0] = atoi(argv[2]);
    assert(pin[0] >= 0);

    if (argc >= 4) {
        pin[1] = atoi(argv[3]);
        assert(pin[1] >= 0);
    }

    if (argc >= 5) {
        pin[2] = atoi(argv[4]);
        assert(pin[2] >= 0);
    }

    if (argc >= 6) {
        pin[3] = atoi(argv[5]);
        assert(pin[3] >= 0);
    }

    fprintf(stderr, "%s: using %d %d %d %d %d\n", program, seconds, pin[0], pin[1], pin[2], pin[3] );

    /*
     * Install signal handlers.
     */

    rc = diminuto_terminator_install(!0);
    assert(rc >= 0);

    rc = diminuto_interrupter_install(!0);
    assert(rc >= 0);

    /*
     * Initialize the modulator.
     */

    duty[0] = 0;
    duty[1] = duty[0] + 33;
    duty[2] = duty[1] + 33;
    duty[3] = duty[2] + 33;

    for (ii = 0; ii < countof(pin); ++ii) {
    	if (pin[ii] >= 0) {
    	    fprintf(stderr, "%s: initializing %d %d\n", program, pin[ii], duty[ii]);
    		rc = diminuto_modulator_init(&(modulator[ii]), pin[ii], duty[ii]);
    		diminuto_modulator_print(stderr, &(modulator[ii]));
    		assert(rc == 0);
    		assert(modulator[ii].pin == pin[ii]);
    		assert(modulator[ii].fp != (FILE *)0);
    		assert(modulator[ii].duty == duty[ii]);
    	}
    }

    /*
     * Start the modulator.
     */

    for (ii = 0; ii < countof(pin); ++ii) {
    	if (pin[ii] >= 0) {
    	    fprintf(stderr, "%s: starting %d %d\n", program, pin[ii], duty[ii]);
    		rc = diminuto_modulator_start(&(modulator[ii]));
    		diminuto_modulator_print(stderr, &(modulator[ii]));
    		assert(rc == 0);
    	}
    }

    /*
     * Enter the work loop.
     */

    fprintf(stderr, "%s: working\n", program);

    while (!0) {

        if (sleep(seconds) > 0) {
            fprintf(stderr, "%s: awoke\n", program);
        }

        if (diminuto_terminator_check()) {
            fprintf(stderr, "%s: terminated\n", program);
            break;
        } else if (diminuto_interrupter_check()) {
            fprintf(stderr, "%s: interrupted\n", program);
            break;
        } else {
            if (!reverse) {
                duty[0] += 1;
                if (duty[0] > 100) {
                    duty[0] = 99;
                    reverse = !0;
                }
            } else {
                duty[0] -= 1;
                if (duty[0] < 0) {
                    duty[0] = 0;
                    break;
                }
            }
            duty[1] = (duty[0] + 33) % 101;
            duty[2] = (duty[1] + 33) % 101;
            duty[3] = (duty[2] + 33) % 101;
            for (ii = 0; ii < countof(pin); ++ii) {
            	if (pin[ii] >= 0) {
                    fprintf(stderr, "%s: setting %d %d\n", program, pin[ii], duty[ii]);
            		rc = diminuto_modulator_set(&(modulator[ii]), duty[ii]);
                    assert(rc == 0);
            		diminuto_modulator_print(stderr, &(modulator[ii]));
            		assert(modulator[ii].duty == duty[ii]);
            		assert((100 % (modulator[ii].ton + modulator[ii].toff)) == 0);
            	}
            }
            continue;
        }

    }

    /*
     * Stop the modulator.
     */

    for (ii = 0; ii < countof(pin); ++ii) {
    	if (pin[ii] >= 0) {
            fprintf(stderr, "%s: stopping %d %d\n", program, pin[ii], duty[ii]);
    		rc = diminuto_modulator_stop(&(modulator[ii]));
    		diminuto_modulator_print(stderr, &(modulator[ii]));
    		assert(rc == 0);
    	}
    }

    /*
     * Tear down the modulator.
     */

    for (ii = 0; ii < countof(pin); ++ii) {
    	if (pin[ii] >= 0) {
            fprintf(stderr, "%s: finishing %d %d\n", program, pin[ii], duty[ii]);
   		rc = diminuto_modulator_fini(&(modulator[ii]));
    		diminuto_modulator_print(stderr, &(modulator[ii]));
    		assert(rc == 0);
    	}
    }

    /*
     * Exit.
     */

    fprintf(stderr, "%s: exiting\n", program);

    return xc;
}

