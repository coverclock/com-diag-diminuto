/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_BREAD_PRIVATE_
#define _H_COM_DIAG_DIMINUTO_BREAD_PRIVATE_

/**
 * @file
 * @copyright Copyright 2021 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This defines the private API for the Buffered Read (BREAD) feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 */

#include "com/diag/diminuto/diminuto_bread.h"
#include "com/diag/diminuto/diminuto_assert.h"

/*
 *      P
 *      C            
 *      B           E
 *      v           v
 *      ............
 *
 *      B   C   P   E
 *      v   v   v   v
 *      ....XXXX....
 *
 *      B   P   C   E
 *      v   v   v   v
 *      XXXX....XXXX
 *
 *      P
 *      C
 *      B           E
 *      v           v
 *      XXXXXXXXXXXX
 */

/**
 * Conpute the number of contiguous bytes free in the feature buffer.
 * @param sp points to the Buffered Read object.
 * @return the number of contiguous bytes free, which may be zero.
 */
static inline size_t diminuto_bread_produceable(const diminuto_bread_t * sp) {
    size_t result = 0;

    if (sp->bread_free == 0) {
        /* Do nothing. */
    } else if (sp->bread_producer < sp->bread_consumer) {
        result = sp->bread_consumer - sp->bread_producer;
    } else {
        result = sp->bread_end - sp->bread_producer;
    }

    return result;
}

/**
 * Conpute the number of contiguous bytes available in the feature buffer.
 * @param sp points to the Buffered Read object.
 * @return the number of contiguous bytes available, which may be zero.
 */
static inline size_t diminuto_bread_consumeable(const diminuto_bread_t * sp) {
    size_t result = 0;

    if (sp->bread_used == 0) {
        /* Do nothing. */
    } else if (sp->bread_consumer < sp->bread_producer) {
        result = sp->bread_producer - sp->bread_consumer;
    } else {
        result = sp->bread_end - sp->bread_consumer;
    }

    return result;
}

/**
 * Update the state with the number of bytes produced (inserted).
 * @param sp points to the Buffered Read object.
 * @param ll is the number of bytes produced.
 */
static inline void diminuto_bread_produced(diminuto_bread_t * sp, size_t ll) {
    diminuto_assert(ll <= sp->bread_free);
    sp->bread_used += ll;
    sp->bread_free -= ll;
    sp->bread_producer += ll;
    if (sp->bread_producer == sp->bread_end) {
        sp->bread_producer = sp->bread_begin;
    }
}

/**
 * Update the state with the number of bytes consumed (removed).
 * @param sp points to the Buffered Read object.
 * @param ll is the number of bytes consumed.
 */
static inline void diminuto_bread_consumed(diminuto_bread_t * sp, size_t ll) {
    diminuto_assert(ll <= sp->bread_used);
    sp->bread_used -= ll;
    sp->bread_free += ll;
    if (sp->bread_used == 0) {
        sp->bread_producer = sp->bread_begin;
        sp->bread_consumer = sp->bread_begin;
    } else {
        sp->bread_consumer += ll;
        if (sp->bread_consumer == sp->bread_end) {
            sp->bread_consumer = sp->bread_begin;
        }
    }
}

#endif
