/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_MAP_
#define _H_COM_DIAG_DIMINUTO_MAP_

/**
 * @file
 *
 * Copyright 2008-2011 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "com/diag/diminuto/diminuto_types.h"

#if defined(__KERNEL__) || defined(MODULE)

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
extern int diminuto_kernel_map(uintptr_t start, size_t length, const char * name, struct resource ** regionpp, void ** basepp, void __iomem ** pagepp, int align);

/**
 * Unmap a region of kernel virtual address space that was previously mapped.
 * @param pagepp points to the virtual page address returned by map or NULL.
 * @param regionpp points to the address of the reserved resource structure returned by map or NULL.
 */
extern void diminuto_kernel_unmap(void __iomem ** pagepp, struct resource ** regionpp);

#else

/**
 * Map a region of physical address space to user virtual address space.
 * @param start is the physical address of the region.
 * @param length is the size of the region in octets.
 * @param startp points to where the start address for the unmap is
 * returned.
 * @param lengthp points to where the length for the unmap is returned.
 * @return a user virtual address if successful, NULL otherwise.
 */
extern void * diminuto_map_map(uintptr_t start, size_t length, void ** startp, size_t * lengthp);

/**
 * Unmap a region of user virtual address space that was previously mapped.
 * @param startp points to where the start address from the map was
 * returned.
 * @param lengthp points to where the length from the map was returned.
 * @return 0 if successful, !0 otherwise.
 */
extern int diminuto_map_unmap(void ** startp, size_t * lengthp);

/**
 * Set the minimum physical address which may be mapped. Setting this
 * value to non-zero prevents the null page from being mapped. Setting it to
 * zero allows the null page to be mapped so that null pointers will actually
 * point to a valid portion of memory. Wackiness may ensue. This function
 * can only be called by a process with root privileges. It has no effect on
 * kernels prior to 2.6.26, and only then if CONFIG_SECURITY is enabled.
 * @param minimum is the minimum physical address which may be mapped.
 * @return 0 if successful or if the operation is not supported, !0 otherwise.
 */
extern int diminuto_map_minimum(uintptr_t minimum);

#endif

#endif
