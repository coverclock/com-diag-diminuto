/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2018-2024 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is part of the Modulator functional test using the ADC.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 *
 * This is part of the Modulator functional test using the ADC.
 *
 * REFERENCES
 *
 * TI, "Ultra-Small, Low-Power, 16-Bit Analog-to-Digital Converter with
 * Internal Reference", (ADS1113, ADS1114, ADS1115), SBAS4448, Texas
 * Instruments, 2009-10
 *
 * Adafruit, "ADS1115 16-Bit ADC - 4 Channel with Programmable Gain
 * Amplifier, P/N 1085, <https://www.adafruit.com/product/1085>
 */

#include "com/diag/diminuto/diminuto_countof.h"
#include "com/diag/diminuto/diminuto_criticalsection.h"
#include "com/diag/diminuto/diminuto_delay.h"
#include "com/diag/diminuto/diminuto_frequency.h"
#include "com/diag/diminuto/diminuto_interrupter.h"
#include "com/diag/diminuto/diminuto_i2c.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_modulator.h"
#include "com/diag/diminuto/diminuto_mux.h"
#include "com/diag/diminuto/diminuto_line.h"
#include "com/diag/diminuto/diminuto_terminator.h"
#include "com/diag/diminuto/diminuto_time.h"
#include "ti/ads1115.h"
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

static const int PWM = HARDWARE_TEST_FIXTURE_PIN_PWM_ADC;
static const int BUS = HARDWARE_TEST_FIXTURE_BUS_I2C;
static const int DEVICE = HARDWARE_TEST_FIXTURE_DEV_I2C_ADC;
static const int INTERRUPT = HARDWARE_TEST_FIXTURE_PIN_INT_ADC;
static const int DUTY = 255;
static const int SUSTAIN = 4;
static const int MEASURE = 5;

int main(int argc, char ** argv) {
    int xc = 0;
    const char * program = (const char *)0;
    int fdi2c = -1;
    int fdisr = -1;
    int fdpwm = -1;
    int rc = -1;
    int pwm = PWM;
    int duty = DUTY;
    int bus = BUS;
    int device = DEVICE;
    int interrupt = INTERRUPT;
    double volts = 0.0;
    diminuto_mux_t mux;
    diminuto_sticks_t ticks = 0;
    diminuto_ticks_t delay = 0;
    diminuto_sticks_t now = 0;
    diminuto_sticks_t was = 0;
    diminuto_ticks_t elapsed = 0;
    int value = 0;
    diminuto_modulator_t modulator = { 0 };
    diminuto_modulator_t * mp = (diminuto_modulator_t *)0;
    int increment = 1;
    int sustain = SUSTAIN;
    int measure = MEASURE;
    const char * path = (const char *)0;

    program = ((program = strrchr(argv[0], '/')) == (char *)0) ? argv[0] : program + 1;
    assert(program != (const char *)0);
    (void)diminuto_line_consumer(program);

    path = hardware_test_fixture_gpio_device();
    assert(path != (const char *)0);

    delay = diminuto_frequency() / 4; /* 250ms > 125ms conversion time. */

    /*
     * I2C light sensor.
     */

    fdi2c = diminuto_i2c_open(bus);
    assert(fdi2c >= 0);

    rc = ti_ads1115_configure_default(fdi2c, device);
    assert(rc >= 0);

    rc = ti_ads1115_print(fdi2c, device, stdout);
    assert(rc >= 0);

    rc = ti_ads1115_sense(fdi2c, device, &volts);
    assert(rc >= 0);

    /*
     * GPIO interrupt pin.
     * GPIO PWM pin.
     */

    fdisr = diminuto_line_open_read(path, interrupt, DIMINUTO_LINE_FLAG_INPUT | DIMINUTO_LINE_FLAG_ACTIVE_LOW | DIMINUTO_LINE_FLAG_EDGE_RISING, 0);
    assert(fdisr >= 0);

    value = diminuto_line_get(fdisr);
    assert(value >= 0);

    fdpwm = diminuto_line_open_output(path, pwm);
    assert(fdpwm >= 0);

    /*
     * Multiplexer.
     */

    diminuto_mux_init(&mux);

    rc = diminuto_mux_register_read(&mux, fdisr);
    assert(rc >= 0);

    /*
     * Pulse width moddulator.
     */

    mp = diminuto_modulator_init(&modulator, &diminuto_modulator_function, &fdpwm, duty);
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

    while (!0) {

        rc = diminuto_mux_wait(&mux, -1);

        if (diminuto_terminator_check()) {
            fprintf(stderr, "%s: terminated\n", program);
            break;
        } else if (diminuto_interrupter_check()) {
            fprintf(stderr, "%s: interrupted\n", program);
            break;
        } else if (rc < 0) {
            break;
        } else if (rc == 0) {
            ticks = diminuto_delay(delay, !0);
            assert(ticks >= 0);
            continue;
        } else {
            /* Do nothing. */
        }

        while ((rc = diminuto_mux_ready_read(&mux)) >= 0) {

            if (rc != fdisr) {
                continue;
            }

            value = diminuto_line_read(fdisr);
            assert(value >= 0);
            if (value == 0) {
                continue;
            }

            rc = ti_ads1115_sense(fdi2c, device, &volts);
            assert(rc >= 0);

            now = diminuto_time_elapsed();
            assert(now >= 0);
            elapsed = (now - was) * 1000 / diminuto_frequency();
            was = now;

            if (modulator.set) {
                continue;
            }

            /*
             * Sustain allows the voltage to stabilize for a bit.
             */

            if (sustain > 1) {
                sustain -= 1;
                continue;
            }

            sustain = SUSTAIN;

            printf("%s: PWM %6.2lf %% on %3u off %3u ADC %7.3f v Period %3lld ms\n", program, 100.0 * (double)duty / (double)DIMINUTO_MODULATOR_DUTY_MAX, modulator.on, modulator.off, volts, (long long int)elapsed);

            /*
             * Measure allows us to read the voltage more than once.
             */

            if (measure > 1) {
                measure -= 1;
                continue;
            }

            measure = MEASURE;

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

    rc = diminuto_line_clear(fdpwm);
    assert(rc >= 0);
    fdpwm = diminuto_line_close(fdpwm);
    assert(fdpwm < 0);

    fdi2c = diminuto_i2c_close(fdi2c);
    assert(fdi2c < 0);

    printf("%s: exiting\n", program);

    return xc;
}

