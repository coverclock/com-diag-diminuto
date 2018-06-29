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
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

static const char DIMINUTO_I2C_FORMAT[] = "/dev/i2c-%d";

static int diminuto_i2c_open(int bus)
{
    int fd = -1;
    char buffer[sizeof(DIMINUTO_I2C_FORMAT)] = { '\0' };

    snprintf(buffer, sizeof(buffer), DIMINUTO_I2C_FORMAT, bus);
    buffer[sizeof(buffer) - 1] = '\0';

    fd = open(buffer, O_RDWR);
    if (fd < 0) {
        diminuto_perror(buffer);
    }

    return fd;
}

static int diminuto_i2c_use(int fd, uint8_t addr)
{
    int rc = -1;

    rc = ioctl(fd, I2C_SLAVE, addr);
    if (rc < 0) {
        diminuto_perror("ioctl");
    }

    return rc;
}

static ssize_t diminuto_i2c_read(int fd, uint8_t * datap)
{
    ssize_t rc = -1;

    rc = read(fd, datap, sizeof(*datap));
    if (rc < 0) {
        diminuto_perror("read");
    }

    return rc;
}

static ssize_t diminuto_i2c_write(int fd, uint8_t data)
{
    ssize_t rc = -1;

    rc = write(fd, &data, sizeof(data));
    if (rc < 0) {
        diminuto_perror("write");
    }

    return rc;
}

static int diminuto_i2c_close(int fd)
{
    int rc = -1;

    rc = close(fd);
    if (rc < 0) {
        diminuto_perror("close");
    } else {
        fd = -1;
    }

    return rc;
}

static int diminuto_i2c_get(int fd, uint8_t addr, uint8_t reg, uint8_t * datap)
{
    int rc = -1;

    if (diminuto_i2c_use(fd, addr) < 0) {
        /* Do nothing. */
    } else if (diminuto_i2c_write(fd, reg) < 0) {
        /* Do nothing. */
    } else if (diminuto_i2c_read(fd, datap) < 0) {
        /* Do nothing. */
    } else {
        rc = 0; 
    }

    return rc;
}

static int diminuto_i2c_set(int fd, uint8_t addr, uint8_t reg, uint8_t data)
{
    int rc = -1;

    if (diminuto_i2c_use(fd, addr) < 0) {
        /* Do nothing. */
    } else if (diminuto_i2c_write(fd, reg) < 0) {
        /* Do nothing. */
    } else if (diminuto_i2c_write(fd, data) < 0) {
        /* Do nothing. */
    } else {
        rc = 0; 
    }

    return rc;
}

int main(int argc, char ** argv) {
    int xc = 0;

    return xc;
}
