/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2018 Digital Aggregates Corporation, Colorado USA<BR>
 * Licensed under the terms in LICENSE.txt<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
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

#include "apds_9301.h"

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

static const int LED = 12;
static const int DUTY = 0;
static const int BUS = 1;
static const int DEVICE = 0x39;
static const int INTERRUPT = 26;
static const int GAIN = !0;
static const int SUSTAIN = 3;

int main(int argc, char ** argv) {
    int xc = 0;
    const char * program = (const char *)0;
    int fd = -1;
    int rc = -1;
    uint8_t datum = 0;
    int led = LED;
    int duty = DUTY;
    int bus = BUS;
    int device = DEVICE;
    int interrupt = INTERRUPT;
    int gain = GAIN;
    FILE * fp = (FILE *)0;
    uint16_t chan0 = 0;
    uint16_t chan1 = 0;
    double lux = 0.0;
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

    program = ((program = strrchr(argv[0], '/')) == (char *)0) ? argv[0] : program + 1;

    delay = diminuto_frequency() / 2; /* 500ms > 400ms integration time. */

    /*
     * I2C light sensor.
     */

    fd = diminuto_i2c_open(bus);
    assert(fd >= 0);

    rc = diminuto_i2c_set(fd, device, 0x80, 0x00); 
    assert(rc >= 0);

    ticks = diminuto_delay(delay, !0);
    assert(ticks >= 0);

    rc = diminuto_i2c_set_get(fd, device, 0x80, 0x03, &datum);
    assert(rc >= 0);
    assert(datum == 0x03);

    {
        static int registers[] = { 0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x8, 0xa };
        int ii = 0;

        for (ii = 0; ii < countof(registers); ++ii) {
            rc = diminuto_i2c_get(fd, device, 0x80 | registers[ii], &datum);
            assert(rc >= 0);
            printf("%s: %d@0x%02x[0x%02x] = 0x%02x\n", program, bus, device, registers[ii], datum);
        }
    }

    /* Some bits apparently persist despite a software power down above. */

    rc = diminuto_i2c_get_set(fd, device, 0x81, &datum, gain ? 0x12 : 0x02);
    assert(rc >= 0);
    assert((datum == 0x02) || (datum == 0x12));

    rc = diminuto_i2c_get_set(fd, device, 0x86, &datum, 0x10);
    assert(rc >= 0);
    assert((datum == 0x00) || (datum == 0x10));

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
     * Pulse width moddulator.
     */

    rc = diminuto_modulator_init(&modulator, led, duty);
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
            if (value == 0) {
                continue;
            }

            rc = diminuto_i2c_get(fd, device, 0xcc, &datum);
            assert(rc >= 0);
            chan0 = datum;

            rc = diminuto_i2c_get(fd, device, 0x8d, &datum);
            assert(rc >= 0);
            chan0 = ((uint16_t)datum << 8) | chan0;

            rc = diminuto_i2c_get(fd, device, 0x8e, &datum);
            assert(rc >= 0);
            chan1 = datum;

            rc = diminuto_i2c_get(fd, device, 0x8f, &datum);
            assert(rc >= 0);
            chan1 = ((uint16_t)datum << 8) | chan1;

            lux = apds_9310_chan2lux(chan0, chan1);

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

            printf("%s: PWM %d %% chan0 0x%x %d chan1 0x%x %d Lux %.2f %d lx Period %lld ms\n", program, duty, chan0, chan0, chan1, chan1, lux, value = lux, elapsed);

            duty += increment;
            if (duty > 100) {
                duty = 99;
                increment = -1;
            } else if (duty < 0) {
                break;
            } else {
                /* Do nothing. */
            }

            diminuto_modulator_set(&modulator, duty);

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