/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_BITS_
#define _H_COM_DIAG_DIMINUTO_BITS_

/**
 * @file
 * @copyright Copyright 2020 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * Provides helper macros (masquerading as functions) to compute bit
 * indices, offsets, and masks. Works best with unsigned types,
 * otherwise C may promote the value to a wider type and extend
 * its sign. See the unit test for specific examples.
 */

#include "com/diag/diminuto/diminuto_widthof.h"
#include <sys/types.h>
#include <stdint.h>

typedef size_t diminuto_bits_size_t;

typedef uint64_t diminuto_bits_mask_t;

#define diminuto_bits_index(_TYPE_, _BIT_) \
    (((diminuto_bits_size_t)(_BIT_)) / diminuto_widthof(_TYPE_))

#define diminuto_bits_offset(_TYPE_, _BIT_) \
    (((diminuto_bits_size_t)(_BIT_)) % diminuto_widthof(_TYPE_))

#define diminuto_bits_mask(_TYPE_) \
    ((_TYPE_)(~((diminuto_bits_mask_t)0)))

#define diminuto_bits_or(_TYPE_, _BIT_) \
    ((_TYPE_)(((_TYPE_)1) << diminuto_bits_offset(_TYPE_, _BIT_)))

#define diminuto_bits_and(_TYPE_, _BIT_) \
    ((_TYPE_)(~(((_TYPE_)1) << diminuto_bits_offset(_TYPE_, _BIT_))))

#define diminuto_bits_get(_ARRAY_, _TYPE_, _BIT_) \
    ((_TYPE_)(((((_ARRAY_)[diminuto_bits_index(_TYPE_, _BIT)]) & diminuto_bits_one(_TYPE_, _BIT_)) >> diminuto_bits_offset(_TYPE_, _BIT_))))

#define diminuto_bits_set(_ARRAY_, _TYPE_, _BIT_) \
    (((_ARRAY_)[diminuto_bits_index(_TYPE_, _BIT)]) |= diminuto_bits_one(_TYPE_, _BIT_))

#define diminuto_bits_clear(_ARRAY_, _TYPE_, _BIT_) \
    (((_ARRAY_)[diminuto_bits_index(_TYPE_, _BIT)]) &= diminuto_bits_zero(_TYPE_, _BIT_))

#endif
