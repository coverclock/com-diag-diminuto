/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2018 Digital Aggregates Corporation, Colorado USA<BR>
 * Licensed under the terms in LICENSE.txt<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 *
 * USAGE
 *
 * EXAMPLES
 *
 * ABSTRACT
 *
 * REFERENCES
 *
 */

#include "com/diag/diminuto/diminuto_controller.h"
#include "com/diag/diminuto/diminuto_countof.h"
#include "com/diag/diminuto/diminuto_criticalsection.h"
#include "com/diag/diminuto/diminuto_interrupter.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_modulator.h"
#include "com/diag/diminuto/diminuto_pin.h"
#include "com/diag/diminuto/diminuto_terminator.h"
#include "com/diag/diminuto/diminuto_types.h"

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

/*********************************************************************************
 * I2C
 ********************************************************************************/

const char DIMINUTO_I2C_FILENAME[] = "/dev/i2c-%d";

pthread_mutex_t diminuto_i2c_mutex = PTHREAD_MUTEX_INITIALIZER;

int diminuto_i2c_open(int bus)
{
    int fd = -1;
    char buffer[sizeof(DIMINUTO_I2C_FILENAME)] = { '\0' };

    snprintf(buffer, sizeof(buffer), DIMINUTO_I2C_FILENAME, bus);
    buffer[sizeof(buffer) - 1] = '\0';

    fd = open(buffer, O_RDWR);
    if (fd < 0) {
        diminuto_perror("diminuto_i2c_open: open");
    }

    return fd;
}

int diminuto_i2c_use(int fd, uint8_t addr)
{
    int rc = -1;

    rc = ioctl(fd, I2C_SLAVE, addr);
    if (rc < 0) {
        diminuto_perror("diminuto_i2c_use: ioctl");
    }

    return rc;
}

ssize_t diminuto_i2c_read(int fd, void * bufferp, size_t size)
{
    ssize_t rc = -1;

    rc = read(fd, bufferp, size);
    if (rc < 0) {
        diminuto_perror("diminuto_i2c_read: read");
    } else if (rc > size) {
        errno = E2BIG; /* Should be impossible. */
        diminuto_perror("diminuto_i2c_read: read");
        rc = -1;
    } else if (rc == 0) {
        errno = ENODEV;
        diminuto_perror("diminuto_i2c_read: read");
        rc = -1;
    } else {
        /* Do nothing. */
    }

    return rc;
}

ssize_t diminuto_i2c_write(int fd, const void *  bufferp, size_t size)
{
    ssize_t rc = -1;

    rc = write(fd, bufferp, size);
    if (rc < 0) {
        diminuto_perror("diminuto_i2c_write: write");
    } else if (rc > size) {
        errno = E2BIG; /* Should be impossible. */
        diminuto_perror("diminuto_i2c_write: write");
        rc = -1;
    } else if (rc == 0) {
        errno = ENODEV;
        diminuto_perror("diminuto_i2c_write: write");
        rc = -1;
    } else {
        /* Do nothing. */
    }

    return rc;
}

int diminuto_i2c_close(int fd)
{
    if (close(fd) < 0) {
        diminuto_perror("close");
    } else {
        fd = -1;
    }

    return fd;
}

int diminuto_i2c_get(int fd, uint8_t addr, uint8_t reg, uint8_t * datap)
{
    int rc = -1;

    DIMINUTO_CRITICAL_SECTION_BEGIN(&diminuto_i2c_mutex);

        if (diminuto_i2c_use(fd, addr) < 0) {
            /* Do nothing. */
        } else if (diminuto_i2c_write(fd, &reg, sizeof(reg)) < 0) {
            /* Do nothing. */
        } else {
            rc = diminuto_i2c_read(fd, datap, sizeof(*datap));
        }

    DIMINUTO_CRITICAL_SECTION_END;

    return rc;
}

int diminuto_i2c_set(int fd, uint8_t addr, uint8_t reg, uint8_t data)
{
    int rc = -1;

    DIMINUTO_CRITICAL_SECTION_BEGIN(&diminuto_i2c_mutex);

        if (diminuto_i2c_use(fd, addr) < 0) {
            /* Do nothing. */
        } else if (diminuto_i2c_write(fd, &reg, sizeof(reg)) < 0) {
            /* Do nothing. */
        } else {
            rc = diminuto_i2c_write(fd, &data, sizeof(data));
        }

    DIMINUTO_CRITICAL_SECTION_END;

    return rc;
}

/*********************************************************************************
 * Avago APDS 9310
 ********************************************************************************/

static double apds_9310_raw2lux(uint16_t raw0, uint16_t raw1)
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

/*********************************************************************************
 * Main
 ********************************************************************************/

int main(int argc, char ** argv) {
    int xc = 0;
    const char * program = (const char *)0;
    int fd = -1;
    int rc = -1;
    uint8_t datum = 0;
    int bus = 1;
    int device = 0x39;

    program = ((program = strrchr(argv[0], '/')) == (char *)0) ? argv[0] : program + 1;

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
            printf("%s: [0x%02x] = 0x%02x\n", program,  registers[ii], datum);
        }
    }

    return xc;
}
