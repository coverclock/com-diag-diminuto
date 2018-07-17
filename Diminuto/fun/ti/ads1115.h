/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_TI_ADS1115_
#define _H_COM_DIAG_DIMINUTO_TI_ADS1115_

/**
 * @file
 *
 * Copyright 2018 Digital Aggregates Corporation, Colorado USA<BR>
 * Licensed under the terms in LICENSE.txt<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 *
 * Implements a simple API to the Texas Instruments (TI) ADS1115 Analog to
 * Digital Convertor (ADC). In a normal application these functions would
 * be outlined into their own library. For the functional tests, I just
 * inline them.
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

#include "com/diag/diminuto/diminuto_types.h"
#include "com/diag/diminuto/diminuto_i2c.h"
#include "com/diag/diminuto/diminuto_frequency.h"
#include "com/diag/diminuto/diminuto_delay.h"
#include "com/diag/diminuto/diminuto_countof.h"
#include <stdio.h>

/**
 * Possible I2C addresses depending on hardware configuration.
 */
enum TiAds1115Address {
    TI_ADS1115_ADDRESS_LOW  = 0x48,
    TI_ADS1115_ADDRESS_HIGH = 0x49,
    TI_ADS1115_ADDRESS_SDA  = 0x4a,
    TI_ADS1115_ADDRESS_SCL  = 0x4b,
};

/**
 * Commands.
 */
enum TiAds1115Command {
    TI_ADS1115_COMMAND_ZEROES   = 0xfc,
    TI_ADS1115_COMMAND_POINTER  = 0x03,
};

/**
 * Register addresses.
 */
enum TiAds1115Register {
    TI_ADS1115_REGISTER_CONVERSION  = 0x0,
    TI_ADS1115_REGISTER_CONFIG      = 0x1,
    TI_ADS1115_REGISTER_LO_THRESH   = 0x2,
    TI_ADS1115_REGISTER_HI_THRESH   = 0x3,
};

/**
 * Config register.
 */
enum TiAds1115Config {
    TI_ADS1115_CONFIG_OS                    = 0x8000,
    TI_ADS1115_CONFIG_OS_START              = 0x8000,
    TI_ADS1115_CONFIG_OS_BUSY               = 0x8000,
    TI_ADS1115_CONFIG_OS_IDLE               = 0x0000,
    TI_ADS1115_CONFIG_MUX                   = 0x7000,
    TI_ADS1115_CONFIG_MUX_AIN0_AIN1         = 0x0000,
    TI_ADS1115_CONFIG_MUX_AIN0_AIN3         = 0x1000,
    TI_ADS1115_CONFIG_MUX_AIN1_AIN3         = 0x2000,
    TI_ADS1115_CONFIG_MUX_AIN2_AIN3         = 0x3000,
    TI_ADS1115_CONFIG_MUX_AIN0_GND          = 0x4000,
    TI_ADS1115_CONFIG_MUX_AIN1_GND          = 0x5000,
    TI_ADS1115_CONFIG_MUX_AIN2_GND          = 0x6000,
    TI_ADS1115_CONFIG_MUX_AIN3_GND          = 0x7000,
    TI_ADS1115_CONFIG_PGA                   = 0x0e00,
    TI_ADS1115_CONFIG_PGA_6_144V            = 0x0000,
    TI_ADS1115_CONFIG_PGA_4_096V            = 0x0200,
    TI_ADS1115_CONFIG_PGA_2_048V            = 0x0400,
    TI_ADS1115_CONFIG_PGA_1_024V            = 0x0600,
    TI_ADS1115_CONFIG_PGA_0_512V            = 0x0800,
    TI_ADS1115_CONFIG_PGA_0_256V            = 0x0a00,
    TI_ADS1115_CONFIG_PGA_0_256BV           = 0x0c00,
    TI_ADS1115_CONFIG_PGA_0_256CV           = 0x0e00,
    TI_ADS1115_CONFIG_MODE                  = 0x0100,
    TI_ADS1115_CONFIG_MODE_CONTINUOUS       = 0x0000,
    TI_ADS1115_CONFIG_MODE_SINGLE           = 0x0100,
    TI_ADS1115_CONFIG_DR                    = 0x00e0,
    TI_ADS1115_CONFIG_DR_8SPS               = 0x0000,
    TI_ADS1115_CONFIG_DR_16SPS              = 0x0020,
    TI_ADS1115_CONFIG_DR_32SPS              = 0x0040,
    TI_ADS1115_CONFIG_DR_64SPS              = 0x0060,
    TI_ADS1115_CONFIG_DR_128SPS             = 0x0080,
    TI_ADS1115_CONFIG_DR_250SPS             = 0x00a0,
    TI_ADS1115_CONFIG_DR_475SPS             = 0x00c0,
    TI_ADS1115_CONFIG_DR_860SPS             = 0x00e0,
    TI_ADS1115_CONFIG_COMP_MODE             = 0x0010,
    TI_ADS1115_CONFIG_COMP_MODE_HYSTERESIS  = 0x0000,
    TI_ADS1115_CONFIG_COMP_MODE_WINDOW      = 0x0010,
    TI_ADS1115_CONFIG_COMP_POL              = 0x0008,
    TI_ADS1115_CONFIG_COMP_POL_LOW          = 0x0000,
    TI_ADS1115_CONFIG_COMP_POL_HIGH         = 0x0008,
    TI_ADS1115_CONFIG_COMP_LAT              = 0x0004,
    TI_ADS1115_CONFIG_COMP_LAT_OFF          = 0x0000,
    TI_ADS1115_CONFIG_COMP_LAT_ON           = 0x0004,
    TI_ADS1115_CONFIG_COMP_QUE              = 0x0003,
    TI_ADS1115_CONFIG_COMP_QUE_1            = 0x0000,
    TI_ADS1115_CONFIG_COMP_QUE_2            = 0x0001,
    TI_ADS1115_CONFIG_COMP_QUE_4            = 0x0002,
    TI_ADS1115_CONFIG_COMP_QUE_DISABLE      = 0x0003,
};

