/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_ABSOLUTE_
#define _H_COM_DIAG_DIMINUTO_ABSOLUTE_

/**
 * @file
 * @copyright Copyright 2020 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief Provides inline absolute value function for stdint signed integer types.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 */

#include <stdint.h>

#define COM_DIAG_DIMINUTO_ABSOLUTE(_TYPE_) \
static inline _TYPE_ diminuto_absolute_##_TYPE_##_f(_TYPE_ value) \
{ \
    return (value < 0) ? -value : value; \
} \

COM_DIAG_DIMINUTO_ABSOLUTE(int8_t)
COM_DIAG_DIMINUTO_ABSOLUTE(int16_t)
COM_DIAG_DIMINUTO_ABSOLUTE(int32_t)
COM_DIAG_DIMINUTO_ABSOLUTE(int64_t)

/**
 * @def abs8
 * Return the absolute value of the 8-bit value of  @a _VALUE_.
 */
#define abs8(_VALUE_) diminuto_absolute_int8_t_f(_VALUE_)

/**
 * @def abs16
 * Return the absolute value of the 16-bit value of  @a _VALUE_.
 */
#define abs16(_VALUE_) diminuto_absolute_int16_t_f(_VALUE_)

/**
 * @def abs32
 * Return the absolute value of the 32-bit value of  @a _VALUE_.
 */
#define abs32(_VALUE_) diminuto_absolute_int32_t_f(_VALUE_)

/**
 * @def abs64
 * Return the absolute value of the 64-bit value of  @a _VALUE_.
 */
#define abs64(_VALUE_) diminuto_absolute_int64_t_f(_VALUE_)

#endif
