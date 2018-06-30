/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2018 Digital Aggregates Corporation, Colorado USA<BR>
 * Licensed under the terms in LICENSE.txt<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 */

#include "com/diag/diminuto/diminuto_criticalsection.h"
#include "com/diag/diminuto/diminuto_log.h"

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

static const char DIMINUTO_I2C_FILENAME[] = "/dev/i2c-%d";

pthread_mutex_t diminuto_i2c_mutex = PTHREAD_MUTEX_INITIALIZER; /* Global but private. */

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
