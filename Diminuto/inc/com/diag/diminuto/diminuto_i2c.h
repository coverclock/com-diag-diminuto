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
 */

#include "com/diag/diminuto/diminuto_types.h"

extern int diminuto_i2c_open(int bus);

extern int diminuto_i2c_close(int fd);

extern int diminuto_i2c_use(int fd, uint8_t addr);

extern ssize_t diminuto_i2c_read(int fd, void * bufferp, size_t size);

extern ssize_t diminuto_i2c_write(int fd, const void *  bufferp, size_t size);

extern int diminuto_i2c_get(int fd, uint8_t addr, uint8_t reg, uint8_t * datap);

extern int diminuto_i2c_set(int fd, uint8_t addr, uint8_t reg, uint8_t data);

#endif
