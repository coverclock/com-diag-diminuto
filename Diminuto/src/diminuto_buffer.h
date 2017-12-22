/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_BUFFER_PRIVATE_
#define _H_COM_DIAG_DIMINUTO_BUFFER_PRIVATE_

/**
 * @file
 *
 * Copyright 2015 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 *
 * This describes the buffer private API.
 */

#include <stddef.h>
#include <stdint.h>
#include "com/diag/diminuto/diminuto_buffer.h"

/**
 * This type defines the structure of the header that prepends every buffer
 * allocated from the buffer pool. The header is always eight bytes long, and
 * contains either a pointer to the next buffer on the linked list (if the
 * buffer is in the pool), or an index to the linked list from which the buffer
 * was allocated (if the buffer was originally allocated from the pool), or the
 * total size of the buffer including the header overhead (if the buffer was
 * malloc'ed from and will be free'ed back into the heap because it was larger
 * than the pool could accomodate).
 */
typedef struct DiminutoBuffer {
    union {
        uint64_t bits; /* Header is at least eight bytes. */
        size_t item;
        struct DiminutoBuffer * next;
    } header;
    char payload[0];
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

#endif
