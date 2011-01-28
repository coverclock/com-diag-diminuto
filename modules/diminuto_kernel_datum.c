/* vi: set ts=4 expandtab shiftwidth=4: */

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

#include "diminuto_datum.h"
#include "diminuto_barrier.h"
#include <linux/module.h>
#include <linux/io.h>
#include <asm/errno.h>

#define DIMINUTO_KERNEL_GET(_ALIGN_, _DEST_, _TYPE_) \
	do { \
        if ((uintptr_t)address & (_ALIGN_)) { \
            rc = -EINVAL; \
        } else { \
            datump->_DEST_ = *(const volatile _TYPE_ __iomem *)address; \
        } \
	} while (0)

#define DIMINUTO_KERNEL_PUT(_ALIGN_, _SRC_, _TYPE_) \
	do { \
        if ((uintptr_t)address & (_ALIGN_)) { \
            rc = -EINVAL; \
        } else { \
            *(volatile _TYPE_ __iomem *)address = datump->_SRC_; \
        } \
    } while (0)

int
diminuto_kernel_get(
    const void * address,
    diminuto_datum_width width,
    diminuto_datum_value * datump
) {
    int rc = 0;

    diminuto_barrier();

    switch (width) {
    case EIGHT:		DIMINUTO_KERNEL_GET(0x0, eight, uint8_t);		break;
    case SIXTEEN:	DIMINUTO_KERNEL_GET(0x1, sixteen, uint16_t);	break;
    case THIRTYTWO:	DIMINUTO_KERNEL_GET(0x3, thirtytwo, uint32_t);	break;
    case SIXTYFOUR:	DIMINUTO_KERNEL_GET(0x7, sixtyfour, uint64_t);	break;
    default:		rc = -EINVAL;									break;
    }

    return rc;
}
EXPORT_SYMBOL(diminuto_kernel_get);

int
diminuto_kernel_put(
    void * address,
    diminuto_datum_width width,
    const diminuto_datum_value * datump
) {
    int rc = 0;

    switch (width) {
    case EIGHT:		DIMINUTO_KERNEL_PUT(0x0, eight, uint8_t);		break;
    case SIXTEEN:	DIMINUTO_KERNEL_PUT(0x1, sixteen, uint16_t);	break;
    case THIRTYTWO:	DIMINUTO_KERNEL_PUT(0x3, thirtytwo, uint32_t);	break;
    case SIXTYFOUR:	DIMINUTO_KERNEL_PUT(0x7, sixtyfour, uint64_t);	break;
    default:		rc = -EINVAL;									break;
    }

    if (rc == 0) { diminuto_barrier(); }

    return rc;
}
EXPORT_SYMBOL(diminuto_kernel_put);

MODULE_AUTHOR("coverclock@diag.com");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("diminuto datum module");
