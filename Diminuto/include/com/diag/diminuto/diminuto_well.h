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
 *
 * A well is like a Diminuto pool except the linked lists and allocated user
 * objects are kept in separately allocated sections of memory. Both sections
 * are page aligned, and objects in the well can have any specified alignment
 * in memory instead of the usual eight-byte alignment. Also, a well is of
 * fixed size; additional objects are never allocated after initialization.
 */

#include "com/diag/diminuto/diminuto_types.h"
#include "com/diag/diminuto/diminuto_list.h"

/**
 * This is the type of a well. It is in fact an array of lists, the first
 * containing nodes pointing to free objects, the second containing unused
 * nodes, and the remainder the nodes themselves.
 */
typedef diminuto_list_t diminuto_well_t;

/**
 * These are handy indices that expose the composition of the list array.
 * Note that the data field of nodes on the USED list have no meaning.
 */
enum DiminutoWellIndex {
	DIMINUTO_WELL_FREE = 0,
	DIMINUTO_WELL_USED = 1,
	DIMINUTO_WELL_NODE = 2,
};

/**
 * This feature likes to know the virtual page size and the cache line size
 * in bytes. If it can't figure it out from looking the system, these are the
 * values it uses.
 */
enum DiminutoWellDefault {
	DIMINUTO_WELL_PAGESIZE = 4096, /* Or specify via -DDIMINUTO_PAGESIZE */
	DIMINUTO_WELL_LINESIZE = 64,   /* Or specify via -DDIMINUTO_LINESIZE */
};

/**
 * Return the size of virtual memory pages of the underlying platform.
 * @return the size of virtual memory pages in bytes or <0 if error.
 */
extern size_t diminuto_well_pagesize(void);

/**
 * Return the size of level 1 cache lines of the underlying platform.
 * @return the size of cache lines in bytes or <0 of error.
 */
extern size_t diminuto_well_linesize(void);

/**
 * Compute the smallest power of two that is greater than or equal to the
 * specified alignment.
 * @param alignment is any arbitrary value greater than or equal to zero.
 * @return a power of two (for example: 1, 2, 4, 8, etc.).
 */
extern size_t diminuto_well_power(size_t alignment);

/**
 * Return true if the specified alignment is a power of two (for example: 1, 2,
 * 4, 8, etc.), false otherwise.
 * @param alignment is any arbitrary value greater than or equal to zero.
 * @return true if alignment is a power of two, false otherwise.
 */
extern int diminuto_well_is_power(size_t alignment);

/**
 * Compute the effective size of an object given a specified alignment.
 * @param size is the desired size of an object..
 * @param alignment is the alignment of each object in the well; it MUST be
 * greater than zero AND a power of two (including one).
  */
extern size_t diminuto_well_alignment(size_t size, size_t alignment);

/**
 * Allocate a fixed-size well for objects of the specified size. The well is
 * initially full and never grows. Each object in the well is memory aligned on
 * the specified alignment. The alignment should be at least that required for
 * the first field in the object. Most memory allocators (including Diminuto
 * pool) align on (at least) an eight-byte boundary. However, when smaller
 * alignments can be used, they can lead to fitting more data in a page and in
 * a cache line. So field packing or careful ordering of fields in an object
 * can lead to more efficient memory use (but possible less efficient run-time).
 * Another approach is to align objects on a cache line, which may waste space
 * but avoids word tearing. As usual, life is a series of trade-offs. A zero
 * alignment can be used to specify the cache line size returned by
 * diminuto_well_cacheline(); values greater than zero will be adjusted to the
 * smallest power of two greater than or equal to the specified value.
 * @param size is the desired size of objects to be kept in the well.
 * @param count is the desired number of objects in the well.
 * @param alignment is the alignment of each object in the well.
 * @return a pointer to the well.
 */
extern diminuto_well_t * diminuto_well_init(size_t size, size_t count, size_t alignment);

/**
 * Release all of memory associated with the well. This includes not just
 * the objects in the well, but the well itself.
 * @param wellp points to a well.
 */
extern void diminuto_well_fini(diminuto_well_t * wellp);

/**
 * Allocate an object from the well. If an object is in the well, it
 * is removed from the well and a pointer to it is returned. If the well
 * is empty, a null pointer is returned.
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
