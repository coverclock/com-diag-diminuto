/* vi: set ts=4 expandtab shiftwidth=4: */

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
 * modules and device drivers.
 */

#include "com/diag/diminuto/diminuto_datum.h"
#include "com/diag/diminuto/diminuto_barrier.h"
#include <linux/module.h>
#include <linux/io.h>
#include <asm/errno.h>

#define DIMINUTO_KERNEL_GET(_WIDTH_) \
	do { \
        if ((uintptr_t)address & DIMINUTO_DATUM_ALIGNMENT(_WIDTH_)) { \
            rc = -EINVAL; \
        } else { \
            datump->DIMINUTO_DATUM_VALUE(_WIDTH_) = *(const volatile DIMINUTO_DATUM_TYPE(_WIDTH_) __iomem *)address; \
        } \
	} while (0)

#define DIMINUTO_KERNEL_PUT(_WIDTH_) \
	do { \
        if ((uintptr_t)address & DIMINUTO_DATUM_ALIGNMENT(_WIDTH_)) { \
            rc = -EINVAL; \
        } else { \
            *(volatile DIMINUTO_DATUM_TYPE(_WIDTH_) __iomem *)address = datump->DIMINUTO_DATUM_VALUE(_WIDTH_); \
        } \
    } while (0)

int
diminuto_kernel_get(
    const void * address,
    diminuto_datum_width_t width,
    diminuto_datum_value_t * datump
) {
    int rc = 0;

    diminuto_barrier();

    switch (width) {
    case DIMINUTO_DATUM_WIDTH8:		DIMINUTO_KERNEL_GET(8);		break;
    case DIMINUTO_DATUM_WIDTH16:	DIMINUTO_KERNEL_GET(16);	break;
    case DIMINUTO_DATUM_WIDTH32:	DIMINUTO_KERNEL_GET(32);	break;
    case DIMINUTO_DATUM_WIDTH64:	DIMINUTO_KERNEL_GET(64);	break;
    default:						rc = -EINVAL;				break;
    }

    return rc;
}
EXPORT_SYMBOL(diminuto_kernel_get);

int
diminuto_kernel_put(
    void * address,
    diminuto_datum_width_t width,
    const diminuto_datum_value_t * datump
) {
    int rc = 0;

    switch (width) {
    case DIMINUTO_DATUM_WIDTH8:		DIMINUTO_KERNEL_PUT(8);		break;
    case DIMINUTO_DATUM_WIDTH16:	DIMINUTO_KERNEL_PUT(16);	break;
    case DIMINUTO_DATUM_WIDTH32:	DIMINUTO_KERNEL_PUT(32);	break;
    case DIMINUTO_DATUM_WIDTH64:	DIMINUTO_KERNEL_PUT(64);	break;
    default:						rc = -EINVAL;				break;
    }

    if (rc == 0) { diminuto_barrier(); }

    return rc;
}
EXPORT_SYMBOL(diminuto_kernel_put);

MODULE_AUTHOR("coverclock@diag.com");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("diminuto datum module");
