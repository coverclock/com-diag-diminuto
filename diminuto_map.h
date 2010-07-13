/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_MAP_
#define _H_COM_DIAG_DIMINUTO_MAP_

/**
 * @file
 *
 * Copyright 2008 Digital Aggregates Corporation, Arvada CO 80001-0587 USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include <stdint.h>
#include <sys/types.h>

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
