/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2018 Digital Aggregates Corporation, Colorado USA<BR>
 * Licensed under the terms in LICENSE.txt<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 *
 * ABSTRACT
 *
 * This is a functional test of the PID controller, using the PWM
 * modulator to control the brighness on an LED, and a light sensor
 * to implement a feedback loop. The parameters work for the hardware
 * text fixture I use to exercise a number of Diminuto functional tests.
 *
 * The PWM modulator duty cycle ranges from 0 (off) to 100 (on).
 *
 * The light sensor output ranges from 0 to about 1992 lux.
 *
 * USAGE
 *
 * pidtest [ -d ] [ LUXTARGET [ DUTYCYCLE ] ]
 *
 * EXAMPLE
 *
 * pidtest 1500 10
 *
 * REFERENCES
 *
 * Avago, "APDS-9301 Miniature Ambient Light Photo Sensor with Digital
 * (I2C) Output", Avago Technologies, AV02-2315EN, 2010-01-07
 *
 * Wikipedia, "Lux", 2018-03-17
 *
 * Sparkfun, "SparkFun Ambient Light Sensor Breakout - APDS-9301",
 * SEN-14350, https://www.sparkfun.com/products/14350
 *
 * QUOTE
 *
 * "Test what you fly, fly what you test." - NASA aphorism
 *
 * NOTE
 *
 * One weird thing about my hardware test fixture: the ADPS 9301 sensor
 * does *not* measure the highest reading with the duty cycle of the LED
 * is 100%, where the LED is fully on and not modulated at all.. It's moreu
 * like 74%. Still trying to figure that one out.
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

/*
 * These parameters control the setttling time of the software PWM control
 * of the LED (INPUT), and how often we print headers on the report (OUTPUT).
 */

static const int INPUT_MODULO = 4;
static const int OUTPUT_MODULO = 24;

/*
 * These parameters describe the configuration of the hardware test fixture
 * that I built.
 */

static const int INPUT_I2C_BUS = 1;
static const int INPUT_I2C_DEVICE = AVAGO_APDS9301_ADDRESS_FLOAT;
static const int INPUT_GAIN = !0;
static const int INPUT_GPIO_PIN = 26;
static const int OUTPUT_GPIO_PIN = 12;

/*
 * These parameters tune the PID controller. Choosing good values are in
 * my personal experience a bit of an art and require some experimentation.
 */

static const diminuto_controller_value_t PID_INPUT_WINDUP = 20000;
static const diminuto_controller_output_t PID_OUTPUT_MINIMUM = 0;
static const diminuto_controller_output_t PID_OUTPUT_MAXIMUM = 100;
static const diminuto_controller_output_t PID_OUTPUT_LOWER = 0;
static const diminuto_controller_output_t PID_OUTPUT_UPPER = 100;
static const diminuto_controller_factor_t PID_KP_NUMERATOR = 1;
static const diminuto_controller_factor_t PID_KP_DENOMINATOR = 4;
static const diminuto_controller_factor_t PID_KI_NUMERATOR = 1;
static const diminuto_controller_factor_t PID_KI_DENOMINATOR = 8;
static const diminuto_controller_factor_t PID_KD_NUMERATOR = 1;
static const diminuto_controller_factor_t PID_KD_DENOMINATOR = 4;
static const diminuto_controller_factor_t PID_KC_NUMERATOR = 1;
static const diminuto_controller_factor_t PID_KC_DENOMINATOR = 200;
static const bool PID_FILTER = 0;

/*
 * These are the beginning PWM duty cycle (OUTPUT) in percent [0..100],
 * and the default sensor target value (INPUT) in decilux [0..19922].
 */

static const diminuto_controller_output_t PWM_DUTY = 50;
static const diminuto_controller_input_t TARGET_DECILUX= 10000;
static const bool DEBUG = 0;

/*
 * These are the maximum allowable values for the input parameters.
 * (Neither is allowed to be negative.)
 */

static const diminuto_controller_output_t MAXIMUM_DUTY = 100;
static const diminuto_controller_input_t MAXIMUM_DECILUX = 20000;

