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
 */

#include "com/diag/diminuto/diminuto_controller.h"
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

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

static double apds_9310_chan2lux(uint16_t raw0, uint16_t raw1)
{
    double lux = 0.0;
    double chan0 = 0.0;
    double chan1 = 0.0;
    double ratio = 0.0;

    chan0 = raw0;
    chan1 = raw1;

    ratio = chan1 / chan0;

    if (ratio <= 0.50) {
        lux = (0.0304 * chan0) - (0.062 * chan1 * pow(ratio, 1.4));
    } else if ((0.50 < ratio) && (ratio <= 0.61)) {
        lux = (0.0224 * chan0) - (0.031 * chan1);
    } else if ((0.61 < ratio) && (ratio <= 0.80)) {
        lux = (0.0128 * chan0) - (0.0153 * chan1);
    } else if ((0.80 < ratio) && (ratio <= 1.30)) {
        lux = (0.00146 * chan0) - (0.00112 * chan1);
    } else {
        lux = 0.0;
    }

    return lux;
}

int main(int argc, char ** argv) {
    int xc = 0;
    const char * program = (const char *)0;
    int fd = -1;
    int rc = -1;
    uint8_t datum = 0;
    int led = 12;
    int bus = 1;
    int device = 0x39;
    int interrupt = 26;
    FILE * fp = (FILE *)0;
    uint16_t chan0 = 0;
    uint16_t chan1 = 0;
    double lux = 0.0;
    diminuto_mux_t mux;
    diminuto_ticks_t ticks = 0;

    program = ((program = strrchr(argv[0], '/')) == (char *)0) ? argv[0] : program + 1;

    /*
     * I2C light sensor.
     */

    fd = diminuto_i2c_open(bus);
    assert(fd >= 0);

    rc = diminuto_i2c_set(fd, device, 0x80, 0x00); 
    assert(rc >= 0);

    rc = diminuto_i2c_set(fd, device, 0x80, 0x03);
    assert(rc >= 0);

    rc = diminuto_i2c_get(fd, device, 0x80, &datum);
    assert(rc >= 0);
    assert(datum == 0x03);

    rc = diminuto_i2c_set(fd, device, 0x81, 0x12);
    assert(rc >= 0);

    rc = diminuto_i2c_set(fd, device, 0x86, 0x10);
    assert(rc >= 0);

    {
        static int registers[] = { 0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x8, 0xa };
        int ii = 0;

        for (ii = 0; ii < countof(registers); ++ii) {
            rc = diminuto_i2c_get(fd, device, 0x80 | registers[ii], &datum);
            assert(rc >= 0);
            printf("%s: %d@0x%02x[0x%02x] = 0x%02x\n", program, bus, device, registers[ii], datum);
        }
    }

    /*
     * GPIO interrupt pin.
     */

    (void)diminuto_pin_unexport_ignore(interrupt); 

    fp = diminuto_pin_input(interrupt);
    assert(fp != (FILE *)0);

    rc = diminuto_pin_edge(interrupt, DIMINUTO_PIN_EDGE_FALLING);
    assert(rc == 0);

    /*
     * Multiplexer.
     */

    diminuto_mux_init(&mux);

    rc = diminuto_mux_register_interrupt(&mux, fileno(fp));
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

    ticks = diminuto_frequency() / 2;

    while (!0) {

        rc = diminuto_mux_wait(&mux, -1);

        if (rc >= 0) {
            /* Do nothing. */
        } else if (errno != EINTR) {
            break;
        } else if (diminuto_terminator_check()) {
            break;
        } else if (diminuto_interrupter_check()) {
            break;
        } else {
            diminuto_delay(ticks, !0);
            continue;
        }

        if (rc == 0) {
            diminuto_delay(ticks, !0);
            continue;
        }

        if (diminuto_mux_ready_interrupt(&mux) != fileno(fp)) {
            diminuto_delay(ticks, !0);
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

        printf("%s: 0x%04x 0x%04x %.2f\n", program, chan0, chan1, lux);

    }

    /*
     * Done.
     */

    diminuto_mux_unregister_interrupt(&mux, fileno(fp));

    diminuto_mux_fini(&mux);

    fp = diminuto_pin_unused(fp, interrupt);
    assert(fp == (FILE *)0);

    fd = diminuto_i2c_close(fd);
    assert(fd < 0);

    return xc;
}
