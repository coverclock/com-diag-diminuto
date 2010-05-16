/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_MMDRIVER_
#define _H_COM_DIAG_DIMINUTO_MMDRIVER_

/**
 * @file
 *
 * Copyright 2010 Digital Aggregates Corporation, Arvada CO 80001-0587 USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 *
 * This file describes the user space interface to the Diminuto Generic
 * Memory Mapped Device Driver. At compile or install time, the driver is
 * provided by the user with a default space physical address and length
 * in octets. It implements atomic read, write, bit set, and bit clear
 * operations against physical addresses in that space. This is a common
 * approach used to communicate with FPGAs which expose registers mapped
 * into physical memory. Typically additional application specific I/O
 * control commands would be added, although you can do a lot of testing
 * with just the basic operations.
 */

/*******************************************************************************
 *  INCLUDES
 ******************************************************************************/

#include <asm/ioctl.h>
#if defined(__KERNEL__) || defined(MODULE)
#include <linux/types.h>
#else
#include <stdint.h>
#endif
#include "diminuto_kernel_reg.h"

/*******************************************************************************
 *  MANIFEST CONSTANTS
 ******************************************************************************/

#define DIMINUTO_MMDRIVER_NODE      ("/dev/mmdriver")
#define DIMINUTO_MMDRIVER_MAGIC     (0xe0)

/*******************************************************************************
 *  DATA STRUCTURES
 ******************************************************************************/

typedef struct {
    uintptr_t relative;             /* Relative to beginning of driver space */
    diminuto_kernel_width width;    /* From diminuto_kernel_reg.h */
    diminuto_kernel_datum datum;    /* From diminuto_kernel_reg.h */
} diminuto_mmdriver_op;

/*******************************************************************************
 *  I/O CONTROL COMMANDS
 ******************************************************************************/
    
/**
 * @def DIMINUTO_MMDRIVER_READ
 *
 * Read at address of length octets into data.
 */
#define DIMINUTO_MMDRIVER_READ       _IOWR(DIMINUTO_MMDRIVER_MAGIC, 0, diminuto_mmdriver_op)

/**
 * @def DIMINUTO_MMDRIVER_WRITE
 *
 * Write data to address of length octets.
 */
#define DIMINUTO_MMDRIVER_WRITE      _IOW(DIMINUTO_MMDRIVER_MAGIC, 1, diminuto_mmdriver_op)

/**
 * @def DIMINUTO_MMDRIVER_SET
 *
 * Or at address of length octets with data.
 */
#define DIMINUTO_MMDRIVER_SET        _IOW(DIMINUTO_MMDRIVER_MAGIC, 2, diminuto_mmdriver_op)

/**
 * @def DIMINUTO_MMDRIVER_CLEAR
 *
 * And at address of length octets with bit inverse of data.
 */
#define DIMINUTO_MMDRIVER_CLEAR      _IOW(DIMINUTO_MMDRIVER_MAGIC, 3, diminuto_mmdriver_op)

#endif
