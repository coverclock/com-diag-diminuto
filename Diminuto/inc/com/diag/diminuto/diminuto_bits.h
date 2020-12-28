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
 * Provides helper macros to compute bit indices, offsets, and masks.
 * Works best with unsigned types, otherwise C may promote the value
 * to a wider type and extend its sign. See the unit test for specific
 * examples.
 *
 * Inspired by FD_CLR. FD_SET, and FD_ISSET from select(2) and
 * defined in <sys/select.h>.
 *
 * C99 (6.3.1.1): "If an int can represent all values of the original
 * type, the value is converted to an int; otherwise, it is converted
 * to an unsigned int. These are called the integer promotions. All
 * other types are unchanged by the integer promotions."
 */

#include "com/diag/diminuto/diminuto_widthof.h"
#include <sys/types.h>

/**
 * @def DIMINUTO_BITS_COUNT
 * Given a bit field array of type @a _TYPE_ and a desired minimum
 * number of bits @a _BITS_, compute the necessaary dimension of the
 * array.
 */
#define DIMINUTO_BITS_COUNT(_TYPE_, _BITS_) \
    ((size_t)(((_BITS_) + diminuto_widthof(_TYPE_) - 1) / diminuto_widthof(_TYPE_)))

/**
 * @def DIMINUTO_BITS_INDEX
 * Given a bit field array of type @a _TYPE_, compute the array index
 * for bit @a _BIT_.
 */
#define DIMINUTO_BITS_INDEX(_TYPE_, _BIT_) \
    (((size_t)(_BIT_)) / diminuto_widthof(_TYPE_))

/**
 * @def DIMINUTO_BITS_OFFSET
 * Given a bit field array of type @a _TYPE_, compute the bit offset
 * for bit @a _BIT_, where the bit 0x1 is at offset 0.
 */
#define DIMINUTO_BITS_OFFSET(_TYPE_, _BIT_) \
    (((size_t)(_BIT_)) % diminuto_widthof(_TYPE_))

/**
 * @def DIMINUTO_BITS_MASK
 * GIven a bit field array of type @a _TYPE_, compute a mask that
 * keeps all the bits for the width of that type.
 */
#define DIMINUTO_BITS_MASK(_TYPE_) \
    ((_TYPE_)(~((_TYPE_)0)))

/**
 * @def DIMINUTO_BITS_OR
 * Given a bit field array of type @a _TYPE_, compute an OR mask
 * that can be used to set bit @a _BIT_.
 */
#define DIMINUTO_BITS_OR(_TYPE_, _BIT_) \
    ((_TYPE_)(((_TYPE_)1) << DIMINUTO_BITS_OFFSET(_TYPE_, _BIT_)))

/**
 * @def DIMINUTO_BITS_AND
 * Given a bit field array of type @a _TYPE_, compute an AND mask
 * that can be used to clear bit @a _BIT_.
 */
#define DIMINUTO_BITS_AND(_TYPE_, _BIT_) \
    ((_TYPE_)(~(((_TYPE_)1) << DIMINUTO_BITS_OFFSET(_TYPE_, _BIT_))))

/**
 * @def DIMINUTO_BITS_GET
 * Given a bit field array named @a _ARRAY_ of type @a _TYPE_, extract
 * the bit @a _BIT_ yielding a 0 or a 1.
 */
#define DIMINUTO_BITS_GET( _TYPE_, _BIT_, _ARRAY_) \
    ((_TYPE_)(((((_ARRAY_)[DIMINUTO_BITS_INDEX(_TYPE_, _BIT_)]) & DIMINUTO_BITS_OR(_TYPE_, _BIT_)) >> DIMINUTO_BITS_OFFSET(_TYPE_, _BIT_))))

/**
 * @def DIMINUTO_BITS_SET
 * Given a bit field array named @a _ARRAY_ of type @a _TYPE_, set
 * the bit @a _BIT_.
 */
#define DIMINUTO_BITS_SET(_TYPE_, _BIT_, _ARRAY_) \
    ((void)(((_ARRAY_)[DIMINUTO_BITS_INDEX(_TYPE_, _BIT_)]) |= DIMINUTO_BITS_OR(_TYPE_, _BIT_)))

/**
 * @def DIMINUTO_BITS_CLEAR
 * Given a bit field array named @a _ARRAY_ of type @a _TYPE_, clear
 * the bit @a _BIT_.
 */
#define DIMINUTO_BITS_CLEAR(_TYPE_, _BIT_, _ARRAY_) \
    ((void)(((_ARRAY_)[DIMINUTO_BITS_INDEX(_TYPE_, _BIT_)]) &= DIMINUTO_BITS_AND(_TYPE_, _BIT_)))

#endif
