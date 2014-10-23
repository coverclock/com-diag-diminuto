/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_DATUM_
#define _H_COM_DIAG_DIMINUTO_DATUM_

/**
 * @file
 *
 * Copyright 2010-2011 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 *
 * Implements get and put operators typically used for memory mapped
 * register operations. May be included by user space applications (just
 * to get the data structures) or by kernel space code such as kernel
 * modules and device drivers. The get and put functions enforce strict
 * alignment even if the underlying processor does not. Getting an alignment
 * hardware exception in kernel-state is more serious, because it may deny the
 * use of a kernel module to other applications, than doing so in a strictly
 * user-state program.
 */

#include "com/diag/diminuto/diminuto_types.h"

/**
 * This union can contain any value that is supported by the kernel-space Datum
 * loadable module.
 */
typedef union DiminutoDatumValue {
    uint8_t  value8;
    uint16_t value16;
    uint32_t value32;
    uint64_t value64;
} diminuto_datum_value_t;

/**
 * This enumerates all of the data types supported by the kernel-space Datum
 * loadable module.
 */
typedef enum DiminutoDatumWidth {
    WIDTH8  = sizeof(uint8_t),
    WIDTH16 = sizeof(uint16_t),
    WIDTH32 = sizeof(uint32_t),
    WIDTH64 = sizeof(uint64_t)
} diminuto_datum_width_t;

#define DIMINUTO_DATUM_ALIGNMENT(_WIDTH_) (((_WIDTH_) / 8 ) - 1)
#define DIMINUTO_DATUM_WIDTH(_WIDTH_) WIDTH ## _WIDTH_
#define DIMINUTO_DATUM_TYPE(_WIDTH_) uint ## _WIDTH_ ## _t
#define DIMINUTO_DATUM_VALUE(_WIDTH_) value ## _WIDTH_

#if defined(__KERNEL__) || defined(MODULE)

/**
 * Gets a datum of a specified width from a virtual address.
 * Alignment must match the width of the datum.
 * @param address is the virtual address.
 * @param width is the width of the datum in bits.
 * @param datump points to where the datum will be returned.
 * @return 0 for success, !0 otherwise.
 */
extern int diminuto_kernel_get(const void * address, diminuto_datum_width_t width, diminuto_datum_value_t * datump);

/**
 * Puts a datum of a specified width to a virtual address.
 * Alignment must match the width of the datum.
 * @param address is the virtual address.
 * @param width is the width of the datum in bits.
 * @param datump points to where the datum will be loaded.
 * @return 0 for success, !0 otherwise.
 */
extern int diminuto_kernel_put(void * address, diminuto_datum_width_t width, const diminuto_datum_value_t * datump);

#endif

#endif
