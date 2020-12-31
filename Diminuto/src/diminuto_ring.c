/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2020 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is the implementation of the Ring feature.
 */

#include "com/diag/diminuto/diminuto_ring.h"

int diminuto_ring_producer_request(diminuto_ring_t * rp, unsigned int request)
{
    int result = -1;

    if ((rp->measure + request) <= rp->capacity) {
        rp->measure += request;
        result = rp->producer;
        rp->producer = (rp->producer + request) % rp->capacity;
    }

    return result;
}

int diminuto_ring_producer_revoke(diminuto_ring_t * rp, unsigned int request)
{
    int result = -1;

    if (rp->measure >= request) {
        rp->measure -= request;
        result = request;
        rp->producer = (rp->producer + rp->capacity - request) % rp->capacity;
    }

    return result;
}

int diminuto_ring_consumer_request(diminuto_ring_t * rp, unsigned int request)
{
    int result = -1;

    if (rp->measure >= request) {
        rp->measure -= request;
        result = rp->consumer;
        rp->consumer = (rp->consumer + request) % rp->capacity;
    }

    return result;
}

int diminuto_ring_consumer_revoke(diminuto_ring_t * rp, unsigned int request)
{
    int result = -1;

    if ((rp->measure + request) <= rp->capacity) {
        rp->measure += request;
        rp->consumer = (rp->consumer + rp->capacity - request) % rp->capacity;
        result = request;
    }

    return result;
}
