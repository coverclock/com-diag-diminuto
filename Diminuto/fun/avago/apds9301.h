/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_APDS_9301_
#define _H_COM_DIAG_DIMINUTO_APDS_9301_

/**
 * @file
 *
 * Copyright 2018 Digital Aggregates Corporation, Colorado USA<BR>
 * Licensed under the terms in LICENSE.txt<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 *
 * Implements a simple API to the Avago APDS-9301 light sensor.
 *
 * In a normal application these functions would be outlined into
 * their own library. For the functional tests, I just inline them.
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

#include "com/diag/diminuto/diminuto_types.h"
#include "com/diag/diminuto/diminuto_i2c.h"
#include "com/diag/diminuto/diminuto_frequency.h"
#include "com/diag/diminuto/diminuto_delay.h"
#include "com/diag/diminuto/diminuto_countof.h"
#include <stdio.h>
#include <math.h>

/**
 * Possible I2C addresses depending on hardware configuration.
 */
enum AvagoApds9301Address {
    AVAGO_APDS9301_ADDRESS_LOW      = 0x29,
    AVAGO_APDS9301_ADDRESS_FLOAT    = 0x39,
    AVAGO_APDS9301_ADDRESS_HIGH     = 0x49,
};

/**
 * Commands.
 */
enum AvagoApds9301Command {
    AVAGO_APDS9301_COMMAND_CMD      = 0x80,
    AVAGO_APDS9301_COMMAND_CLEAR    = 0x40,
    AVAGO_APDS9301_COMMAND_WORD     = 0x20,
    /*  RESERVED                      0x10 */
    AVAGO_APDS9301_COMMAND_ADDRESS  = 0x0f,
};

/**
 * Register addresses.
 */
enum AvagoApds9301Register {
    AVAGO_APDS9301_REGISTER_CONTROL         = 0x0,
    AVAGO_APDS9301_REGISTER_TIMING          = 0x1,
    AVAGO_APDS9301_REGISTER_THRESHLOWLOW    = 0x2,
    AVAGO_APDS9301_REGISTER_THRESHLOWHIGH   = 0x3,
    AVAGO_APDS9301_REGISTER_THRESHHIGHLOW   = 0x4,
    AVAGO_APDS9301_REGISTER_THRESHHIGHHIGH  = 0x5,
    AVAGO_APDS9301_REGISTER_INTERRUPT       = 0x6,
    /*  RESERVED                              0x7 */
    AVAGO_APDS9301_REGISTER_CRC             = 0x8,
    /*  RESERVED                              0x9 */
    AVAGO_APDS9301_REGISTER_ID              = 0xa,
    /*  RESERVED                              0xb */
    AVAGO_APDS9301_REGISTER_DATA0LOW        = 0xc,
    AVAGO_APDS9301_REGISTER_DATA0HIGH       = 0xd,
    AVAGO_APDS9301_REGISTER_DATA1LOW        = 0xe,
    AVAGO_APDS9301_REGISTER_DATA1HIGH       = 0xf,
};

/**
 * Control register.
 */
enum AvagoApds9301Control {
    /* RESERVED                           0xfc */
    AVAGO_APDS9301_CONTROL_POWER        = 0x03,
    AVAGO_APDS9301_CONTROL_POWER_OFF    = 0x00,
    AVAGO_APDS9301_CONTROL_POWER_ON     = 0x03,
};

/**
 * Timing register.
 */
enum AvagoApds9301Timing {
    /* RESERVED                           0xe0 */
    AVAGO_APDS9301_TIMING_GAIN          = 0x10,
    AVAGO_APDS9301_TIMING_GAIN_LOW      = 0x00,
    AVAGO_APDS9301_TIMING_GAIN_HIGH     = 0x10,
    AVAGO_APDS9301_TIMING_MANUAL        = 0x08,
    AVAGO_APDS9301_TIMING_MANUAL_STOP   = 0x00,
    AVAGO_APDS9301_TIMING_MANUAL_START  = 0x08,
    /* RESERVED                           0x04 */
    AVAGO_APDS9301_TIMING_INTEG         = 0x03,
    AVAGO_APDS9301_TIMING_INTEG_13_7MS  = 0x00,
    AVAGO_APDS9301_TIMING_INTEG_101MS   = 0x01,
    AVAGO_APDS9301_TIMING_INTEG_402MS   = 0x02,
    AVAGO_APDS9301_TIMING_INTEG_MANUAL  = 0x03,
};

