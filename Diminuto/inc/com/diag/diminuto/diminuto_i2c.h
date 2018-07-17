/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_I2C_
#define _H_COM_DIAG_DIMINUTO_I2C_

/**
 * @file
 *
 * Copyright 2018 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in LICENSE.txt<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 *
 * ABSTRACT
 *
 * This is not a general purpose I2C API. It implements an API on top
 * of the Linux I2C ioctl(2) API that is useful for the kinds of I2C
 * devices I tend to work with.
 *
 * REFERENCES
 *
 * Linux 4.4.34, Documentation/i2c/muxes/dev-interface
 *
 * NXP, "I2C-bus specification and user manual", UM10204, Rev.6, NXP
 * Semiconductor N.V., 2014-04-04
 */

#include "com/diag/diminuto/diminuto_types.h"

/*******************************************************************************
 * WRAPPER OPERATIONS
 ******************************************************************************/

/**
 * Open the I2C device for the specified bus and return its file descriptor.
 * @param bus is the bus number, typically 0 or 1.
 * @return an open file descriptor.
 */
extern int diminuto_i2c_open(int bus);

/**
 * CLose the file descriptor pointed to the device representing an I2C bus.
 * @param fd is an open file descriptor.
 * @return <0 for success, or the original argument if an error occurred.
 */
extern int diminuto_i2c_close(int fd);

/**
 * Indicate which device on the I2C bus to use for subsequent operations.
 * @param fd is an open file descriptor.
 * @param addr identifies the device address.
 * @return 0 for success, or <0 if an error occurred.
 */
extern int diminuto_i2c_use(int fd, uint8_t addr);

/**
 * Read the indicated number of bytes from the I2C device in use.
 * @param fd is an open file descriptor.
 * @param bufferp points to a buffer into which data will be read.
 * @param size is the number of bytes to read.
 * @return the number of bytes read, or <0 if an error occurred.
 */
extern ssize_t diminuto_i2c_read(int fd, void * bufferp, size_t size);

/**
 * Write the indicated number of bytes to the I2C device in use.
 * @param fd is an open file descriptor.
 * @param bufferp points to a buffer from which data will be written.
 * @param size is the number of bytes to write.
 * @return the number of bytes written, or <0 if an error occurred.
 */
extern ssize_t diminuto_i2c_write(int fd, const void *  bufferp, size_t size);

/*******************************************************************************
 * COMPOSITE OPERATIONS
 ******************************************************************************/

/**
 * Get data from the specified register of the specified I2C device.
 * @param fd is an open file descriptor.
 * @param addr identifies the device address.
 * @param reg identifies the device register.
 * @param bufferp points to the buffer into which the data will be gotten.
 * @param size is the number of bytes to get.
 * @return 0 for success, <0 if an error occurred.
 */
extern int diminuto_i2c_get(int fd, uint8_t addr, uint8_t reg, void * bufferp, size_t size);

/**
 * Set data to the specified register of the specified I2C device.
 * @param fd is an open file descriptor.
 * @param addr identifies the device address.
 * @param reg identifies the device register.
 * @param datap points to the data to set.
 * @param size is the number of bytes to set.
 * @return 0 for success, <0 if an error occurred.
 */
extern int diminuto_i2c_set(int fd, uint8_t addr, uint8_t reg, const void * datap, size_t size);

/*******************************************************************************
 * MULTIPLE OPERATIONS
 ******************************************************************************/

/**
 * Perform a set followed by a get to the same register of the same I2C device.
 * @param fd is an open file descriptor.
 * @param addr identifies the device address.
 * @param reg identifies the device register.
 * @param datap points to the data to set.
 * @param bufferp points to the buffer into which the data will be gotten.
 * @param size is the number of bytes to both set and get.
 * @return 0 for success, <0 if an error occurred.
 */
extern int diminuto_i2c_set_get(int fd, uint8_t addr, uint8_t reg, const void * datap, void * bufferp, size_t size);

/**
 * Perform a get followed by a set to the same register of the same I2C device.
 * @param fd is an open file descriptor.
 * @param addr identifies the device address.
 * @param reg identifies the device register.
 * @param bufferp points to the buffer into which the data will be gotten.
 * @param datap points to the data to set.
 * @param size is the number of bytes to both set and get.
 * @return 0 for success, <0 if an error occurred.
 */
extern int diminuto_i2c_get_set(int fd, uint8_t addr, uint8_t reg, void * bufferp, const void * datap, size_t size);

/*******************************************************************************
 * DERIVED OPERATIONS (not adequately tested IMO)
 ******************************************************************************/

