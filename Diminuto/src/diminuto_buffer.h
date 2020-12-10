/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_BUFFER_PRIVATE_
#define _H_COM_DIAG_DIMINUTO_BUFFER_PRIVATE_

/**
 * @file
 * @copyright Copyright 2015-2020 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This describes the Buffer private API.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This describes the Buffer private API.
 */

#include "com/diag/diminuto/diminuto_buffer.h"
#include <stddef.h>
#include <stdint.h>

/**
 * This type defines the structure of the header that prepends every buffer
 * allocated from the buffer pool. The header is always eight bytes long, and
 * contains either a pointer to the next buffer on the linked list (if the
 * buffer is in the pool), or an index to the linked list from which the buffer
 * was allocated (if the buffer was originally allocated from the pool), or the
 * total size of the buffer including the header overhead (if the buffer was
 * malloc'ed from and will be free'ed back into the heap because it was larger
 * than the pool could accomodate). The contents of the header can always be
 * inferred from context. N.B. pointers, and size_t, differ in size between
 * X86_64 processors and some ARM processors (including those with 32-bit
 * kernels even on 64-bit processors).
 */
typedef struct DiminutoBuffer {
    union {
        uint64_t align;                 /* Header is at least eight bytes. */
        uintptr_t item;                 /* Pool index or allocated size. */
        struct DiminutoBuffer * next;   /* Pointer to next buffer. */
    } header;
    uint64_t payload[0]; /* Will produce -pedantic warnings whereever used. */
} diminuto_buffer_t;

/**
 * This type defines the metadata we need to know about a buffer pool: the
 * number of quanta it has, the size of each quanta in bytes, and the linked
 * lists containing the buffers for each quanta.
 */
typedef struct DiminutoBufferMeta {
    size_t count;
    const size_t * sizes;
    diminuto_buffer_t ** pool;
} diminuto_buffer_meta_t;

/*******************************************************************************
 * EXPOSED FOR UNIT TESTING
 ******************************************************************************/

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
extern size_t diminuto_buffer_hash(size_t requested, size_t * actualp);

/**
 * Given an index from the header in an allocated buffer (the value of which
 * may be larger than the largest legitimate index into the pool), return the
 * actual buffer size (including header overhead).
 * @param item is the pool index.
 * @return the acutal buffer size in bytes.
 */
extern size_t diminuto_buffer_effective(size_t item);

/**
 * Return the number quanta in the active pool.
 * @return the number of quanta in the active pool.
 */
extern size_t diminuto_buffer_count(void);

/**
 * Given an index from the header in an allocated buffer (the value of which
 * may be larger than the largest legitimate index into the pool), return the
 * buffer size (not including header overhead).
 * @param item is the pool index.
 * @return the buffer payload size in bytes.
 */
extern size_t diminuto_buffer_size(size_t item);

/**
 * Given an index from the header in an allocated buffer (the value of which
 * may be larger than the largest legitimate index into the pool), return the
 * true if the pool has no freed buffers for that index.
 * @param item is the pool index.
 * @return true if the pool is empty for that index.
 */
extern size_t diminuto_buffer_isempty(size_t item);

#endif