/**
 * Interrupt register.
 */
enum AvagoApds9301Interrupt {
    /* RESERVED                               0xc0 */
    AVAGO_APDS9301_INTERRUPT_INTR           = 0x30,
    AVAGO_APDS9301_INTERRUPT_INTR_DISABLE   = 0x00,
    AVAGO_APDS9301_INTERRUPT_INTR_ENABLE    = 0x10,
    AVAGO_APDS9301_INTERRUPT_PERSIST        = 0x0f,
    AVAGO_APDS9301_INTERRUPT_PERSIST_EVERY  = 0x00,
    AVAGO_APDS9301_INTERRUPT_PERSIST_ANY    = 0x01,
};

/**
 * Identification register.
 */
enum AvagoApds9301Id {
    AVAGO_APDS9301_ID_PARTNO            = 0xf0,
    AVAGO_APDS9301_ID_PARTNO_EXPECTED   = 0x50,
    AVAGO_APDS9301_ID_REVNO             = 0x0f,
};

/**
 * Power cycle the device and verify that it comes up as expected.
 * @param fd is the open file descriptor to the appropriate I2C bus.
 * @param device is the device address.
 * @return 0 if as expected, <0 otherwise.
 */
static int avago_apds9301_reset(int fd, int device)
{
    int rc = -1;
    uint8_t datum = 0x00;
    diminuto_sticks_t ticks = -1;

    do {

        rc = diminuto_i2c_set(fd, device, AVAGO_APDS9301_COMMAND_CMD | AVAGO_APDS9301_REGISTER_CONTROL, AVAGO_APDS9301_CONTROL_POWER_OFF); 
        if (rc < 0) { break; }

        ticks = diminuto_delay(diminuto_frequency() / 2, !0);
        if (ticks < 0) { rc = -2; break; }

        rc = diminuto_i2c_set_get(fd, device, AVAGO_APDS9301_COMMAND_CMD | AVAGO_APDS9301_REGISTER_CONTROL, AVAGO_APDS9301_CONTROL_POWER_ON, &datum);
        if (rc < 0) { break; }

        datum &= AVAGO_APDS9301_CONTROL_POWER;
        if (datum != AVAGO_APDS9301_CONTROL_POWER_ON) { rc = -3; break; }

    } while (0);

    return rc;
}

/**
 * Dump most device registers to the specified FILE stream.
 * @param fd is the open file descriptor to the appropriate I2C bus.
 * @param device is the device address.
 * @param fp points to an open FILE stream.
 * @return 0 if successful, <0 if an error occurred.
 */
static int avago_apds9301_print(int fd, int device, FILE * fp)
{
    int rc = -1;
    int ii = 0;
    uint8_t datum = 0x00;
    static int REGISTERS[] = {
        AVAGO_APDS9301_REGISTER_CONTROL,
        AVAGO_APDS9301_REGISTER_TIMING,
        AVAGO_APDS9301_REGISTER_THRESHLOWLOW,
        AVAGO_APDS9301_REGISTER_THRESHLOWHIGH,
        AVAGO_APDS9301_REGISTER_THRESHHIGHLOW,
        AVAGO_APDS9301_REGISTER_THRESHHIGHHIGH,
        AVAGO_APDS9301_REGISTER_INTERRUPT,
        AVAGO_APDS9301_REGISTER_CRC,
        AVAGO_APDS9301_REGISTER_ID,
    };

    for (ii = 0; ii < countof(REGISTERS); ++ii) {
        rc = diminuto_i2c_get(fd, device, 0x80 | REGISTERS[ii], &datum);
        if (rc < 0) { break; }
        fprintf(fp, "APDS9310: 0x%02x[0x%02x] = 0x%02x\n", device, REGISTERS[ii], datum);
    }

    return rc;
}

/**
 * Configure the device to its default configuration. This is what I
 * use; your mileage may vary.
 * @param fd is the open file descriptor to the appropriate I2C bus.
 * @param device is the device address.
 * @param gain is false for low gain, true for hish gain.
 * @return 0 if successful, <0 if an error occurred.
 */
