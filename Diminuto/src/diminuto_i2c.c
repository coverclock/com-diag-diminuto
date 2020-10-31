/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2018 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is the implementation of the I-squared-C (I2C) feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is the implementation of the I-squared-C (I2C) feature.
 */

#include "com/diag/diminuto/diminuto_criticalsection.h"
#include "com/diag/diminuto/diminuto_i2c.h"
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

/*******************************************************************************
 * WRAPPER OPERATIONS
 ******************************************************************************/

int diminuto_i2c_open(int bus)
{
    int fd = -1;
    char buffer[sizeof(DIMINUTO_I2C_FILENAME)] = { '\0' };

    snprintf(buffer, sizeof(buffer), DIMINUTO_I2C_FILENAME, bus);
    buffer[sizeof(buffer) - 1] = '\0';

    if ((fd = open(buffer, O_RDWR)) < 0) {
        diminuto_perror("diminuto_i2c_open: open");
    }

    return fd;
}

int diminuto_i2c_use(int fd, uint8_t addr)
{
    int rc = -1;

    if ((rc = ioctl(fd, I2C_SLAVE, addr)) < 0) {
        diminuto_perror("diminuto_i2c_use: ioctl");
    }

    return rc;
}

ssize_t diminuto_i2c_read(int fd, void * bufferp, size_t size)
{
    ssize_t rc = -1;

    if ((rc = read(fd, bufferp, size)) < 0) {
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

    if ((rc = write(fd, bufferp, size)) < 0) {
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

/*******************************************************************************
 * COMPOSITE OPERATIONS
 ******************************************************************************/

int diminuto_i2c_get(int fd, uint8_t addr, uint8_t reg, void * bufferp, size_t size)
{
    int rc = -1;

    DIMINUTO_CRITICAL_SECTION_BEGIN(&diminuto_i2c_mutex);

        if ((rc = diminuto_i2c_use(fd, addr)) < 0) {
            /* Do nothing. */
        } else if ((rc = diminuto_i2c_write(fd, &reg, sizeof(reg))) < 0) {
            /* Do nothing. */
        } else {
            rc = diminuto_i2c_read(fd, bufferp, size);
        }

    DIMINUTO_CRITICAL_SECTION_END;

    return rc;
}

int diminuto_i2c_set(int fd, uint8_t addr, uint8_t reg, const void * datap, size_t size)
{
    int rc = -1;

    DIMINUTO_CRITICAL_SECTION_BEGIN(&diminuto_i2c_mutex);

        if ((rc = diminuto_i2c_use(fd, addr)) < 0) {
            /* Do nothing. */
        } else if ((rc = diminuto_i2c_write(fd, &reg, sizeof(reg))) < 0) {
            /* Do nothing. */
        } else {
            rc = diminuto_i2c_write(fd, datap, size);
        }

    DIMINUTO_CRITICAL_SECTION_END;

    return rc;
}

int diminuto_i2c_send(int fd, uint8_t addr, const void * datap, size_t size)
{
    int rc = -1;

    DIMINUTO_CRITICAL_SECTION_BEGIN(&diminuto_i2c_mutex);

        if ((rc = diminuto_i2c_use(fd, addr)) < 0) {
            /* Do nothing. */
        } else {
            rc = diminuto_i2c_write(fd, datap, size);
        }

    DIMINUTO_CRITICAL_SECTION_END;

    return rc;
}

/*******************************************************************************
 * MULTIPLE OPERATIONS
 ******************************************************************************/

int diminuto_i2c_get_set(int fd, uint8_t addr, uint8_t reg, void * bufferp, const void * datap, size_t size)
{
    int rc = -1;

    DIMINUTO_CRITICAL_SECTION_BEGIN(&diminuto_i2c_mutex);

        if (diminuto_i2c_use(fd, addr) < 0) {
            /* Do nothing. */
        } else if (diminuto_i2c_write(fd, &reg, sizeof(reg)) < 0) {
            /* Do nothing. */
        } else if ((rc = diminuto_i2c_read(fd, bufferp, size)) < 0) {
            /* Do nothing. */
        } else {
            rc = diminuto_i2c_write(fd, datap, size);
        }

    DIMINUTO_CRITICAL_SECTION_END;

    return rc;
}

int diminuto_i2c_set_get(int fd, uint8_t addr, uint8_t reg, const void * datap, void * bufferp, size_t size)
{
    int rc = -1;

    DIMINUTO_CRITICAL_SECTION_BEGIN(&diminuto_i2c_mutex);

        if (diminuto_i2c_use(fd, addr) < 0) {
            /* Do nothing. */
        } else if (diminuto_i2c_write(fd, &reg, sizeof(reg)) < 0) {
            /* Do nothing. */
        } else if ((rc = diminuto_i2c_write(fd, datap, size)) < 0) {
            /* Do nothing. */
        } else {
            rc = diminuto_i2c_read(fd, bufferp, size);
        }

    DIMINUTO_CRITICAL_SECTION_END;

    return rc;
}