/**
 * Dump most device registers to the specified FILE stream.
 * @param fd is the open file descriptor to the appropriate I2C bus.
 * @param device is the device address.
 * @param fp points to an open FILE stream.
 * @return 0 if successful, <0 if an error occurred.
 */
static int ti_ads1115_print(int fd, int device, FILE * fp)
{
    int rc = -1;
    int ii = 0;
    uint16_t datum = 0x0000;
    static int REGISTERS[] = {
        TI_ADS1115_REGISTER_CONVERSION,
        TI_ADS1115_REGISTER_CONFIG,
        TI_ADS1115_REGISTER_LO_THRESH,
        TI_ADS1115_REGISTER_HI_THRESH,
    };

    for (ii = 0; ii < countof(REGISTERS); ++ii) {
        rc = diminuto_i2c_get16(fd, device, REGISTERS[ii], &datum);
        if (rc < 0) { break; }
        fprintf(fp, "APDS9310: 0x%02x[0x%02x] = 0x%04x\n", device, REGISTERS[ii], datum);
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
static int ti_ads1115_configure(int fd, int device, int gain)
{
    int rc = -1;
    uint16_t value = 0x0000;

    do {

        value = TI_ADS1115_CONFIG_MUX_AIN0_GND;
        value |= TI_ADS1115_CONFIG_PGA_2_048V;
        value |= TI_ADS1115_CONFIG_MODE_CONTINUOUS;
        value |= TI_ADS1115_CONFIG_DR_128SPS;
        
        rc = diminuto_i2c_set(fd, device, TI_ADS1115_REGISTER_CONFIG, value);
        if (rc < 0) { break; }
        
    } while (0);

    return rc;
};

/**
 * Extract the raw data from the device on the I2C bus and convert
 * it into units of Lux. This assumes an integration has been completed.
 * As a side effect, it clears the pending interrupt, which is necessary
 * to get another interrupt.
 * @param fd is the open file descriptor to the appropriate I2C bus.
 * @param device is the device address.
 * @return the value in Lux or <0.0 if an error occurred.
 */ 
static uint16_t ti_ads1115_convert(int fd, int device)
{
    uint16_t datum = 0x0000;
    int rc = -1;

    while (0) {

        rc = diminuto_i2c_get(fd, device, TI_ADS1115_REGISTER_CONVERSION, &datum);
        if (rc < 0) { break; }

    }

    return lux;
}

#endif