static int avago_apds9301_configure(int fd, int device, int gain)
{
    int rc = -1;
    uint8_t datum = 0x00;
    uint8_t value = 0x00;

    do {

        /*
         * Some bits apparently persist despite a software power down above.
         * I'm doing a get_set here mostly just to test it; but it does
         * verify we can access the registers before we try to write to them.
         */

        value = gain ? AVAGO_APDS9301_TIMING_GAIN_HIGH : AVAGO_APDS9301_TIMING_GAIN_LOW;
        value |= AVAGO_APDS9301_TIMING_INTEG_402MS;

        rc = diminuto_i2c_get_set(fd, device, AVAGO_APDS9301_COMMAND_CMD | AVAGO_APDS9301_REGISTER_TIMING, &datum, value);
        if (rc < 0) { break; }

        value = AVAGO_APDS9301_INTERRUPT_INTR_ENABLE;
        value |= AVAGO_APDS9301_INTERRUPT_PERSIST_EVERY;

        rc = diminuto_i2c_get_set(fd, device, AVAGO_APDS9301_COMMAND_CMD | AVAGO_APDS9301_REGISTER_INTERRUPT, &datum, value);
        if (rc < 0) { break; }

    } while (0);

    return rc;
};

/**
 * Compute the measured brightness floating point value in Lux
 * given the raw channel 0 and channel 1 values from the APDS 9301
 * light sensor.
 * @param raw0 is the raw value from channel 0.
 * @param raw1 is the raw value from channel 1.
 * @return the measured brightness in Lux.
 */
static double avago_apds9301_chan2lux(uint16_t raw0, uint16_t raw1)
{
    double lux = 0.0;
    double chan0 = 0.0;
    double chan1 = 0.0;
    double ratio = 0.0;

    chan0 = raw0;
    chan1 = raw1;

    /*
     * APDS-9301 data sheet, p. 4.
     */

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

    /*
     * $ luxcompute -p
     * luxcompute: 0x0 0x0 0.000000 0xffff 0x0 1992.264000
     */

    return lux;
}

/**
 * Extract the raw data from the device on the I2C bus and convert
 * it into units of Lux. This assumes an integration has been completed.
 * As a side effect, it clears the pending interrupt, which is necessary
 * to get another interrupt.
 * @param fd is the open file descriptor to the appropriate I2C bus.
 * @param device is the device address.
 * @return the value in Lux.
 */ 
static double avago_apds9301_sense(int fd, int device)
{
    double lux = -1.0;
    uint8_t datum = 0x00;
    uint16_t chan0 = 0x0000;
    uint16_t chan1 = 0x0000;
    int rc = -1;

    do {

        /*
         * The LOW HIGH access order is important. The corresponding HIGH
         * value isn't latched until the LOW value is read. APDS-9301 data
         * sheet p. 13.
         */

        rc = diminuto_i2c_get(fd, device, AVAGO_APDS9301_COMMAND_CMD | AVAGO_APDS9301_COMMAND_CLEAR | AVAGO_APDS9301_REGISTER_DATA0LOW, &datum);
        if (rc < 0) { break; }
        chan0 = datum;

        rc = diminuto_i2c_get(fd, device, AVAGO_APDS9301_COMMAND_CMD | AVAGO_APDS9301_REGISTER_DATA0HIGH, &datum);
        if (rc < 0) { break; }
        chan0 = ((uint16_t)datum << 8) | chan0;

        rc = diminuto_i2c_get(fd, device, AVAGO_APDS9301_COMMAND_CMD | AVAGO_APDS9301_REGISTER_DATA1LOW, &datum);
        if (rc < 0) { break; }
        chan1 = datum;

        rc = diminuto_i2c_get(fd, device, AVAGO_APDS9301_COMMAND_CMD | AVAGO_APDS9301_REGISTER_DATA1HIGH, &datum);
        if (rc < 0) { break; }
        chan1 = ((uint16_t)datum << 8) | chan1;

        lux = avago_apds9301_chan2lux(chan0, chan1);

    } while (0);

    return lux;
}

/**
 * This is the minimum value in Lux.
 */
static const double AVAGO_APDS9301_LUX_MINIMUM = 0.0;

/**
 * This is the maximum value in Lux (actually around 1992.264).
 */
static const double AVAGO_APDS9301_LUX_MAXIMUM = 2000.0;

#endif
