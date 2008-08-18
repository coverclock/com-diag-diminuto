/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_DIMINUTO_MAP_
#define _H_DIMINUTO_MAP_

/**
 * @file
 *
 * Copyright 2008 Digital Aggregates Corporation, Arvada CO 80001-0587 USA<BR>
 * Licensed under the GNU GPL V2<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include <stdint.h>
#include <sys/types.h>

/**
 * Map a region of physical address space to virtual address space.
 * @param start is the physical address of the region.
 * @param length is the size of the region in octets.
 * @param startp points to where the start address for the unmap is returned.
 * @param lengthp points to where the length for the unmap is returned.
 * @return a virtual address if successful, NULL otherwise.
 */
extern void * diminuto_map(uintptr_t start, size_t length, void ** startp, size_t * lengthp);

/**
 * Unmap a region of virtual address space that was previously mapped.
 * @param start is the start address returned by map.
 * @param length is the length returned by map.
 * @return 0 if successful, !0 otherwise.
 */
extern int diminuto_unmap(void * base, size_t size);

#endif
