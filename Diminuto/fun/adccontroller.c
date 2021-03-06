/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2018-2020 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is part of the Modulator and Controller functional test using the ADC.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 *
 * ABSTRACT
 *
 * This is a functional test of the PID controller, using the PWM
 * modulator to control the voltage on a pin, and an analog-to-digital
 * converter to implement a feedback loop. The parameters work for the
 * hardware text fixture I use to exercise a number of Diminuto functional
 * tests.
 *
 * The PWM modulator duty cycle ranges from 0 (off) to 255 (on).
 *
 * The ADC output ranges functionally from approximately -6.14V to 6.14V,
 * and emperically with this test fixture from approximately -0.003V to
 * 3.289V (nominally 3.3V).
 *
 * USAGE
 *
 * adccontroller [ -d ] [ ADCTARGET [ DUTYCYCLE ] ]
 *
 * EXAMPLE
 *
 * adccontroller 1500 10
 *
 * REFERENCES
 *
 * TI, "Ultra-Small, Low-Power, 16-Bit Analog-to-Digital Converter with
 * Internal Reference", (ADS1113, ADS1114, ADS1115), SBAS4448, Texas
 * Instruments, 2009-10
 *
 * Adafruit, "ADS1115 16-Bit ADC - 4 Channel with Programmable Gain
 * Amplifier, P/N 1085, <https://www.adafruit.com/product/1085>
 *
 * QUOTE
 *
 * "Test what you fly, fly what you test." - NASA aphorism
 */

#include "com/diag/diminuto/diminuto_controller.h"
#include "com/diag/diminuto/diminuto_criticalsection.h"
#include "com/diag/diminuto/diminuto_delay.h"
#include "com/diag/diminuto/diminuto_frequency.h"
#include "com/diag/diminuto/diminuto_interrupter.h"
#include "com/diag/diminuto/diminuto_i2c.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_modulator.h"
#include "com/diag/diminuto/diminuto_mux.h"
#include "com/diag/diminuto/diminuto_pin.h"
#include "com/diag/diminuto/diminuto_terminator.h"
#include "com/diag/diminuto/diminuto_time.h"
#include "hardware_test_fixture.h"
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

/*
 * These parameters control the setttling time of the software PWM control
 * of the ADC (INPUT), and how often we print headers on the report (OUTPUT).
 */

static const int INPUT_MODULO = 8;
static const int OUTPUT_MODULO = 24;

/*
 * These parameters describe the configuration of the hardware test fixture
 * that I built.
 */

static const int INPUT_I2C_BUS = HARDWARE_TEST_FIXTURE_BUS_I2C;
static const int INPUT_I2C_DEVICE = HARDWARE_TEST_FIXTURE_DEV_I2C_ADC;
static const int INPUT_GPIO_PIN = HARDWARE_TEST_FIXTURE_PIN_INT_ADC;
static const int OUTPUT_GPIO_PIN = HARDWARE_TEST_FIXTURE_PIN_PWM_ADC;

/*
 * These parameters tune the PID controller. Choosing good values are in
 * my personal experience a bit of an art and require some experimentation.
 */

static const diminuto_controller_value_t PID_INPUT_WINDUP = 3300;
static const diminuto_controller_output_t PID_OUTPUT_MINIMUM = 0;
static const diminuto_controller_output_t PID_OUTPUT_MAXIMUM = 255;
static const diminuto_controller_output_t PID_OUTPUT_LOWER = 0;
static const diminuto_controller_output_t PID_OUTPUT_UPPER = 255;
static const diminuto_controller_factor_t PID_KP_NUMERATOR = 1;
static const diminuto_controller_factor_t PID_KP_DENOMINATOR = 16;
static const diminuto_controller_factor_t PID_KI_NUMERATOR = 1;
static const diminuto_controller_factor_t PID_KI_DENOMINATOR = 16;
static const diminuto_controller_factor_t PID_KD_NUMERATOR = 1;
static const diminuto_controller_factor_t PID_KD_DENOMINATOR = 16;
static const diminuto_controller_factor_t PID_KC_NUMERATOR = 1;
static const diminuto_controller_factor_t PID_KC_DENOMINATOR = 3300 / 256;
static const bool PID_FILTER = 0;

/*
 * These are the beginning PWM duty cycle (OUTPUT) [0..255],
 * and the default ADC target value (INPUT) in millivolts [-6144..6144].
 */

static const diminuto_controller_output_t PWM_DUTY = 127;
static const diminuto_controller_input_t TARGET_MILLIVOLTS = 2475;
static const bool DEBUG = 0;

