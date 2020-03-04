/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_POOL_
#define _H_COM_DIAG_DIMINUTO_POOL_

/**
 * @file
 *
 * Copyright 2010-2015 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in LICENSE.txt<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 */

#include "com/diag/diminuto/diminuto_types.h"
#include "com/diag/diminuto/diminuto_list.h"

/**
 * This describes the pool head. The allocation size of the type of object held
 * in the pool or allocated from the heap is kept in the data field of the list
 * structure. Hence this field cannot be used for any other purpose.
 */
typedef diminuto_list_t diminuto_pool_t;

/**
 * This describes the memory layout of the type of object held in the pool or
 * allocated from the heap. Normally, the application only sees a pointer to the
 * payload portion of the object (the part it cares about, whatever it is). But
 * the format of the pool object is deliberately exposed so that an application
 * may use, for example, diminuto_containerof(), to recover the pointer to the
 * pool object so that the link field can be reused to queue the object on
 * other lists, a common application idiom. However, note that a pointer to
 * the pool from whence it came is kept in the data field of the link field of
 * the pool object so that the object can be freed without the caller knowing
 * the object's originating pool. Hence this field cannot be used for any other
 * purpose.
 */
typedef struct DiminutoPoolObject {
    diminuto_list_t link;
    uint64_t payload[0]; /* Will produce -pedantic warnings whereever used. */
} diminuto_pool_object_t;

/**
 * Initialize a pool for objects of the specified size. The pool is initially
 * empty. Do not initialize a pool that already has objects stored in it; that
 * will result in a memory leak.
 * @param poolp points to an uninitialized pool structure.
 * @param size is the desired size of objects to be kept in the pool.
 * @return a pointer to the pool.
 */
extern diminuto_pool_t * diminuto_pool_init(diminuto_pool_t * poolp, size_t size);

/**
 * Release all of the objects stored in the pool, freeing their memory.
 * @param poolp points to the pool.
 * @return a pointer to the pool.
 */
extern diminuto_pool_t * diminuto_pool_fini(diminuto_pool_t * poolp);

/**
 * Allocate an object from the pool. If an object is in the pool, it
 * is removed from the pool and a pointer to it is returned. If the pool
 * is empty, a new object of the correct size is allocated from the heap
 * and a pointer to it is returned.
 * @param poolp points to the pool.
 * @return a pointer to an object of the desired size.
 */
extern void * diminuto_pool_alloc(diminuto_pool_t * poolp);

/**
 * Free an object back to the pool. The object is returned to the pool from
 * whence it came.
 * @param pointer points to the object.
 */
extern void diminuto_pool_free(void * pointer);

#endif
