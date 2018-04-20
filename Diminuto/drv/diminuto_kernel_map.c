/* vi: set ts=4 expandtab shiftwidth=4: */

/**
 * @file
 *
 * Copyright 2010-2011 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in LICENSE.txt<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 *
 * Implements functions to map physical memory into kernel virtual memory and
 * to unmap it. This translation unit is only applicable to code in kernel
 * space such as kernel modules and device drivers.
 */

#include "com/diag/diminuto/diminuto_map.h"
#include <linux/module.h>
#include <asm/page.h>
#include <asm/errno.h>

int
diminuto_kernel_map(
	uintptr_t start,
    size_t length,
    const char * name,
    struct resource ** regionpp,
    void ** basepp,
    void __iomem ** pagepp,
    int align
) {
    int rc = 0;
    size_t offset;
    uintptr_t address;
    size_t size;

    do {

        /* Align physical memory on a page boundary. */
        offset = align ? start % PAGE_SIZE : 0;
        address = start - offset;
        size = length + offset;

        if ((!name) || (!regionpp)) {
            /* Do nothing. */
        } else if (!(*regionpp = request_mem_region(start, length, name))) {
            pr_err("diminuto_kernel_map: request_mem_region failed!\n");
            rc = -EPERM;
            break;
        } else {
            /* Do nothing. */
        }

        if (!pagepp) {
            /* Do nothing. */
        } else if (!(*pagepp = ioremap_nocache(address, size))) {
            pr_err("diminuto_kernel_map: ioremap_nocache failed!\n");
            rc = -ENOMEM;
            break;
        } else if (basepp) {
            *basepp = (char *)*pagepp + offset;
        } else {
            /* Do nothing. */
        }

    } while (0);

    
    if (rc == 0) {
        /* Do nothing. */
    } else if (regionpp && *regionpp) {
        release_mem_region((*regionpp)->start, (*regionpp)->end - (*regionpp)->start);
    } else {
        /* Do nothing. */
    }

    return rc;
}
EXPORT_SYMBOL(diminuto_kernel_map);

void
diminuto_kernel_unmap(
    void __iomem ** pagepp,
    struct resource ** regionpp
) {
    if (pagepp && *pagepp) {
        iounmap(*pagepp);
        *pagepp = 0;
    }

    if (regionpp && *regionpp) {
        release_mem_region((*regionpp)->start, (*regionpp)->end - (*regionpp)->start);
        *regionpp = 0;
    }
}
EXPORT_SYMBOL(diminuto_kernel_unmap);

MODULE_AUTHOR("coverclock@diag.com");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("diminuto memory map module");