/*
 * These are the maximum allowable values for the input parameters.
 * (Neither is allowed to be negative.)
 */

static const diminuto_controller_output_t MAXIMUM_DUTY = 255;
static const diminuto_controller_input_t MAXIMUM_MILLIVOLTS = 6144;

int main(int argc, char ** argv) {
    int xc = 0;
    const char * program = (const char *)0;
    int fd = -1;
    int rc = -1;
    int led = OUTPUT_GPIO_PIN;
    int bus = INPUT_I2C_BUS;
    int device = INPUT_I2C_DEVICE;
    int interrupt = INPUT_GPIO_PIN;
    FILE * fp = (FILE *)0;
    double adc = 0.0;
    diminuto_mux_t mux;
    diminuto_sticks_t ticks = 0;
    diminuto_ticks_t delay = 0;
    diminuto_sticks_t now = 0;
    diminuto_sticks_t was = 0;
    diminuto_ticks_t elapsed = 0;
    int bit = 0;
    diminuto_modulator_t modulator = { 0 };
    diminuto_modulator_t * mp = (diminuto_modulator_t *)0;
    diminuto_controller_parameters_t parameters = { 0 };
    diminuto_controller_state_t state = { 0 };
    int increment = 1;
    diminuto_controller_input_t target = 0;
    diminuto_controller_input_t input = 0;
    diminuto_controller_output_t output = 0;
    diminuto_controller_output_t prime = 0;
    char * end = (char *)0;
    int inputs = 0;
    int outputs = 0;
    bool debug = 0;

    delay = diminuto_frequency() / 2; /* 500ms > 400ms integration time. */

    /*
     * Command line arguments.
     */

    if (argc < 1) {
        program = "adccontroller";
    } else {
        program = ((program = strrchr(*argv, '/')) == (char *)0) ? *argv : program + 1;
        argv += 1;
        argc -= 1;
    }

    if (argc < 1) {
        /* Do nothing. */
    } else if (strncmp(*argv, "-?", sizeof("-?")) == 0) {
        fprintf(stderr, "usage: %s [ -? | [ -d ] [ MILLIVOLTS [ DUTYCYCLE ] ] ]\n", program);
        exit(0);
    }

    if (argc < 1) {
        debug = DEBUG;
    } else if (strncmp(*argv, "-d", sizeof("-d")) == 0) {
        debug = !0;
        argv += 1;
        argc -= 1;
    }

    if (argc < 1) {
        target = TARGET_MILLIVOLTS;
    } else {
        end = (char *)0;
        target = strtoul(*argv, &end, 0);
        if ((end == (char *)0) || (*end != '\0') || (target > MAXIMUM_MILLIVOLTS)) {
            errno = EINVAL;
            diminuto_perror(*argv);
            exit(1);
        }
        argv += 1;
        argc -= 1;
    }

    if (argc < 1) {
        output = PWM_DUTY;
    } else {
        end = (char *)0;
        output = strtoul(*argv, &end, 0);
        if ((end == (char *)0) || (*end != '\0') || (output > MAXIMUM_DUTY)) {
            errno = EINVAL;
            diminuto_perror(*argv);
            exit(1);
        }
        argv += 1;
        argc -= 1;
    }

    /*
     * I2C light sensor.
     */

    fd = diminuto_i2c_open(bus);
    assert(fd >= 0);

    rc = ti_ads1115_configure_default(fd, device);
    assert(rc >= 0);

    if (debug) {
        rc = ti_ads1115_print(fd, device, stderr);
        assert(rc >= 0);
    }

    /*
     * GPIO interrupt pin.
     */

    (void)diminuto_pin_unexport_ignore(interrupt); 

    fp = diminuto_pin_input(interrupt);
    assert(fp != (FILE *)0);

    rc = diminuto_pin_active(interrupt, 0);
    assert(rc == 0);

    rc = diminuto_pin_edge(interrupt, DIMINUTO_PIN_EDGE_RISING);
    assert(rc == 0);

    /*
     * Multiplexer.
     */

    diminuto_mux_init(&mux);

    rc = diminuto_mux_register_interrupt(&mux, fileno(fp));
    assert(rc >= 0);

    /*
     * Proporational integral derivative controller.
     */

    diminuto_controller_init(&parameters, &state);

    parameters.windup = PID_INPUT_WINDUP;
    parameters.minimum = PID_OUTPUT_MINIMUM;
    parameters.maximum = PID_OUTPUT_MAXIMUM;
    parameters.lower = PID_OUTPUT_LOWER;
    parameters.upper = PID_OUTPUT_UPPER;
    parameters.kp.numerator = PID_KP_NUMERATOR;
    parameters.kp.denominator = PID_KP_DENOMINATOR;
    parameters.ki.numerator = PID_KI_NUMERATOR;
    parameters.ki.numerator = PID_KI_NUMERATOR;
    parameters.kd.numerator = PID_KD_NUMERATOR;
    parameters.kd.denominator = PID_KD_DENOMINATOR;
    parameters.kc.denominator = PID_KC_DENOMINATOR;
    parameters.kc.denominator = PID_KC_DENOMINATOR;
    parameters.filter = PID_FILTER;

    if (debug) {
        fprintf(stderr, "%s: ", program);
        diminuto_controller_parameters_print(stderr, &parameters);
        fprintf(stderr, "%s: ", program);
        diminuto_controller_state_print(stderr, &state);
    }


    /*
     * Pulse width modulator.
     */

    mp = diminuto_modulator_init(&modulator, led, output);
    assert(mp == &modulator);

    /*
     * Signal handlers.
     */

    rc = diminuto_terminator_install(!0);
    assert(rc >= 0);

    rc = diminuto_interrupter_install(!0);
    assert(rc >= 0);

    /*
     * Start.
     */

    rc = diminuto_modulator_start(&modulator);
    assert(rc >= 0);

    /*
     * Retire pending.
     */

    bit = diminuto_pin_get(fp);
    assert(bit >= 0);

    rc = ti_ads1115_sense(fd, device, &adc);
    assert(rc >= 0);

    /*
     * Work loop.
     */

    was = diminuto_time_elapsed();
    assert(was >= 0);

    while (!0) {

        rc = diminuto_mux_wait(&mux, delay);

        if (diminuto_terminator_check()) {
            fprintf(stderr, "%s: terminated\n", program);
            break;
        } else if (diminuto_interrupter_check()) {
            fprintf(stderr, "%s: interrupted\n", program);
            break;
        } else if (rc > 0) {
            /* Do nothing. */
        } else if (rc == 0) {
            fprintf(stderr, "%s: timeout\n", program);
            continue;
        } else {
            fprintf(stderr, "%s: failed\n", program);
            break;
        }

        while ((rc = diminuto_mux_ready_interrupt(&mux)) >= 0) {

            if (rc != fileno(fp)) {
                continue;
            }

            bit = diminuto_pin_get(fp);
            assert(bit >= 0);

            if (bit == 0) {
                continue;
            }

            rc = ti_ads1115_sense(fd, device, &adc);
            assert(rc >= 0);

            input = (adc + 0.5) * 1000.0;

            now = diminuto_time_elapsed();
            assert(now >= 0);
            elapsed = (now - was) * 1000 / diminuto_frequency();
            was = now;

            inputs += 1;

            if ((inputs % INPUT_MODULO) != 0) {
                continue;
            }

            if ((outputs % OUTPUT_MODULO) == 0) {
                printf("%6s %4s %4s %6s %6s %6s %6s %6s %6s %6s %6s %4s\n", "STEP", "MS", "DUTY", "TARGET", "ACTUAL", "SAMPLE", "PROPOR", "INTEG", "DIFFER", "TOTAL", "DELTA", "NEXT");
            }

            prime = diminuto_controller(&parameters, &state, target, input, output);

            printf("%6d %4llu %4d %6d %6d %6d %6d %6d %6d %6d %6d %4d\n", outputs, (long long unsigned int)elapsed, output, target, input, state.sample, state.proportional, state.integral, state.differential, state.total, state.delta, prime);

            if ((outputs == 0) || (output != prime)) {
                output = prime;
                assert(diminuto_modulator_set(&modulator, output) == 0);
            }

            outputs += 1;

        }

    }

    /*
     * Fini.
     */

    rc = diminuto_modulator_stop(&modulator);
    assert(rc >= 0);

    mp = diminuto_modulator_fini(&modulator);
    assert(mp == (diminuto_modulator_t *)0);

    diminuto_controller_fini(&parameters, &state);

    rc = diminuto_mux_unregister_interrupt(&mux, fileno(fp));
    assert(rc >= 0);

    diminuto_mux_fini(&mux);

    fp = diminuto_pin_unused(fp, interrupt);
    assert(fp == (FILE *)0);

    fd = diminuto_i2c_close(fd);
    assert(fd < 0);

    if (debug) {
        fprintf(stderr, "%s: exiting\n", program);
    }

    return xc;
}
