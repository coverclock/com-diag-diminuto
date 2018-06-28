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

static const char * program = (const char *)0;

void dump(FILE * fp, const diminuto_modulator_t * mp)
{
    fprintf(stderr, "modulator@%p: timer=%p initialized=%d pin=%d duty=%d fp=%p on=%d off=%d set=%d total=%d cycle=%d ton=%d toff=%d condition=%d\n",
    	mp,
		(void *)(mp->timer),
		mp->initialized,
		mp->pin,
		mp->duty,
		mp->fp,
		mp->on,
		mp->off,
		mp->set,
		mp->total,
		mp->cycle,
		mp->ton,
		mp->toff,
		mp->condition
	);
}

int main(int argc, char * argv[])
{
    int xc = 0;
    int pin[3] = { -1, -1, -1 };
    int duty[3] = { 0 };
    int rc = 0;
    diminuto_modulator_t modulator[3] = { { 0 } };
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

    if (argc == 1) {
    	fprintf(stderr, "usage: %s SECONDS PIN1 [ PIN2 [ PIN3 ] ]\n", program);
    	exit(1);
    }

    assert(argc >= 3);

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

    fprintf(stderr, "%s: %d %d %d %d\n", program, seconds, pin[0], pin[1], pin[2]);

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

    fprintf(stderr, "%s: initializing\n", program);

    duty[0] = 0;
    duty[1] = (duty[0] + 33) % 101;
    duty[2] = (duty[1] + 33) % 101;

    for (ii = 0; ii < countof(pin); ++ii) {
    	if (pin[ii] >= 0) {
    		rc = diminuto_modulator_init(&(modulator[ii]), pin[ii], duty[ii]);
    		dump(stderr, &(modulator[ii]));
    		assert(rc == 0);
    		assert(modulator[ii].pin == pin[ii]);
    		assert(modulator[ii].fp != (FILE *)0);
    		assert(modulator[ii].duty == duty[ii]);
    	}
    }

    /*
     * Start the modulator.
     */

    fprintf(stderr, "%s: starting\n", program);

    for (ii = 0; ii < countof(pin); ++ii) {
    	if (pin[ii] >= 0) {
    		rc = diminuto_modulator_start(&(modulator[ii]));
    		dump(stderr, &(modulator[ii]));
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
                    break;
                }
            }
            duty[1] = (duty[0] + 33) % 101;
            duty[2] = (duty[1] + 33) % 101;
            for (ii = 0; ii < countof(pin); ++ii) {
            	if (pin[ii] >= 0) {
            		diminuto_modulator_set(&(modulator[ii]), duty[ii]);
            		dump(stderr, &(modulator[ii]));
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

    fprintf(stderr, "%s: stopping\n", program);

    for (ii = 0; ii < countof(pin); ++ii) {
    	if (pin[ii] >= 0) {
    		rc = diminuto_modulator_stop(&(modulator[ii]));
    		dump(stderr, &(modulator[ii]));
    		assert(rc == 0);
    	}
    }

    /*
     * Tear down the modulator.
     */

    fprintf(stderr, "%s: finishing\n", program);

    for (ii = 0; ii < countof(pin); ++ii) {
    	if (pin[ii] >= 0) {
    		rc = diminuto_modulator_fini(&(modulator[ii]));
    		dump(stderr, &(modulator[ii]));
    		assert(rc == 0);
    	}
    }

    /*
     * Exit.
     */

    fprintf(stderr, "%s: exiting\n", program);

    return xc;
}

