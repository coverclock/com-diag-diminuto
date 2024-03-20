/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2018-2024 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is part of the Modulator functional test using the lux sensor.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 *
 * This is part of the Modulator functional test using the lux sensor.
 *
 * REFERENCES
 *
 * Avago, "APDS-9301 Miniature Ambient Light Photo Sensor with Digital
 * (I2C) Output", Avago Technologies, AV02-2315EN, 2010-01-07
 *
 * Wikipedia, "Lux", 2018-03-17
 *
 * Sparkfun, "SparkFun Ambient Light Sensor Breakout - APDS-9301",
 * SEN-14350, <https://www.sparkfun.com/products/14350>
 */

#include "com/diag/diminuto/diminuto_countof.h"
#include "com/diag/diminuto/diminuto_criticalsection.h"
#include "com/diag/diminuto/diminuto_delay.h"
#include "com/diag/diminuto/diminuto_frequency.h"
#include "com/diag/diminuto/diminuto_interrupter.h"
#include "com/diag/diminuto/diminuto_i2c.h"
#include "com/diag/diminuto/diminuto_line.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_modulator.h"
#include "com/diag/diminuto/diminuto_mux.h"
#include "com/diag/diminuto/diminuto_terminator.h"
#include "com/diag/diminuto/diminuto_time.h"
#include "avago/apds9301.h"
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "../fun/hardware_test_fixture.h"

static const int LED = HARDWARE_TEST_FIXTURE_PIN_PWM_LED;
static const int BUS = HARDWARE_TEST_FIXTURE_BUS_I2C;
static const int DEVICE = HARDWARE_TEST_FIXTURE_DEV_I2C_LUX;
static const int INTERRUPT = HARDWARE_TEST_FIXTURE_PIN_INT_LUX;
static const int DUTY = 255;
static const int SUSTAIN = 3;

int main(int argc, char ** argv) {
    int xc = 0;
    const char * program = (const char *)0;
    int fdi2c = -1;
    int fdisr = -1;
    int fdled = -1;
    int rc = -1;
    int led = LED;
    int duty = DUTY;
    int bus = BUS;
    int device = DEVICE;
    int interrupt = INTERRUPT;
    double lux = 0.0;
    diminuto_mux_t mux;
    diminuto_ticks_t delay = 0;
    diminuto_sticks_t now = 0;
    diminuto_sticks_t was = 0;
    diminuto_ticks_t elapsed = 0;
    int value = 0;
    diminuto_modulator_t modulator = { 0 };
    diminuto_modulator_t * mp = (diminuto_modulator_t *)0;
    int increment = 1;
    int sustain = SUSTAIN;
    const char * path = (const char *)0;

    program = ((program = strrchr(argv[0], '/')) == (char *)0) ? argv[0] : program + 1;
    assert(program != (const char *)0);
    (void)diminuto_line_consumer(program);

    delay = diminuto_frequency() / 2; /* 500ms > 400ms integration time. */

    path = hardware_test_fixture_gpio_device();
    assert(path != (const char *)0);

    /*
     * I2C light sensor.
     */

    fdi2c = diminuto_i2c_open(bus);
    assert(fdi2c >= 0);

    rc = avago_apds9301_reset(fdi2c, device); 
    assert(rc >= 0);

    rc = avago_apds9301_print(fdi2c, device, stdout);
    assert(rc >= 0);

    rc = avago_apds9301_configure_default(fdi2c, device);
    assert(rc >= 0);

    /*
     * GPIO interrupt pin.
     * GPIO LED pin.
     */

    fdisr = diminuto_line_open_read(path, interrupt, DIMINUTO_LINE_FLAG_INPUT | DIMINUTO_LINE_FLAG_EDGE_RISING, 0);
    assert(fdisr >= 0);

    fdled = diminuto_line_open_output(path, led);
    assert(fdled >= 0);

    /*
     * Multiplexer.
     */

    diminuto_mux_init(&mux);

    rc = diminuto_mux_register_read(&mux, fdisr);
    assert(rc >= 0);

    /*
     * Pulse width moddulator.
     */

    mp = diminuto_modulator_init(&modulator, &diminuto_modulator_function, &fdled, duty);
    assert(mp == &modulator);

    /*
     * Signal handlers.
     */

    rc = diminuto_terminator_install(!0);
    assert(rc >= 0);

    rc = diminuto_interrupter_install(!0);
    assert(rc >= 0);

    /*
     * Work loop.
     */

    rc = diminuto_modulator_start(&modulator);
    assert(rc >= 0);

    was = diminuto_time_elapsed();
    assert(was >= 0);

    rc = avago_apds9301_sense(fdi2c, device, &lux);
    assert(rc >= 0);

    assert((AVAGO_APDS9301_LUX_MINIMUM <= lux) && (lux <= AVAGO_APDS9301_LUX_MAXIMUM));

    while (!0) {

        rc = diminuto_mux_wait(&mux, delay);

        if (diminuto_terminator_check()) {
            fprintf(stderr, "%s: terminated\n", program);
            break;
        } else if (diminuto_interrupter_check()) {
            fprintf(stderr, "%s: interrupted\n", program);
            break;
        } else if (rc < 0) {
            break;
        } else if (rc == 0) {
            continue;
        } else {
            /* Do nothing. */
        }

        while ((rc = diminuto_mux_ready_read(&mux)) >= 0) {

            if (rc != fdisr) {
                continue;
            }

            value = diminuto_line_read(fdisr);
            if (value == 0) {
                continue;
            }

            rc = avago_apds9301_sense(fdi2c, device, &lux);
            assert(rc >= 0);

            assert((AVAGO_APDS9301_LUX_MINIMUM <= lux) && (lux <= AVAGO_APDS9301_LUX_MAXIMUM));

            now = diminuto_time_elapsed();
            assert(now >= 0);
            elapsed = (now - was) * 1000 / diminuto_frequency();
            was = now;

            if (modulator.set) {
                continue;
            }

            if (sustain > 0) {
                sustain -= 1;
                continue;
            }

            sustain = SUSTAIN;

            printf("%s: PWM %6.2lf %% on %3u off %3u Lux %7.2f lx Period %3lld ms\n", program, 100.0 * (double)duty / (double)DIMINUTO_MODULATOR_DUTY_MAX, modulator.on, modulator.off, lux, (long long int)elapsed);

            duty += increment;
            if (duty > 255) {
                duty = 254;
                increment = -1;
            } else if (duty < 0) {
                break;
            } else {
                /* Do nothing. */
            }

            rc = diminuto_modulator_set(&modulator, duty);
            assert(rc >= 0);

        }

        if (duty < 0) {
            break;
        }

    }

    /*
     * Done.
     */

    rc = diminuto_modulator_stop(&modulator);
    assert(rc >= 0);

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

    mp = diminuto_modulator_fini(&modulator);
    assert(mp == (diminuto_modulator_t *)0);

    rc = diminuto_mux_unregister_read(&mux, fdisr);
    assert(rc >= 0);

    diminuto_mux_fini(&mux);

    fdisr = diminuto_line_close(fdisr);
    assert(fdisr < 0);

    rc = diminuto_line_clear(fdled);
    assert(rc >= 0);
    fdled = diminuto_line_close(fdled);
    assert(fdled < 0);

    fdi2c = diminuto_i2c_close(fdi2c);
    assert(fdi2c < 0);

    printf("%s: exiting\n", program);

    return xc;
}