int main(int argc, char ** argv) {
    int xc = 0;
    const char * program = (const char *)0;
    int fd = -1;
    int rc = -1;
    int led = OUTPUT_GPIO_PIN;
    int bus = INPUT_I2C_BUS;
    int device = INPUT_I2C_DEVICE;
    int interrupt = INPUT_GPIO_PIN;
    int gain = INPUT_GAIN;;
    FILE * fp = (FILE *)0;
    double lux = 0.0;
    diminuto_mux_t mux;
    diminuto_sticks_t ticks = 0;
    diminuto_ticks_t delay = 0;
    diminuto_sticks_t now = 0;
    diminuto_sticks_t was = 0;
    diminuto_ticks_t elapsed = 0;
    int bit = 0;
    diminuto_modulator_t modulator = { 0 };
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
        program = "pidtest";
    } else {
        program = ((program = strrchr(*argv, '/')) == (char *)0) ? *argv : program + 1;
        argv += 1;
        argc -= 1;
    }

    if (argc < 1) {
        /* Do nothing. */
    } else if (strncmp(*argv, "-?", sizeof("-?")) == 0) {
        fprintf(stderr, "usage: %s [ -? | [ -d ] [ DECILUX [ DUTYCYCLE ] ] ]\n", program);
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
        target = TARGET_DECILUX;
    } else {
        end = (char *)0;
        target = strtoul(*argv, &end, 0);
        if ((end == (char *)0) || (*end != '\0') || (target > MAXIMUM_DECILUX)) {
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

    rc = avago_apds9301_reset(fd, device); 
    assert(rc >= 0);

    rc = avago_apds9301_configure(fd, device, gain);
    assert(rc >= 0);

    if (debug) {
        rc = avago_apds9301_print(fd, device, stderr);
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
    parameters.kp.denominator = PID_KP_DENOMINATOR;;
    parameters.ki.numerator = PID_KI_NUMERATOR;
    parameters.ki.numerator = PID_KI_NUMERATOR;
    parameters.kd.numerator = PID_KD_NUMERATOR;
    parameters.kd.denominator = PID_KD_DENOMINATOR;;
    parameters.kc.denominator = PID_KC_DENOMINATOR;;
    parameters.kc.denominator = PID_KC_DENOMINATOR;;
    parameters.filter = PID_FILTER;

    if (debug) {
        fprintf(stderr, "%s: ", program);
        diminuto_controller_parameters_print(stderr, &parameters);
        fprintf(stderr, "%s: ", program);
        diminuto_controller_state_print(stderr, &state);
    }


    /*
     * Pulse width moddulator.
     */

    rc = diminuto_modulator_init(&modulator, led, output);
    assert(rc >= 0);

    if (debug) {
        fprintf(stderr, "%s: ", program);
        diminuto_modulator_print(stderr, &modulator);
    }

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

        while ((rc = diminuto_mux_ready_interrupt(&mux)) >= 0) {

            if (rc != fileno(fp)) {
                continue;
            }

            bit = diminuto_pin_get(fp);
            if (bit == 0) {
                continue;
            }

            lux = avago_apds9301_sense(fd, device);
            assert(lux >= 0.0);
            input = (lux + 0.5) * 10.0;

            now = diminuto_time_elapsed();
            assert(now >= 0);
            elapsed = (now - was) * 1000 / diminuto_frequency();
            was = now;

            inputs += 1;

            if ((inputs % INPUT_MODULO) != 0) {
                continue;
            }

            if ((outputs % OUTPUT_MODULO) == 0) {
                printf("%6s %4s %6s %6s %6s %6s %6s %6s %6s %6s %6s %6s\n", "STEP", "MS", "DUTY", "TARGET", "ACTUAL", "SAMPLE", "PROPOR", "INTEG", "DIFFER", "TOTAL", "DELTA", "NEXT");
            }

            prime = diminuto_controller(&parameters, &state, target, input, output);

            printf("%6d %4llu %6d %6d %6d %6d %6d %6d %6d %6d %6d %6d\n", outputs, elapsed, output, target, input, state.sample, state.proportional, state.integral, state.differential, state.total, state.delta, prime);

            output = prime;

            while (diminuto_modulator_set(&modulator, output) != 0) {
                ticks = diminuto_delay(delay, !0);
                assert(ticks >= 0);
            }

            outputs += 1;

        }

    }

    /*
     * Fini.
     */

    rc = diminuto_modulator_stop(&modulator);
    assert(rc >= 0);

    rc = diminuto_modulator_fini(&modulator);
    assert(rc >= 0);

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
