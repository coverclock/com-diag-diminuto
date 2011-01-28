/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_DATUM_
#define _H_COM_DIAG_DIMINUTO_DATUM_

/**
 * @file
 *
 * Copyright 2010-2011 Digital Aggregates Corporation, Arvada CO 80001-0587 USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 *
 * Implements get and put operators typically used for memory mapped
 * register operations. May be included by user space applications (just
 * to get the data structures) or by kernel space code such as kernel
 * modules and device drivers.
 */

#include "diminuto_types.h"

typedef union {
    uint8_t eight;
    uint16_t sixteen;
    uint32_t thirtytwo;
    uint64_t sixtyfour;
} diminuto_datum_value;

typedef enum {
    EIGHT = sizeof(uint8_t),
    SIXTEEN = sizeof(uint16_t),
    THIRTYTWO = sizeof(uint32_t),
    SIXTYFOUR = sizeof(uint64_t)
} diminuto_datum_width;

#if defined(__KERNEL__) || defined(MODULE)

/**
 * Gets a datum of a specified width from a virtual address.
 * Alignment must match the width of the datum.
 * @param address is the virtual address.
 * @param width is the width of the datum in bits.
 * @param datump points to where the datum will be returned.
 * @return 0 for success, !0 otherwise.
 */
extern int diminuto_kernel_get(const void * address, diminuto_datum_width width, diminuto_datum_value * datump);

/**
 * Puts a datum of a specified width to a virtual address.
 * Alignment must match the width of the datum.
 * @param address is the virtual address.
 * @param width is the width of the datum in bits.
 * @param datump points to where the datum will be loaded.
 * @return 0 for success, !0 otherwise.
 */
extern int diminuto_kernel_put(void * address, diminuto_datum_width width, const diminuto_datum_value * datump);

#endif

#endif
