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

/*********************************************************************************
 * Basic Operations
 ********************************************************************************/

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

/*********************************************************************************
 * Composite Operations
 ********************************************************************************/

/**
 * Read a byte from the specified register of the specified I2C device.
 * @param fd is an open file descriptor.
 * @param addr identifies the device address.
 * @param reg identifies the device register.
 * @param datap points to the buffer into which the byte will be gotten.
 * @return 0 for success, <0 if an error occurred.
 */
extern int diminuto_i2c_get(int fd, uint8_t addr, uint8_t reg, uint8_t * datap);

/**
 * Write a byte to the specified register of the specified I2C device.
 * @param fd is an open file descriptor.
 * @param addr identifies the device address.
 * @param reg identifies the device register.
 * @param data is the byte to set.
 * @return 0 for success, <0 if an error occurred.
 */
extern int diminuto_i2c_set(int fd, uint8_t addr, uint8_t reg, uint8_t data);

/*********************************************************************************
 * Multiple Operations
 ********************************************************************************/

/**
 * Perform a set followed by a get to the same register of the same I2C device.
 * @param fd is an open file descriptor.
 * @param addr identifies the device address.
 * @param reg identifies the device register.
 * @param data is the byte to set.
 * @param datap points to the buffer into which the byte will be gotten.
 * @return 0 for success, <0 if an error occurred.
 */
extern int diminuto_i2c_set_get(int fd, uint8_t addr, uint8_t reg, uint8_t data, uint8_t * datap);

/**
 * Perform a get followed by a set to the same register of the same I2C device.
 * @param fd is an open file descriptor.
 * @param addr identifies the device address.
 * @param reg identifies the device register.
 * @param datap points to the buffer into which the byte will be gotten.
 * @param data is the byte to set.
 * @return 0 for success, <0 if an error occurred.
 */
extern int diminuto_i2c_get_set(int fd, uint8_t addr, uint8_t reg, uint8_t * datap, uint8_t data);

#endif