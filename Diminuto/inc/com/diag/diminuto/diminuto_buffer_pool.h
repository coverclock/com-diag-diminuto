/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_BUFFER_POOL_
#define _H_COM_DIAG_DIMINUTO_BUFFER_POOL_

/**
 * @file
 *
 * Copyright 2015-2016 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 *
 * This is the public API for the generic buffer pool feature. The buffer
 * feature is built on top of this API. However, this feature can be used
 * by the application to implement its own buffer API.
 */

#include <stddef.h>

/**
 * This type defines a structure used implement a generic buffer pool. The pool
 * itself is an array of linked list heads. Each head will contain a linked list
 * of freed buffers whose payload is the size in bytes of the corresponding
 * entry in the sizes array. The sizes array and the pool array both have count
 * entries. The values in the sizes array must be monotonically increasing.
 * The linked list heads are declared void to avoid exposing the internal
 * buffer format. An empty linked list head must be initialized to null.
 */
typedef struct DiminutoBufferPool {
    size_t count; /**< Number of entries in each array. */
    const size_t * sizes; /**< Array of sizes of each buffer quanta in bytes. */
    void ** pool; /**< Array of null void pointers as linked list heads. */
} diminuto_buffer_pool_t;

/**
 * Get a buffer from the appropriate linked list in a pool based on the
 * requested minimum payload size in bytes. If the appropriate linked list is
 * empty, or the the pool cannot accomodate the requested size, and if dynamic
 * memory allocation is not explicitly disabled, allocate a buffer from the
 * heap.
 * @param poolp points to the pool.
 * @param size is the minimum payload size in bytes.
 * @param nomalloc if true disables allocating a buffer from the heap.
 * @return a pointer to the payload portion of the buffer.
 */
extern void * diminuto_buffer_pool_get(diminuto_buffer_pool_t * poolp, size_t size, int nomalloc);

/**
 * Put a buffer back on to the appropriate linked list in a pool, or if the
 * buffer is too large to be accomodated by the pool, free it back to the heap.
 * @param poolp points to the pool.
 * @param ptr points to the payload portion of the buffer.
 */
extern void diminuto_buffer_pool_put(diminuto_buffer_pool_t * poolp, void * ptr);

/**
 * Free all storage associated with the buffer pool.
 * @param poolp points to the pool.
 */
extern void diminuto_buffer_pool_fini(diminuto_buffer_pool_t * poolp);

/**
 * Dynamically allocate the specified number of buffers with the minimum
 * specified payload size from the heap and put them on the appropriate linked
 * list in the pool.
 * @param poolp points to the pool.
 * @param nmemb is the number of buffers to dynamically allocate.
 * @param size is the minimum payload size of the buffers in bytes.
 * @return the total number of bytes allocated.
 */
extern size_t diminuto_buffer_pool_prealloc(diminuto_buffer_pool_t * poolp, size_t nmemb, size_t size);

/**
 * Log information about a pool to the debug log.
 * @param poolp points to the pool.
 * @return the total number of bytes currently allocated by buffers in the pool.
 */
extern size_t diminuto_buffer_pool_log(diminuto_buffer_pool_t * poolp);

#endif
