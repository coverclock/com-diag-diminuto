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
 * UNTESTED
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
#include <arpa/inet.h>

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
    TI_ADS1115_COMMAND_POINTER  = 0x03,
};

/**
 * Register addresses.
 */
enum TiAds1115Register {
    TI_ADS1115_REGISTER_CONVERSION  = 0x00,
    TI_ADS1115_REGISTER_CONFIG      = 0x01,
    TI_ADS1115_REGISTER_LOTHRESH    = 0x02,
    TI_ADS1115_REGISTER_HITHRESH    = 0x03,
};

/**
 * Conversion register.
 */
enum TiAds1115Conversion {
    TI_ADS1115_CONVERSION_MINUMUM = 0x8000, /* -FS */
    TI_ADS1115_CONVERSION_MAXIMUM = 0x7fff, /* +FS */
};

/**
 * Config register.
 */
enum TiAds1115Config {
    TI_ADS1115_CONFIG_OS                    = 0x8000,
    TI_ADS1115_CONFIG_OS_START              = 0x8000,
    TI_ADS1115_CONFIG_OS_NOP                = 0x0000,
    TI_ADS1115_CONFIG_OS_BUSY               = 0x0000,
    TI_ADS1115_CONFIG_OS_IDLE               = 0x8000,
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
 * Low Threshold register.
 */
enum TiAds1115LoThresh {
    TI_ADS1115_LOTHRESH_DEFAULT = 0x8000,
    TI_ADS1115_LOTHRESH_RDY     = 0x0000,
};

/**
 * Low Threshold register.
 */
enum TiAds1115HiThresh {
    TI_ADS1115_HITHRESH_DEFAULT = 0x7fff,
    TI_ADS1115_HITHRESH_RDY     = 0x8000,
};

/**
 * Convert from device byte order (big endian) to host byte order.
 * @param datum to convert.
 * @return converted datum.
 */
static inline uint16_t ti_ads1115_dtoh(uint16_t datum)
{
    return ntohs(datum);
}

/**
 * Convert from host byte order to device byte order (big endian).
 * @param datum to convert.
 * @return converted datum.
 */
static inline uint16_t ti_ads1115_htod(uint16_t datum)
{
    return htons(datum);
}

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
        TI_ADS1115_REGISTER_LOTHRESH,
        TI_ADS1115_REGISTER_HITHRESH,
    };

    for (ii = 0; ii < countof(REGISTERS); ++ii) {
        rc = diminuto_i2c_get_word(fd, device, REGISTERS[ii], &datum);
        if (rc < 0) { break; }
        fprintf(fp, "APDS9310: 0x%02x[0x%02x] = 0x%04x\n", device, REGISTERS[ii], ti_ads1115_dtoh(datum));
    }

    return rc;
}

/**
 * Configure the device. If the config word has the START bit set, a
 * conversion will begin. If a conversion is already running, this
 * operation will fail.
 * @param fd is the open file descriptor to the appropriate I2C bus.
 * @param device is the device address.
 * @param lothresh is the desired value of the low threshold register.
 * @param hithresh is the desired value of the high threshold register.
 * @param config is the desired value of the config register.
 * @return 0 if successful, <0 if an error occurred.
 */
static int ti_ads1115_configure(int fd, int device, uint16_t lothresh, uint16_t hithresh, uint16_t config)
{
    int rc = -1;
    uint8_t buffer[sizeof(uint8_t) + sizeof(uint16_t)];

    do {

        buffer[0] = TI_ADS1115_REGISTER_LOTHRESH;
        buffer[1] = lothresh >> 8;
        buffer[2] = lothresh & 0xff;

        rc = diminuto_i2c_send(fd, device, buffer, sizeof(buffer));
        if (rc < 0) { break; }

        buffer[0] = TI_ADS1115_REGISTER_HITHRESH;
        buffer[1] = hithresh >> 8;
        buffer[2] = hithresh & 0xff;

        rc = diminuto_i2c_send(fd, device, buffer, sizeof(buffer));
        if (rc < 0) { break; }

        buffer[0] = TI_ADS1115_REGISTER_CONFIG;
        buffer[1] = config >> 8;
        buffer[2] = config & 0xff;

        rc = diminuto_i2c_send(fd, device, buffer, sizeof(buffer));
        if (rc < 0) { break; }

    } while (0);

    return rc;
};

/**
 * Configure the device and start a conversion.
 * @param fd is the open file descriptor to the appropriate I2C bus.
 * @param device is the device address.
 * @return 0 if successful, <0 if an error occurred.
 */
