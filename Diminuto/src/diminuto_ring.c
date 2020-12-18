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

ssize_t diminuto_ring_produce(diminuto_ring_t * rp)
{
    ssize_t index = -1;

    if (rp->measure < rp->capacity) {
        rp->measure += 1;
        index = rp->producer;
        rp->producer = (rp->producer + 1) % rp->capacity;
    }

    return index;
}

ssize_t diminuto_ring_consume(diminuto_ring_t * rp)
{
    ssize_t index = -1;

    if (rp->measure > 0) {
        rp->measure -= 1;
        index = rp->consumer;
        rp->consumer = (rp->consumer + 1) % rp->capacity;
    }

    return index;
}
