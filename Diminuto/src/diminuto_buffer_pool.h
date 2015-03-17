/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_BUFFER_POOL_PRIVATE_
#define _H_COM_DIAG_DIMINUTO_BUFFER_POOL_PRIVATE_

/**
 * @file
 *
 * Copyright 2015 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 *
 * This describes the buffer private pool API.
 */

#include <stddef.h>
#include "diminuto_buffer.h"

/**
 * Given a payload size request in bytes (not including header overhead),
 * determine the index for the linked list in the pool that can accomodate
 * a request of that size, and return the actual size in bytes (including
 * header overhead) of a buffer in that linked list. If the returned index
 * is larger than the largest legitimate index into the pool, than this size
 * request must be met by malloc'ing the buffer (including the header overhead)
 * from the heap, and will ultimately be free'ed back to the heap.
 * @param requested is the payload size request in bytes.
 * @param actualp points to a variable into which the actual size in bytes will
 * be returned.
 * @return an index into the pool.
 */
static inline unsigned int buffer_hash(size_t requested, size_t * actualp)
{
    size_t item;
    size_t actual;

    for (item = 0; (item < diminuto_buffer_pool.count) && (diminuto_buffer_pool.sizes[item] < requested); ++item) {
        continue;
    }

    actual = (item < diminuto_buffer_pool.count) ? diminuto_buffer_pool.sizes[item] : requested;

    *actualp = actual + sizeof(diminuto_buffer_t);

    return item;
}

/**
 * Given an index from the header in an allocated buffer (the value of which
 * may be larger than the largest legitimate index into the pool), return the
 * actual buffer size (including header overhead).
 * @param item is an index from the header in an allocated buffer.
 * @return the acutal buffer size in bytes.
 */
static inline size_t buffer_effective(size_t item)
{
    return (item < diminuto_buffer_pool.count) ? diminuto_buffer_pool.sizes[item] + sizeof(diminuto_buffer_t) : item;
}

/**
 * If fail is false (such that heap allocation is enabled), allocate a buffer
 * of the specified total size (including header overhead) from the heap.
 * @param size is the requested size (including overhead) in bytes.
 * @param fail cause the function to return null with errno set.
 * @return a pointer to the new buffer.
 */
static inline diminuto_buffer_t * buffer_malloc(size_t size, int fail)
{
	diminuto_buffer_t * buffer;

    if (!fail) {
        buffer = (diminuto_buffer_t *)malloc(size);
    } else {
        buffer = (diminuto_buffer_t *)0;
        errno = ENOMEM;
    }

    return buffer;
}

/**
 * Get a buffer from the pool with the specified index.
 * @param item is the pool index.
 * @return a pointer to the buffer or null if the pool was empty.
 */
static inline diminuto_buffer_t * buffer_get(size_t item)
{
    diminuto_buffer_t * buffer;

    if ((buffer = diminuto_buffer_pool.pool[item]) != (diminuto_buffer_t *)0) {
        diminuto_buffer_pool.pool[item] = buffer->header.next;
    }

    return buffer;
}

/**
 * Put a buffer back into the pool with the specified index. The index is _not_
 * checked against the pool index in the buffer header.
 * @param buffer points to the buffer.
 * @param item is the pool index.
 */
static inline void buffer_put(diminuto_buffer_t * buffer, size_t item)
{
    buffer->header.next = diminuto_buffer_pool.pool[item];
    diminuto_buffer_pool.pool[item] = buffer;
}

/**
 * Initialize the buffer header with either the pool index (if the index
 * indicates that the buffer came from a pool) or the actual total buffer
 * size (including header overhead).
 * @param buffer points to the buffer.
 * @param item is the pool index.
 * @param actual is the actual buffer size include header overhead in bytes.
 * @return a pointer to the payload portion of the buffer.
 */
static inline void * buffer_init(diminuto_buffer_t * buffer, size_t item, size_t actual)
{
    buffer->header.item = (item < diminuto_buffer_pool.count) ? item : actual;
    return buffer->payload;
}

/**
 * Return true if the pool index is beyond the limits of the pool (such that a
 * buffer with this index in its header must have come from the heap and will
 * be freed back to the heap).
 * @param item is the pool index.
 * @return true if it is a heap buffer, false if it is a pool buffer.
 */
static inline int buffer_isexternal(int item)
{
    return (item >= diminuto_buffer_pool.count);
}

/**
 * Return a pointer to the first buffer on the pool with the specified index or
 * null if that pool is empty. The index is not checked to see if it is too
 * large.
 * @param item is the pool index.
 * @return a pointer to a buffer or null.
 */
static inline diminuto_buffer_t * buffer_first(int item)
{
    return diminuto_buffer_pool.pool[item];
}

/**
 * Return the payload size in bytes of buffers in the pool with the specified
 * index. The index is not checked to see if it is too large.
 * @param item is the pool index.
 * @return the payload size of buffers in the pool with that index.
 */
static inline size_t buffer_size(int item)
{
    return diminuto_buffer_pool.sizes[item];
}

#endif
