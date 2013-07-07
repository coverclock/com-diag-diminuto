/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_WELL_
#define _H_COM_DIAG_DIMINUTO_WELL_

/**
 * @file
 *
 * Copyright 2013 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "com/diag/diminuto/diminuto_types.h"
#include "com/diag/diminuto/diminuto_list.h"

/**
 * This is the type of a well. (It is in fact an array of two lists, the first
 * containing nodes pointing to free objects, the second containing unused
 * nodes).
 */
typedef diminuto_list_t diminuto_well_t;

/**
 * Return the size of virtual memory pages of the underlying platform.
 * @return the size of virtual memory pages in bytes or <0 if error.
 */
extern ssize_t diminuto_well_pagesize();

/**
 * Return the size of level 1 cache lines of the underlying platform.
 * @return the size of cache lines in bytes or <0 of error.
 */
extern ssize_t diminuto_well_linesize();

/**
 * Allocate a well for objects of the specified size. The well is initially
 * full.
 * @param size is the desired size of objects to be kept in the well.
 * @param count is the desired number of objects in the well.
 * @return a pointer to the well.
 */
extern diminuto_well_t * diminuto_well_init(size_t size, size_t count);

/**
 * Release all of memory associated with the well.
 * @param wellp points to a well.
 */
extern void diminuto_well_fini(diminuto_well_t * wellp);

/**
 * Allocate an object from the well. If an object is in the well, it
 * is removed from the well and a pointer to it is returned. If the well
 * is empty, a new object of the correct size is allocated from the heap
 * and a pointer to it is returned.
 * @param wellp points to the well.
 * @return a pointer to an object from the well or NULL for failure.
 */
extern void * diminuto_well_alloc(diminuto_well_t * wellp);

/**
 * Free an object back to the well. The object is returned to the well.
 * @param wellp points to the well.
 * @return 0 for success or <0 for failure.
 */
extern int diminuto_well_free(diminuto_well_t * wellp, void * pointer);

#endif
