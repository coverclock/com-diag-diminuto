/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_MMDRIVER_
#define _H_COM_DIAG_DIMINUTO_MMDRIVER_

/**
 * @file
 *
 * Copyright 2010, 2014 Digital Aggregates Corporation, Colorado, USA.
 * Licensed under the terms in LICENSE.txt.
 *
 * THe Memory Mapped Driver feature  describes the user space interface to
 * the Diminuto Generic Memory Mapped Device Driver. At compile or install
 * time, the driver is provided by the user with a default space physical 
 * address and length in octets. It implements atomic read, write, bit set,
 * and bit clear operations against physical addresses in that space. This
 * is a common approach used to communicate with FPGAs which expose registers
 * mapped into physical memory. Typically additional application specific I/O
 * control commands would be added, although you can do a lot of testing
 * with just the basic operations.
 */

/*******************************************************************************
 *  INCLUDES
 ******************************************************************************/

#include "com/diag/diminuto/diminuto_datum.h"
#include "com/diag/diminuto/diminuto_platform.h"
#if defined(COM_DIAG_DIMINUTO_PLATFORM_KERNEL)
#   include <asm/ioctl.h>
#else
#   if defined(COM_DIAG_DIMINUTO_PLATFORM_CYGWIN)
#      if !defined(__USE_LINUX_IOCTL_DEFS)
#          define __USE_LINUX_IOCTL_DEFS
#          define COM_DIAG_DIMINUTO_MMDRIVER_UNDEF__USE_LINUX_IOCTL_DEFS
#      endif
#   endif
#   include <sys/ioctl.h>
#   if defined(COM_DIAG_DIMINUTO_MMDRIVER_UNDEF__USE_LINUX_IOCTL_DEFS)
#      undef __USE_LINUX_IOCTL_DEFS
#      undef COM_DIAG_DIMINUTO_MMDRIVER_UNDEF__USE_LINUX_IOCTL_DEFS
#   endif
#endif

/*******************************************************************************
 *  MANIFEST CONSTANTS
 ******************************************************************************/

#define DIMINUTO_MMDRIVER_NODE      ("/dev/mmdriver")
#define DIMINUTO_MMDRIVER_MAGIC     (0xe0)

/*******************************************************************************
 *  DATA STRUCTURES
 ******************************************************************************/

/**
 * This structure is used to pass parameters through the ioctl(2) interface from
 * the application in user-space to the Memory Mapped Driver in kernel-space.
 */
typedef struct DiminutoMmDriverOp {

    /** Offset from beginning of driver space. */
    uintptr_t offset;

    /** Datum width. */
    diminuto_datum_width_t width;

    /** Datum value. */
    diminuto_datum_value_t datum;

    /** Mask value. */
    diminuto_datum_value_t mask;

} diminuto_mmdriver_op_t;

/*******************************************************************************
 *  I/O CONTROL COMMANDS
 ******************************************************************************/

/**
 * @def DIMINUTO_MMDRIVER_READ
 *
 * Read at address of length octets into data.
 */
#define DIMINUTO_MMDRIVER_READ       _IOWR(DIMINUTO_MMDRIVER_MAGIC, 0, diminuto_mmdriver_op_t)

/**
 * @def DIMINUTO_MMDRIVER_WRITE
 *
 * Write data to address of length octets.
 */
#define DIMINUTO_MMDRIVER_WRITE      _IOW(DIMINUTO_MMDRIVER_MAGIC, 1, diminuto_mmdriver_op_t)

/**
 * @def DIMINUTO_MMDRIVER_SET
 *
 * Or at address of length octets with data.
 */
#define DIMINUTO_MMDRIVER_SET        _IOW(DIMINUTO_MMDRIVER_MAGIC, 2, diminuto_mmdriver_op_t)

/**
 * @def DIMINUTO_MMDRIVER_CLEAR
 *
 * And at address of length octets with bit inverse of data.
 */
#define DIMINUTO_MMDRIVER_CLEAR      _IOW(DIMINUTO_MMDRIVER_MAGIC, 3, diminuto_mmdriver_op_t)

#endif
