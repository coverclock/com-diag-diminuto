/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2020 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 */

#include "com/diag/diminuto/diminuto_ring.h"

ssize_t diminuto_ring_produces(diminuto_ring_t * rp, size_t request)
{
    ssize_t result = -1;

    if ((rp->measure + request) <= rp->capacity) {
        rp->measure += request;
        result = rp->producer;
        rp->producer = (rp->producer + request) % rp->capacity;
    }

    return result;
}

ssize_t diminuto_ring_consumes(diminuto_ring_t * rp, size_t request)
{
    ssize_t result = -1;

    if (rp->measure >= request) {
        rp->measure -= request;
        result = rp->consumer;
        rp->consumer = (rp->consumer + request) % rp->capacity;
    }

    return result;
}
