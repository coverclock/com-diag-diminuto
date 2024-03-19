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
 */

#include "com/diag/diminuto/diminuto_alarm.h"
#include "com/diag/diminuto/diminuto_coherentsection.h"
#include "com/diag/diminuto/diminuto_countof.h"
#include "com/diag/diminuto/diminuto_criticalsection.h"
#include "com/diag/diminuto/diminuto_delay.h"
#include "com/diag/diminuto/diminuto_frequency.h"
#include "com/diag/diminuto/diminuto_interrupter.h"
#include "com/diag/diminuto/diminuto_line.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_modulator.h"
#include "com/diag/diminuto/diminuto_terminator.h"
#include "com/diag/diminuto/diminuto_timer.h"
#include "com/diag/diminuto/diminuto_types.h"
#include <assert.h>
#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <unistd.h>
#include "../fun/hardware_test_fixture.h"

static const char * program = (const char *)0;

int main(int argc, char * argv[])
{
    int xc = 0;
    int pin[4] = {
        /*
         * In order for left to right as they are on the
         * hardware test fixture.
         */
        HARDWARE_TEST_FIXTURE_PIN_PWM_LED,
        HARDWARE_TEST_FIXTURE_PIN_LED_3,
        HARDWARE_TEST_FIXTURE_PIN_LED_2,
        HARDWARE_TEST_FIXTURE_PIN_LED_1,
    };
    int fd[4] = { -1, -1, -1, -1 };
    int duty[4] = { 0 };
    int rc = 0;
    diminuto_modulator_t modulator[4] = { { 0 } };
    diminuto_modulator_t * mp = (diminuto_modulator_t *)0;
    unsigned int seconds = 1;
    int reverse = 0;
    int ii = 0;
    const char * path = (const char *)0;

    /*
     * Process arguments from the command line.
     */

    program = argv[0];
    assert(program != (const char *)0);

    path = hardware_test_fixture_device();
    assert(path != (const char *)0);

    if ((argc >= 2) && (strcmp(argv[1], "-?") == 0)) {
        fprintf(stderr, "usage: %s [ SECONDS [ PIN1 [ PIN2 [ PIN3 [ PIN4 ] ] ] ] ]\n", program);
        exit(1);
    }

    if (argc >= 2) {
        seconds = atoi(argv[1]);
        assert(seconds > 0);
    }

    if (argc >= 3) {
        pin[0] = atoi(argv[2]);
        assert(pin[0] >= 0);
    }

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

    fprintf(stderr, "%s: using \"%s\" %d %d %d %d %d\n", program, path, seconds, pin[0], pin[1], pin[2], pin[3] );

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
    duty[1] = duty[0] + (255 / 4);
    duty[2] = duty[1] + (255 / 4);
    duty[3] = duty[2] + (255 / 4);

    for (ii = 0; ii < countof(pin); ++ii) {
        if (pin[ii] >= 0) {
            fd[ii] = diminuto_line_open_output(path, pin[ii]);
            assert(fd[ii] >= 0);
            fprintf(stderr, "%s: initializing %d %d %d\n", program, pin[ii], duty[ii], fd[ii]);
            mp = diminuto_modulator_init(&(modulator[ii]), &diminuto_modulator_function, &(fd[ii]), duty[ii]);
            assert(mp == &(modulator[ii]));
            assert(modulator[ii].functionp == &diminuto_modulator_function);
            assert(modulator[ii].contextp == &(fd[ii]));
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
                if (duty[0] > 255) {
                    duty[0] = 254;
                    reverse = !0;
                }
            } else {
                duty[0] -= 1;
                if (duty[0] < 0) {
                    duty[0] = 0;
                    break;
                }
            }
            duty[1] = (duty[0] + (255 / 4)) % (255 + 1);
            duty[2] = (duty[1] + (255 / 4)) % (255 + 1);
            duty[3] = (duty[2] + (255 / 4)) % (255 + 1);
            for (ii = 0; ii < countof(pin); ++ii) {
                if (pin[ii] >= 0) {
                    fprintf(stderr, "%s: setting %d %d\n", program, pin[ii], duty[ii]);
                    rc = diminuto_modulator_set(&(modulator[ii]), duty[ii]);
                    assert(rc == 0);
                    assert(modulator[ii].duty == duty[ii]);
                    assert((255 % (modulator[ii].ton + modulator[ii].toff)) == 0);
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
            assert(rc == 0);
        }
    }

    /*
     * There's a race condition (which I'd like to fix) in which the stop()
     * above sees that the timer callback isn't running, but then the timer
     * callback is started asynchronously by the C library timer feature as
     * we destroy the pthread mutexes below in the fini(). The modulator and
     * timer callbacks try to enter their critical sections to check the state
     * and find that their mutxen have been deallocated. This results in a
     * couple of "invalid argument" log messages form the Diminuto mutex lock
     * function. Why don't the timer and modulator callbacks check their
     * states to see if they are DISARMed? Because they have to do that inside
     * a critical section.
     */

    diminuto_delay(diminuto_frequency(), 0);

    /*
     * Tear down the modulator.
     */

    for (ii = 0; ii < countof(pin); ++ii) {
        if (pin[ii] >= 0) {
            fprintf(stderr, "%s: finishing %d %d\n", program, pin[ii], duty[ii]);
            mp = diminuto_modulator_fini(&(modulator[ii]));
            assert(mp == (diminuto_modulator_t *)0);
        }
    }

    /*
     * Exit.
     */

    for (ii = 0; ii < countof(fd); ++ii) {
        if (pin[ii] >= 0) {
            fprintf(stderr, "%s: closing %d %d\n", program, pin[ii], fd[ii]);
            fd[ii] = diminuto_line_close(fd[ii]);
            assert(fd[ii] < 0);
        }
    }

    fprintf(stderr, "%s: exiting\n", program);

    return xc;
}

