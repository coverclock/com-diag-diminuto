/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2018-2024 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is part of the Modulator functional test.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 *
 * This is part of the Modulator functional test.
 *
 * SEE ALSO fun/pwmtests.sh
 */

#include "com/diag/diminuto/diminuto_alarm.h"
#include "com/diag/diminuto/diminuto_countof.h"
#include "com/diag/diminuto/diminuto_criticalsection.h"
#include "com/diag/diminuto/diminuto_frequency.h"
#include "com/diag/diminuto/diminuto_interrupter.h"
#include "com/diag/diminuto/diminuto_line.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_terminator.h"
#include "com/diag/diminuto/diminuto_timer.h"
#include "com/diag/diminuto/diminuto_types.h"
#include <assert.h>
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <unistd.h>
#include "../fun/hardware_test_fixture.h"

static const diminuto_ticks_t HERTZ = 10000; /* 100us */

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
    int fd = -1;
    diminuto_sticks_t frequency = 0;
    diminuto_sticks_t ticks = 0;
    int cycle = 0;
    int state = 0;
    float percentage = 0.0;
    float ratio = 0.0;
    const char * path = (const char *)0;

    /*
     * Process arguments from the command line.
     */
  
    program = ((program = strrchr(argv[0], '/')) == (char *)0) ? argv[0] : program + 1;
    assert(program != (const char *)0);
    (void)diminuto_line_consumer(program);

    path = hardware_test_fixture_gpio_device();
    assert(path != (const char *)0);

    if (argc != 3) {
        fprintf(stderr, "usage: %s PIN DUTY\n", program);
        exit(1);
    }

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

    assert((100 % (on + off)) == 0);

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

    printf("%s: pin=%d duty=%d=%.2f=(%d,%d)=%.2f hertz=%lld\n", program, pin, duty, percentage, on, off, ratio, (long long int)(frequency / ticks));

    /*
     * Initialize the output pin.
     */

    fd = diminuto_line_open_output(path, pin);
    assert(fd >= 0);

    if (on == 0) {
        rc = diminuto_line_clear(fd);
        state = 0;
    } else if (off == 0) {
        rc = diminuto_line_set(fd);
        state = !0;
    } else {
        rc = diminuto_line_clear(fd);
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

#if 0
    rc = setpriority(PRIO_PROCESS, 0, -20);
    assert(rc >= 0);
#endif

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
            rc = diminuto_line_clear(fd);
            assert(rc >= 0);
            cycle = off - 1;
        } else if (!state && (on > 0)) {
            state = !0;
            rc = diminuto_line_set(fd);
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

    rc = diminuto_line_clear(fd);
    assert(rc >= 0);
    fd = diminuto_line_close(fd);
    assert(fd < 0);

    /*
     * Exit.
     */

    return xc;
}
