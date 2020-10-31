/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2016 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is the implementation of the Fletcher feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is the implementation of the Fletcher feature.
 */

#include "com/diag/diminuto/diminuto_fletcher.h"

uint16_t diminuto_fletcher_16(const void * buffer, size_t length, uint8_t * ap, uint8_t * bp)
{
    const uint8_t * pp;
    uint16_t a;
    uint16_t b;

    a = *ap;
    b = *bp;

    for (pp = (const uint8_t *)buffer; length > 0; --length) {
        a = (a + *(pp++)) % 0xff;
        b = (b + a) % 0xff;
    }

    *ap = a;
    *bp = b;

    return (*bp << 8) | *ap;
}
