/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_MEMORY_
#define _H_COM_DIAG_DIMINUTO_MEMORY_

/**
 * @file
 *
 * Copyright 2013 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "com/diag/diminuto/diminuto_types.h"

/**
 * This feature likes to know the virtual page size and the cache line size
 * in bytes. If it can't figure it out from looking the system, these are the
 * values it uses. Or, you can define values, for example via the command line
 * -DCOM_DIAG_DIMINUTO_MEMORY_PAGESIZE_BYTES=4096
 * -DCOM_DIAG_DIMINUTO_MEMORY_LINESIZE_BYTES=64
 * and override both these defaults and any inferred values the code may glean
 * from the underlying platform. These two values, however they are set, had
 * better both be powers of two otherwise wackiness will ensue.
 */
enum DiminutoMemoryDefault {
	DIMINUTO_MEMORY_PAGESIZE_BYTES = 4096,
	DIMINUTO_MEMORY_LINESIZE_BYTES = 64,
};

/**
 * Return the size of virtual memory pages of the underlying platform.
 * Also optionally indicates the method by which the page size was determined.
 * @param methodp points to a variable into which the method is saved, or NULL.
 * @return the size of virtual memory pages in bytes or <0 if error.
 */
extern size_t diminuto_memory_pagesize(int * methodp);

/**
 * Return the size of level 1 cache lines of the underlying platform.
 * Also optionally indicates the method by which the line size was determined.
 * @param methodp points to a variable into which the method is saved, or NULL.
 * @return the size of cache lines in bytes or <0 of error.
 */
extern size_t diminuto_memory_linesize(int * methodp);

/**
 * Compute the smallest power of two that is greater than or equal to the
 * specified alignment.
 * @param alignment is any arbitrary value greater than or equal to zero.
 * @return a power of two (for example: 1, 2, 4, 8, etc.).
 */
extern size_t diminuto_memory_power(size_t alignment);

/**
 * Return true if the specified alignment is a power of two (for example: 1, 2,
 * 4, 8, etc.), false otherwise.
 * @param alignment is any arbitrary value greater than or equal to zero.
 * @return true if alignment is a power of two, false otherwise.
 */
extern int diminuto_memory_is_power(size_t alignment);

/**
 * Compute the effective size of an object given a specified alignment.
 * @param size is the desired size of an object.
 * @param alignment is the alignment of each object in the well; it MUST be
 * greater than zero AND a power of two (including one).
  */
extern size_t diminuto_memory_alignment(size_t size, size_t alignment);

#endif
