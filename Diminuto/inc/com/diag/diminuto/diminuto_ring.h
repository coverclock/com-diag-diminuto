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
 *
 * The function names are a little cumbersome, but I confess some of that
 * is deliberate on my part. I started off with the usual "get", "put",
 * and "peek". But I was afraid that that would be confusing to the code
 * reader who might be misled into thinking these functions were actually
 * storing and retrieving date from the ring buffer. These functions merely
 * manage the metadata, leaving the application to actually implement the
 * ring buffer in whatever way makes sense (a linked list, an array, a
 * bit map, etc.). For example, the ReaderWriter feature uses the Ring
 * producer and consumer indices to address bits in a Bits array to set
 * and clear.
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
    size_t capacity;    /**< The capacity of the ring in caller units. */
    size_t measure;     /**< The number of caller units in the ring. */
    size_t producer;    /**< Index of the next available slot in the ring. */
    size_t consumer;    /**< Index of the next used slot in the ring. */
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
static inline diminuto_ring_t * diminuto_ring_init(diminuto_ring_t * rp, size_t capacity) {
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
 * ACTIONS
 ******************************************************************************/

/**
 * Return the number of used slots in the ring.
 * @param rp points to the ring object.
 * @return the number of used slots in the ring.
 */
static inline size_t diminuto_ring_consumable(const diminuto_ring_t * rp) {
    return rp->measure;
}

/**
 * Return the number of unused slots in the ring.
 * @param rp points to the ring object.
 * @return the number of unused slots in the ring.
 */
static inline size_t diminuto_ring_producable(const diminuto_ring_t * rp) {
    return (rp->capacity - rp->measure);
}

/**
 * Request the index of the first of one or more unused slots
 * in the ring. If successful, the requested number of unused slots are
 * allocated.
 * @param rp points to the ring object.
 * @param request is the number of requested unused slots.
 * @return the index to the first unused slot or <0 if it cannot be met.
 */
extern ssize_t diminuto_ring_produces(diminuto_ring_t * rp, size_t request);


/**
 * Request the index of the first more unused slots in the ring.
 * If successful, one unused slot is allocated.
 * @param rp points to the ring object.
 * @return the index to the first unused slot or <0 if it cannot be met.
 */
static inline ssize_t diminuto_ring_produce(diminuto_ring_t * rp) {
    return diminuto_ring_produces(rp, 1);
}

/**
 * Request the index of the first of one or more used slots in the ring.
 * If successful, the requested number of used slots are freed.
 * @param rp points to the ring object.
 * @param request is the number of requested used slots.
 * @return the index to the first used slot or <0 if it cannot be met.
 */
extern ssize_t diminuto_ring_consumes(diminuto_ring_t * rp, size_t request);

/**
 * Request the index of the first of one or more used slots
 * in the ring. If successful, one used slot is freed.
 * @param rp points to the ring object.
 * @return the index to the first used slot or <0 if cannot be met.
 */
static inline ssize_t diminuto_ring_consume(diminuto_ring_t * rp) {
    return diminuto_ring_consumes(rp, 1);
}

/**
 * Request the index of the first of one or more used slots
 * in the ring. If successful, one used slot is returned for preview
 * but not consumed.
 * @param rp points to the ring object.
 * @return the index to the first used slot or <0 if it cannot be met.
 */
static inline ssize_t diminuto_ring_preview(diminuto_ring_t * rp) {
    return (rp->measure >= 1) ? rp->consumer : -1;
}

#endif