/**
 * Get byte from the specified register of the specified I2C device.
 * @param fd is an open file descriptor.
 * @param addr identifies the device address.
 * @param reg identifies the device register.
 * @param bufferp points to the buffer into which the byte will be gotten.
 * @return 0 for success, <0 if an error occurred.
 */
static inline int diminuto_i2c_get_byte(int fd, uint8_t addr, uint8_t reg, uint8_t * bufferp)
{
    return diminuto_i2c_get(fd, addr, reg, bufferp, sizeof(*bufferp));
}

/**
 * Get word from the specified register of the specified I2C device.
 * @param fd is an open file descriptor.
 * @param addr identifies the device address.
 * @param reg identifies the device register.
 * @param bufferp points to the buffer into which the word will be gotten.
 * @return 0 for success, <0 if an error occurred.
 */
static inline diminuto_i2c_get_word(int fd, uint8_t addr, uint8_t reg, uint16_t * bufferp)
{
    return diminuto_i2c_get(fd, addr, reg, bufferp, sizeof(*bufferp));
}

/******************************************************************************/

/**
 * Set byte to the specified register of the specified I2C device.
 * @param fd is an open file descriptor.
 * @param addr identifies the device address.
 * @param reg identifies the device register.
 * @param datum is the byte to set.
 * @return 0 for success, <0 if an error occurred.
 */
static inline int diminuto_i2c_set_byte(int fd, uint8_t addr, uint8_t reg, uint8_t datum)
{
    return diminuto_i2c_set(fd, addr, reg, &datum, sizeof(datum));;
}

/**
 * Set word to the specified register of the specified I2C device.
 * @param fd is an open file descriptor.
 * @param addr identifies the device address.
 * @param reg identifies the device register.
 * @param datum is the word to set.
 * @return 0 for success, <0 if an error occurred.
 */
static inline int diminuto_i2c_set_word(int fd, uint8_t addr, uint8_t reg, uint16_t datum)
{
    return diminuto_i2c_set(fd, addr, reg, &datum, sizeof(datum));
}

/******************************************************************************/

/**
 * Perform a set followed by a get to the same register of the same I2C device.
 * @param fd is an open file descriptor.
 * @param addr identifies the device address.
 * @param reg identifies the device register.
 * @param datum is the byte to set.
 * @param bufferp points to the buffer into which the byte will be gotten.
 * @return 0 for success, <0 if an error occurred.
 */
static inline int diminuto_i2c_set_get_byte(int fd, uint8_t addr, uint8_t reg, uint8_t datum, uint8_t * bufferp)
{
    return diminuto_i2c_set_get(fd, addr, reg, &datum, bufferp, sizeof(datum));
}

/**
 * Perform a set followed by a get to the same register of the same I2C device.
 * @param fd is an open file descriptor.
 * @param addr identifies the device address.
 * @param reg identifies the device register.
 * @param datum is to the word to set.
 * @param bufferp points to the buffer into which the word will be gotten.
 * @return 0 for success, <0 if an error occurred.
 */
static inline int diminuto_i2c_set_get_word(int fd, uint8_t addr, uint8_t reg, uint16_t datum, uint16_t * bufferp)
{
    return diminuto_i2c_set_get(fd, addr, reg, &datum, bufferp, sizeof(datum));
}

/******************************************************************************/

/**
 * Perform a get followed by a set to the same register of the same I2C device.
 * @param fd is an open file descriptor.
 * @param addr identifies the device address.
 * @param reg identifies the device register.
 * @param bufferp points to the buffer into which the byte will be gotten.
 * @param datum is the to the byte to set.
 * @return 0 for success, <0 if an error occurred.
 */
static inline int diminuto_i2c_get_set_byte(int fd, uint8_t addr, uint8_t reg, uint8_t * bufferp, uint8_t datum)
{
    return diminuto_i2c_get_set(fd, addr, reg, bufferp, &datum, sizeof(datum));
}

/**
 * Perform a get followed by a set to the same register of the same I2C device.
 * @param fd is an open file descriptor.
 * @param addr identifies the device address.
 * @param reg identifies the device register.
 * @param bufferp points to the buffer into which the word will be gotten.
 * @param datum is the to the word to set.
 * @return 0 for success, <0 if an error occurred.
 */
static inline int diminuto_i2c_get_set_word(int fd, uint8_t addr, uint8_t reg, uint16_t * bufferp, uint16_t datum)
{
    return diminuto_i2c_get_set(fd, addr, reg, bufferp, &datum, sizeof(datum));
}

/******************************************************************************/

#endif
