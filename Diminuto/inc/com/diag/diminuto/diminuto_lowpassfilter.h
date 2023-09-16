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
 */
#define DIMINUTO_LOWPASSFILTER(_SAMPLE_, _ACCUMULATOR_, _COUNT_) \
    ((_ACCUMULATOR_) = ((((_COUNT_)++) == 0) \
        ? (_SAMPLE_) \
        : (((_ACCUMULATOR_) + (_SAMPLE_)) / 2) \
    ))

#endif
