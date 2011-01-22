/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_KERNEL_MAP
#define _H_COM_DIAG_DIMINUTO_KERNEL_MAP

/**
 * @file
 *
 * Copyright 2010-2011 Digital Aggregates Corporation, Arvada CO 80001-0587 USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 *
 * Implements functions to map physical memory into kernel virtual memory and
 * to unmap it. This translation unit is only applicable to code in kernel
 * space such as kernel modules and device drivers.
 */

#if defined(__KERNEL__) || defined(MODULE)

#include "diminuto_kernel_types.h"
#include <linux/kernel.h>
#include <linux/io.h>
#include <linux/ioport.h>

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
 *  @param align if true causes physical memory mapping to be paged aligned.
 *  @return 0 for success, errno otherwise.
 */
extern int
diminuto_kernel_map(
    uintptr_t start,
    size_t length,
    const char * name,
    struct resource ** regionpp,
    void ** basepp,
    void __iomem ** pagepp,
    int align
);

/**
 * Unmap a region of kernel virtual address space that was previously mapped.
 * @param pagepp points to the virtual page address returned by map or NULL.
 * @param regionpp points to the address of the reserved resource structure returned by map or NULL.
 */
extern void
diminuto_kernel_unmap(
    void __iomem ** pagepp,
    struct resource ** regionpp
);

#endif

#endif
