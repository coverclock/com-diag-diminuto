/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_KERNEL_REGISTER
#define _H_COM_DIAG_DIMINUTO_KERNEL_REGISTER

/**
 * @file
 *
 * Copyright 2010 Digital Aggregates Corporation, Arvada CO 80001-0587 USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 *
 * Implements get and put operators typically used for memory mapped
 * register operations. May be included by user space applications (just
 * to get the data structures) or by kernel space code such as kernel
 * modules and device drivers.
 */

#if defined(__KERNEL__) || defined(MODULE)
#include <linux/types.h>
#if 0
typedef unsigned long uintptr_t; /* Some kernels define this, some don't */
#endif
#include <linux/io.h>
#else
#include <stdint.h>
#endif

typedef union {
    uint8_t eight;
    uint16_t sixteen;
    uint32_t thirtytwo;
    uint64_t sixtyfour;
} diminuto_kernel_datum;

typedef enum {
    EIGHT = sizeof(uint8_t),
    SIXTEEN = sizeof(uint16_t),
    THIRTYTWO = sizeof(uint32_t),
    SIXTYFOUR = sizeof(uint64_t)
} diminuto_kernel_width;

#if defined(__KERNEL__) || defined(MODULE)

/**
 * Gets a datum of a specified width from a virtual address.
 * Alignment must match the width of the datum.
 * @param address is the virtual address.
 * @param width is the width of the datum in bits.
 * @param datump points to where the datum will be returned.
 * @return 0 for success, !0 otherwise.
 */
static int
diminuto_kernel_get(
    const void * address,
    diminuto_kernel_width width,
    diminuto_kernel_datum * datump
) {
    int rc = 0;

    switch (width) {
    case EIGHT:
        datump->eight = *(const volatile uint8_t __iomem *)address;
        break;
    case SIXTEEN:
        if ((uintptr_t)address & 0x1) {
            rc = -EINVAL;
        } else {
            datump->sixteen = *(const volatile uint16_t __iomem *)address;
        }
        break;
    case THIRTYTWO:
        if ((uintptr_t)address & 0x3) {
            rc = -EINVAL;
        } else {
            datump->thirtytwo = *(const volatile uint32_t __iomem *)address;
        }
        break;
    case SIXTYFOUR:
        if ((uintptr_t)address & 0x7) {
            rc = -EINVAL;
        } else {
            datump->sixtyfour = *(const volatile uint64_t __iomem *)address;
        }
        break;
    default:
        rc = -EINVAL;
        break;
    }

    return rc;
}

/**
 * Puts a datum of a specified width to a virtual address.
 * Alignment must match the width of the datum.
 * @param address is the virtual address.
 * @param width is the width of the datum in bits.
 * @param datump points to where the datum will be loaded.
 * @return 0 for success, !0 otherwise.
 */
static int
diminuto_kernel_put(
    void * address,
    diminuto_kernel_width width,
    const diminuto_kernel_datum * datump
) {
    int rc = 0;

    switch (width) {
    case EIGHT:
        *(volatile uint8_t __iomem *)address = datump->eight;
        break;
    case SIXTEEN:
        if ((uintptr_t)address & 0x1) {
            rc = -EINVAL;
        } else {
            *(volatile uint16_t __iomem *)address = datump->sixteen;
        }
        break;
    case THIRTYTWO:
        if ((uintptr_t)address & 0x3) {
            rc = -EINVAL;
        } else {
            *(volatile uint32_t __iomem *)address = datump->thirtytwo;
        }
        break;
    case SIXTYFOUR:
        if ((uintptr_t)address & 0x7) {
            rc = -EINVAL;
        } else {
            *(volatile uint64_t __iomem *)address = datump->sixtyfour;
        }
        break;
    default:
        rc = -EINVAL;
        break;
    }

    return rc;
}

#endif

#endif
