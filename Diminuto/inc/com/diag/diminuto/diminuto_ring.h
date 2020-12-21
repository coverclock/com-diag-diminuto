/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_RING_
#define _H_COM_DIAG_DIMINUTO_RING_

/**
 * @file
 * @copyright Copyright 2020 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * Ring provides a generic infrastructure to manage ring buffers (a.k.a.
 * circular buffers). The buffer itself is not implemented, merely the
 * mechanism to manage it for producers (who put data into the buffer) and
 * consumers (who remove data from the buffer).
 */

/*******************************************************************************
 * PREREQUISITES
 ******************************************************************************/

#include <sys/types.h>

/*******************************************************************************
 * TYPES
 ******************************************************************************/

/**
 * This defines the metadata used to manage a ring buffer.
 */
typedef struct DiminutoRing {
    unsigned int capacity;  /**< The capacity of the ring in abstract slots. */
    unsigned int measure;   /**< The number of slots in the ring. */
    unsigned int producer;  /**< Index of next free slot in the ring. */
    unsigned int consumer;  /**< Index of the next used slot in the ring. */
} diminuto_ring_t;

/*******************************************************************************
 * GENERATORS
 ******************************************************************************/

/**
 * @def DIMINUTO_RING_INITIALIZER
 * Generates code to initialize the declaration of a Ring with the given
 * @a _CAPACITY_ in caller-defined units.
 */
#define DIMINUTO_RING_INITIALIZER(_CAPACITY_) \
    { \
        (_CAPACITY_), \
        0, \
        0, \
        0, \
    }

/*******************************************************************************
 * STRUCTORS
 ******************************************************************************/

/**
 * Initialize a Ring with a capacity defined in caller-defined units.
 * @param rp points to the ring object.
 * @param capacity is the capacity of the ring buffer.
 * @return a pointer to the ring object if successful, NULL otherwise.
 */
static inline diminuto_ring_t * diminuto_ring_init(diminuto_ring_t * rp, unsigned int capacity) {
    rp->capacity = capacity;
    rp->measure = 0;
    rp->producer = 0;
    rp->consumer = 0;

    return rp;
}

/**
 * Release any dynamically acquired resources associated with the ring object.
 * @param rp points to the ring object.
 * @return NULL is successful, a pointer to the ring object otherwise.
 */
static inline diminuto_ring_t * diminuto_ring_fini(diminuto_ring_t * rp) {
    return (diminuto_ring_t *)0;
}

/*******************************************************************************
 * HELPERS
 ******************************************************************************/

/**
 * Return the number of used slots in the ring.
 * @param rp points to the ring object.
 * @return the number of used slots in the ring.
 */
static inline unsigned int diminuto_ring_used(const diminuto_ring_t * rp) {
    return rp->measure;
}

/**
 * Return the number of free slots in the ring.
 * @param rp points to the ring object.
 * @return the number of free slots in the ring.
 */
static inline unsigned int diminuto_ring_free(const diminuto_ring_t * rp) {
    return (rp->capacity - rp->measure);
}

/**
 * Given an index of a slot in the ring, return the index of the next slot.
 * This does not affect the allocation of used or free slots in the ring.
 * No check is made as to whether the next slot is used or free. This
 * function is useful when the caller has requested multiple slots and has
 * then received the index of the first slot in that request.
 * @param rp points to the ring object.
 * @param index is the previous index.
 * @return the next index.
 */
static inline unsigned int diminuto_ring_next(const diminuto_ring_t * rp, unsigned int index) {
    return (index + 1) % rp->capacity;
}

/**
 * Request the index of the first of one or more used slots
 * in the ring. If successful, one used slot is returned for preview
 * but not consumed. This does not affect the allocation of used or free
 * slots in the ring. This function is useful for algoritums that need
 * one used slot lookahead. (There is no reason to peek at free slots.
 * The caller can interrogate the ring for the number of free slots.)
 * @param rp points to the ring object.
 * @return the index to the first used slot or <0 if it cannot be met.
 */
static inline int diminuto_ring_peek(diminuto_ring_t * rp) {
    return (rp->measure >= 1) ? rp->consumer : -1;
}

/*******************************************************************************
 * ACTIONS
 ******************************************************************************/

/**
 * Request the index of the first of one or more free slots
 * in the ring. If successful, the requested number of free slots are
 * allocated.
 * @param rp points to the ring object.
 * @param request is the number of requested free slots.
 * @return the index to the first free slot or <0 if it cannot be met.
 */
extern int diminuto_ring_produce_request(diminuto_ring_t * rp, unsigned int request);


/**
 * Request the index of the first more free slots in the ring.
 * If successful, one free slot is allocated.
 * @param rp points to the ring object.
 * @return the index to the first free slot or <0 if it cannot be met.
 */
static inline int diminuto_ring_produce(diminuto_ring_t * rp) {
    return diminuto_ring_produce_request(rp, 1);
}

/**
 * Request the index of the first of one or more used slots in the ring.
 * If successful, the requested number of used slots are freed.
 * @param rp points to the ring object.
 * @param request is the number of requested used slots.
 * @return the index to the first used slot or <0 if it cannot be met.
 */
extern int diminuto_ring_consume_request(diminuto_ring_t * rp, unsigned int request);

/**
 * Request the index of the first of one or more used slots
 * in the ring. If successful, one used slot is freed.
 * @param rp points to the ring object.
 * @return the index to the first used slot or <0 if cannot be met.
 */
static inline int diminuto_ring_consume(diminuto_ring_t * rp) {
    return diminuto_ring_consume_request(rp, 1);
}

#endif
