/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2018-2022 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is part of the Modulator functional test.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 *
 * This is part of the Modulator functional test.
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

int main(int argc, char * argv[])
{
    int xc = 0;
    int pin = -1;
    int duty = 0;
    int rc = 0;
    diminuto_modulator_t modulator = { 0 };
    diminuto_modulator_t * mp = (diminuto_modulator_t *)0;

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

    mp = diminuto_modulator_init(&modulator, pin, duty);
    assert(mp == &modulator);;
    assert(modulator.pin == pin);
    assert(modulator.fp != (FILE *)0);
    assert(modulator.duty == duty);
    assert((255 % (modulator.ton + modulator.toff)) == 0);

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

    mp = diminuto_modulator_fini(&modulator);
    assert(mp == (diminuto_modulator_t *)0);

    /*
     * Exit.
     */

    fprintf(stderr, "%s: exiting\n", program);

    return xc;
}
