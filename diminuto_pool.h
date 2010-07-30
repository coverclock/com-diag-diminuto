/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_POOL_
#define _H_COM_DIAG_DIMINUTO_POOL_

/**
 * @file
 *
 * Copyright 2010 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "diminuto_types.h"
#include "diminuto_list.h"

typedef diminuto_list diminuto_pool;

/**
 * Initialize a pool for objects of the specified size. The pool is initially
 * empty. Do not initialize a pool that already has objects stored in it; that
 * will result in a memory leak.
 * @param poolp points to an uninitialized pool structure.
 * @param size is the desired size of objects to be kept in the pool.
 * @return a pointer to the pool.
 */
extern diminuto_pool * diminuto_pool_init(diminuto_pool * poolp, size_t size);

/**
 * Release all of the objects stored in the pool, freeing their memory.
 * @param poolp points to a pool.
 * @return a pointer to the pool.
 */
extern diminuto_pool * diminuto_pool_fini(diminuto_pool * poolp);

/**
 * Allocate an object from the pool. If an object is in the pool, it
 * is removed from the pool and a pointer to it is returned. If the pool
 * is empty, a new object of the correct size is allocated from the heap
 * and a pointer to it is returned.
 * @param poolp points to the pool.
 * @return a pointer to an object of the desired size.
 */
extern void * diminuto_pool_alloc(diminuto_pool * poolp);

/**
 * Free an object back to the pool. The object is returned to the pool.
 * @param pointer points to the object.
 */
extern void diminuto_pool_free(void * pointer);

#endif
