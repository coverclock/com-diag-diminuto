/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_LOWPASSFILTER_
#define _H_COM_DIAG_DIMINUTO_LOWPASSFILTER_

/**
 * @file
 * @copyright Copyright 2023 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief Implements a simple low pass filter macro.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * THIS IS A WORK IN PROGRESS.
 */

/**
 * @def DIMINUTO_LOWPASSFILTER
 * Apply a low pass filter to a data sample. The sample @a _SAMPLE_
 * is averaged with the prior @a _ACCUMULATOR_ and the result is
 * stored back into the new @a _ACCUMULATOR_ and also returned as a
 * value. The total number of samples is maintained in @a _COUNT_.
 * _ACCUMULATOR_ and _COUNT_ must have been initialized to zero. The
 * values for _SAMPLE_, _ACCUMULATOR_, and _COUNT_ may be accessed more
 * than once and should be lvalues. Some thought should be taken as to
 * the data types for the three variables, for example long for _SAMPLE_,
 * long long for _ACCUMULATOR_, and size_t for _COUNT_.
 */
#define DIMINUTO_LOWPASSFILTER(_SAMPLE_, _ACCUMULATOR_, _COUNT_) \
    ((_ACCUMULATOR_) = ((((_COUNT_)++) == 0) \
        ? (_SAMPLE_) \
        : (((_ACCUMULATOR_) + (_SAMPLE_)) / 2) \
    ))

#endif
