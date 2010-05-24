/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_KERNEL_MAP
#define _H_COM_DIAG_DIMINUTO_KERNEL_MAP

/**
 * @file
 *
 * Copyright 2010 Digital Aggregates Corporation, Arvada CO 80001-0587 USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 *
 * Implements functions to map physical memory into kernel virtual memory and
 * to unmap it. This translation unit is only applicable to code in kernel
 * space such as kernel modules and device drivers.
 */

#if defined(__KERNEL__) || defined(MODULE)

#include <linux/kernel.h>
#include <linux/io.h>
#include <linux/ioport.h>
#include <asm/page.h>

/**
 *  Map a region of physical address space to kernel virtual address space.
 *  Optionally reserve the physical address space from being mapped by other
 *  drivers.
 *  @param start is the physical address of the region.
 *  @param length is the size of the region in octets.
 *  @param name points to a reserved resource name or NULL.
 *  @param regionpp points to a reserved resource structure pointer or NULL.
 *  @param basepp points to where the base address will be returned.
 *  @param pagepp points to where the page address will be returned.
 *  @return 0 for success, errno otherwise.
 */
static int
diminuto_kernel_map(
    unsigned long start,
    unsigned long length,
    const char * name,
    struct resource ** regionpp,
    void ** basepp,
    void __iomem ** pagepp
) {
    int rc = 0;
    unsigned long offset;
    unsigned long address;
    unsigned long size;

    do {

#if 1
        /* Align physical memory on a page boundary. */
        offset = start % PAGE_SIZE;
#else
        /* No need to align physical memory on a page boundary. */
        offset = 0;
#endif

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

/**
 * Unmap a region of kernel virtual address space that was previously mapped.
 * @param pagepp points to the virtual page address returned by map or NULL.
 * @param regionpp points to the address of the reserved resource structure returned by map or NULL.
 */
static void
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

#endif

#endif
