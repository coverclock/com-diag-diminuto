/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_ABSOLUTE_
#define _H_COM_DIAG_DIMINUTO_ABSOLUTE_

/**
 * @file
 *
 * Copyright 2020 Digital Aggregates Corporation, Colorado, USA.
 * Licensed under the terms in LICENSE.txt.
 * Provides inline absolute value function for stdint signed integer types.
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

#define abs8(_VALUE_) diminuto_absolute_int8_t_f(_VALUE_)
#define abs16(_VALUE_) diminuto_absolute_int16_t_f(_VALUE_)
#define abs32(_VALUE_) diminuto_absolute_int32_t_f(_VALUE_)
#define abs64(_VALUE_) diminuto_absolute_int64_t_f(_VALUE_)

#endif
