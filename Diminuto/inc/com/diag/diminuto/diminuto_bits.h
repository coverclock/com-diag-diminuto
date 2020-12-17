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
 * indices, offsets, and masks.
 */

#include "com/diag/diminuto/diminuto_widthof.h"

#define diminuto_bits_index(_TYPE_, _BIT_) \
    ((_BIT_) / diminuto_widthof(_TYPE_))

#define diminuto_bits_offset(_TYPE_, _BIT_) \
    ((_BIT_) % diminuto_widthof(_TYPE_))

#define diminuto_bits_mask(_TYPE_, _BIT_) \
    (((_TYPE_)1) << diminuto_bits_offset(_TYPE_, _BIT_))

#define diminuto_bits_get(_ARRAY_, _TYPE_, _BIT_) \
    ((((_ARRAY_)[diminuto_bits_index(_TYPE_, _BIT)]) & diminuto_bits_mask(_TYPE_, _BIT_)) >> diminuto_bits_offset(_TYPE_, _BIT_))

#define diminuto_bits_set(_ARRAY_, _TYPE_, _BIT_, _VALUE_) \
    (((_ARRAY_)[diminuto_bits_index(_TYPE_, _BIT)]) |= diminuto_bits_mask(_TYPE_, _BIT_))

#endif