static inline int ti_ads1115_configure_default(int fd, int device)
{
    return ti_ads1115_configure(fd, device, TI_ADS1115_LOTHRESH_RDY, TI_ADS1115_HITHRESH_RDY, TI_ADS1115_CONFIG_OS_NOP | TI_ADS1115_CONFIG_MUX_AIN0_GND | TI_ADS1115_CONFIG_PGA_6_144V | TI_ADS1115_CONFIG_MODE_CONTINUOUS | TI_ADS1115_CONFIG_DR_128SPS | TI_ADS1115_CONFIG_COMP_MODE_WINDOW | TI_ADS1115_CONFIG_COMP_POL_HIGH | TI_ADS1115_CONFIG_COMP_LAT_ON | TI_ADS1115_CONFIG_COMP_QUE_1);
}

#if defined(COM_DIAG_DIMINUTO_TI_ADS1115_UNTESTED)
/**
 * Start a conversion using the existing configuration. If a conversion is
 * already running, this operation will fail.
 * @param fd is the open file descriptor to the appropriate I2C bus.
 * @param device is the device address.
 * @param config is the desired value of the config register.
 * @return 0 if successful, <0 if an error occurred.
 */
static int ti_ads1115_start(int fd, int device)
{
    int rc = -1;
    uint16_t datum = 0x0000;

    do {

        rc = diminuto_i2c_get_word(fd, device, TI_ADS1115_REGISTER_CONFIG, &datum);
        if (rc < 0) { break; }
        datum = ti_ads1115_dtoh(datum);

        if ((datum & TI_ADS1115_CONFIG_OS) != TI_ADS1115_CONFIG_OS_IDLE) { rc = -2; break; }

        datum |= TI_ADS1115_CONFIG_OS_START;

        rc = diminuto_i2c_set_word(fd, device, TI_ADS1115_REGISTER_CONFIG, ti_ads1115_htod(datum));
        if (rc < 0) { break; }
        
    } while (0);

    return rc;
};

/**
 * Check if an Analog to Digital Conversion is in progress.
 * @param fd is the open file descriptor to the appropriate I2C bus.
 * @param device is the device address.
 * @return 0 if not busy, >0 if busy, <0 if an error occurred.
 */
static int ti_ads1115_check(int fd, int device)
{
    int rc = -1;
    uint16_t datum = 0x0000;

    do {

        rc = diminuto_i2c_get_word(fd, device, TI_ADS1115_REGISTER_CONFIG, &datum);
        if (rc < 0) { break; }
        datum = ti_ads1115_dtoh(datum);

        rc = ((datum & TI_ADS1115_CONFIG_OS) != TI_ADS1115_CONFIG_OS_IDLE);

    } while (0);

    return rc;
};
#endif

static int ti_ads1115_rawtovolts(uint16_t config, int16_t raw, double * voltsp)
{
    int rc = -1;
    double pga = 0.0;

    /*
     * ADS1115 data sheet, p. 13, t. 3.
     * ADS1115 data sheet, p. 18, t. 9.
     * ADS1115 data sheet, p. 19,
     */

    switch (config & TI_ADS1115_CONFIG_PGA) {
    case TI_ADS1115_CONFIG_PGA_6_144V:
        pga = 6.144;
        break;
    case TI_ADS1115_CONFIG_PGA_4_096V:
        pga = 4.096;
        break;
    case TI_ADS1115_CONFIG_PGA_2_048V:
        pga = 2.048;
        break;
    case TI_ADS1115_CONFIG_PGA_1_024V:
        pga = 1.024;
        break;
    case TI_ADS1115_CONFIG_PGA_0_512V:
        pga = 0.512;
        break;
    case TI_ADS1115_CONFIG_PGA_0_256V:
    case TI_ADS1115_CONFIG_PGA_0_256BV:
    case TI_ADS1115_CONFIG_PGA_0_256CV:
        pga = 0.256;
        break;
    default:
        break;
    }

    /*
     * ADS1115 data sheet, p. 14, t, 4.
     */

    if (pga > 0.0) {
        *voltsp = raw;
        *voltsp *= pga;
        *voltsp /= 32767.0;
        rc = 0;
    }

    return rc;
}

/**
 * Read an Analog to Digital Conversion value.
 * @param fd is the open file descriptor to the appropriate I2C bus.
 * @param device is the device address.
 * @param bufferp is the buffer into which the conversion is placed.
 * @return 0 if successful, <0 if an error occurred.
 */ 
static int ti_ads1115_sense(int fd, int device, double * bufferp)
{
    int rc = -1;
    uint16_t config = 0x0000;
    int16_t raw = 0x0000;


    do {

        rc = diminuto_i2c_get_word(fd, device, TI_ADS1115_REGISTER_CONFIG, &config);
        if (rc < 0) { break; }

        rc = diminuto_i2c_get_word(fd, device, TI_ADS1115_REGISTER_CONVERSION, &raw);
        if (rc < 0) { break; }

        rc = ti_ads1115_rawtovolts(ti_ads1115_dtoh(config), ti_ads1115_dtoh(raw), bufferp);

    } while (0);

    return rc;
}

#endif
