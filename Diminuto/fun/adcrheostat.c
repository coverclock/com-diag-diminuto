/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2018-2020 Digital Aggregates Corporation, Colorado USA<BR>
 * Licensed under the terms in LICENSE.txt<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 *
 * UNTESTED
 * BUILDS
 *
 * REFERENCES
 *
 * TI, "Ultra-Small, Low-Power, 16-Bit Analog-to-Digital Converter with
 * Internal Reference", (ADS1113, ADS1114, ADS1115), SBAS4448, Texas
 * Instruments, 2009-10
 *
 * Adafruit, "ADS1115 16-Bit ADC - 4 Channel with Programmable Gain
 * Amplifier, P/N 1085, https://www.adafruit.com/product/1085
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

static const int PWM = HARDWARE_TEST_FIXTURE_PIN_PWM_ADC;
static const int BUS = HARDWARE_TEST_FIXTURE_BUS_I2C;
static const int DEVICE = HARDWARE_TEST_FIXTURE_DEV_I2C_ADC;
static const int INTERRUPT = HARDWARE_TEST_FIXTURE_PIN_INT_ADC;
static const int DUTY = 100;
static const int SUSTAIN = 4;
static const int MEASURE = 5;

int main(int argc, char ** argv) {
    int xc = 0;
    const char * program = (const char *)0;
    int fd = -1;
    int rc = -1;
    uint8_t datum = 0;
    int pwm = PWM;
    int duty = DUTY;
    int bus = BUS;
    int device = DEVICE;
    int interrupt = INTERRUPT;
    FILE * fp = (FILE *)0;
    double volts = 0.0;
    diminuto_mux_t mux;
    diminuto_sticks_t ticks = 0;
    diminuto_ticks_t delay = 0;
    diminuto_sticks_t now = 0;
    diminuto_sticks_t was = 0;
    diminuto_ticks_t elapsed = 0;
    int value = 0;
    diminuto_modulator_t modulator = { 0 };
    int increment = 1;
    int sustain = SUSTAIN;
    int measure = MEASURE;

    program = ((program = strrchr(argv[0], '/')) == (char *)0) ? argv[0] : program + 1;

    delay = diminuto_frequency() / 4; /* 250ms > 125ms conversion time. */

    /*
     * I2C light sensor.
     */

    fd = diminuto_i2c_open(bus);
    assert(fd >= 0);

    rc = ti_ads1115_configure_default(fd, device);
    assert(rc >= 0);

    rc = ti_ads1115_print(fd, device, stdout);
    assert(rc >= 0);

    rc = ti_ads1115_sense(fd, device, &volts);
    assert(rc >= 0);

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

    value = diminuto_pin_get(fp);
    assert(value >= 0);

    /*
     * Multiplexer.
     */

    diminuto_mux_init(&mux);

    rc = diminuto_mux_register_interrupt(&mux, fileno(fp));
    assert(rc >= 0);

    /*
     * Pulse width moddulator.
     */

    rc = diminuto_modulator_init(&modulator, pwm, duty);
    assert(rc >= 0);

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

            value = diminuto_pin_get(fp);
            assert(value >= 0);
            if (value == 0) {
                continue;
            }

            rc = ti_ads1115_sense(fd, device, &volts);
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

            printf("%s: PWM %3d %% on %3u off %3u ADC %7.3f v Period %3lld ms\n", program, duty, modulator.on, modulator.off, volts, (long long int)elapsed);

            /*
             * Measure allows us to read the voltage more than once.
             */

            if (measure > 1) {
                measure -= 1;
                continue;
            }

            measure = MEASURE;

            duty += increment;
            if (duty > 100) {
                duty = 99;
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

    rc = diminuto_modulator_fini(&modulator);
    assert(rc >= 0);

    rc = diminuto_mux_unregister_interrupt(&mux, fileno(fp));
    assert(rc >= 0);

    diminuto_mux_fini(&mux);

    fp = diminuto_pin_unused(fp, interrupt);
    assert(fp == (FILE *)0);

    fd = diminuto_i2c_close(fd);
    assert(fd < 0);

    printf("%s: exiting\n", program);

    return xc;
}

