/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2018 Digital Aggregates Corporation, Colorado USA<BR>
 * Licensed under the terms in LICENSE.txt<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 *
 * SEE ALSO fun/pwmthreads.sh
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
        (void *)mp,
        (void *)(mp->timer),
        mp->initialized,
        mp->pin,
        mp->duty,
        (void *)(mp->fp),
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
    int pin = -1;
    int duty = 0;
    int rc = 0;
    diminuto_modulator_t modulator = { 0 };
    diminuto_sticks_t frequency = 0;
    diminuto_sticks_t ticks = 0;

    /*
     * Process arguments from the command line.
     */

    program = argv[0]; 
    assert(program != (const char *)0);

    if (argc != 3) {
        fprintf(stderr, "usage: %s PIN DUTY\n", program);
        exit(1);
    }

    pin = atoi(argv[1]);
    assert(pin >= 0);

    duty = atoi(argv[2]);
    assert(duty >= 0);

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

    rc = diminuto_modulator_init(&modulator, pin, duty);
    dump(stderr, &modulator);
    assert(rc == 0);
    assert(modulator.pin == pin);
    assert(modulator.fp != (FILE *)0);
    assert(modulator.duty == duty);
    assert((100 % (modulator.ton + modulator.toff)) == 0);

    /*
     * Start the modulator.
     */

    fprintf(stderr, "%s: starting\n", program);

    rc = diminuto_modulator_start(&modulator);
    assert(rc == 0);

    /*
     * Enter the work loop.
     */

    fprintf(stderr, "%s: working\n", program);

    while (!0) {

        if (sleep(1) > 0) {
            fprintf(stderr, "%s: awoke\n", program);
        }

        if (diminuto_terminator_check()) {
            fprintf(stderr, "%s: terminated\n", program);
            break;
        } else if (diminuto_interrupter_check()) {
            fprintf(stderr, "%s: interrupted\n", program);
            break;
        } else {
            continue;
        }

    }

    /*
     * Stop the modulator.
     */

    fprintf(stderr, "%s: stopping\n", program);

    rc = diminuto_modulator_stop(&modulator);
    assert(rc == 0);

    /*
     * Tear down the modulator.
     */

    fprintf(stderr, "%s: finishing\n", program);

    rc = diminuto_modulator_fini(&modulator);
    assert(rc == 0);

    /*
     * Exit.
     */

    fprintf(stderr, "%s: exiting\n", program);

    return xc;
}
