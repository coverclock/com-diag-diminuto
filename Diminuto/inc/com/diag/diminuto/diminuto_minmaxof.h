/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_MINMAXOF_
#define _H_COM_DIAG_DIMINUTO_MINMAXOF_

/**
 * @file
 * @copyright Copyright 2020 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief Implements generic integer minimum and maximum code generators.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * The minimumof operator generates the minimum value of a signed or
 * unsigned integer type. The maximumof operator generates the maximum value
 * of a signed or unsigned integer type.
 */

#include "com/diag/diminuto/diminuto_widthof.h"

/**
 * @def diminuto_issigned
 * Compute true of an integer of type @a _TYPE_ is signed, false otherwise.
 */
#define diminuto_issigned(_TYPE_) (((_TYPE_)(~(_TYPE_)0)) < 0)

#ifndef issigned
    /**
     * @def issigned
     * Return true of an integer of type @a _TYPE_ is signed, false otherwise.
     */
#   define issigned(_TYPE_) diminuto_issigned(_TYPE_)
#endif

/**
 * @def diminuto_minimumof
 * Compute the minimum value of an integer of the type @a _TYPE_.
 */
#define diminuto_minimumof(_TYPE_) (diminuto_issigned(_TYPE_) ? ((_TYPE_)(((_TYPE_)1)<<(diminuto_widthof(_TYPE_)-1))) : ((_TYPE_)0))

#ifndef minimumof
    /**
     * @def minimumof
     * Cimpute the minimum value of an integer of the type @a _TYPE_.
     */
#   define minimumof(_TYPE_) diminuto_minimumof(_TYPE_)
#endif

/**
 * @def diminuto_maximum
 * Compute the maximum value of an integer of the type @a _TYPE_.
 */
#define diminuto_maximumof(_TYPE_) (diminuto_issigned(_TYPE_) ? ((_TYPE_)(~(((_TYPE_)1)<<(diminuto_widthof(_TYPE_)-1)))) : ((_TYPE_)(~((_TYPE_)0))))

#ifndef maximumof
    /**
     * @def maximum
     * Cimpute the maximum value of an integer of the type @a _TYPE_.
     */
#   define maximumof(_TYPE_) diminuto_maximumof(_TYPE_)
#endif

#endif
