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
 * Work In Progress!
 */

#include <sys/types.h>

typedef struct DiminutoRing {
    size_t capacity;
    size_t measure;
    size_t producer;
    size_t consumer;
} diminuto_ring_t;

static inline diminuto_ring_t * diminuto_ring_init(diminuto_ring_t * rp, size_t length) {
    rp->capacity = length;
    rp->measure = 0;
    rp->producer = 0;
    rp->consumer = 0;

    return rp;
}

static inline diminuto_ring_t * diminuto_ring_fini(diminuto_ring_t * rp) {
    return (diminuto_ring_t *)0;
}

static inline size_t diminuto_ring_used(const diminuto_ring_t * rp) {
    return rp->measure;
}

static inline size_t diminuto_ring_unused(const diminuto_ring_t * rp) {
    return (rp->capacity - rp->measure);
}

extern ssize_t diminuto_ring_produce(diminuto_ring_t * rp);

extern ssize_t diminuto_ring_consume(diminuto_ring_t * rp);

#endif
