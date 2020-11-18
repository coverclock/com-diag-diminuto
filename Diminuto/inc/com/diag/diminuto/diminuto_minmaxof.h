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
 *
 * The minimumof operator generates the minimum value of a signed or
 * unsigned integer type. The maximumof operator generates the maximum value
 * of a signed or unsigned integer type.
 */

#include "com/diag/diminuto/diminuto_widthof.h"

#define diminuto_issigned(_TYPE_) (((_TYPE_)(~(_TYPE_)0)) < 0)

#ifndef issigned
#   define issigned(_TYPE_) diminuto_issigned(_TYPE_)
#endif

#define diminuto_minimumof(_TYPE_) (diminuto_issigned(_TYPE_) ? ((_TYPE_)(((_TYPE_)1)<<(diminuto_widthof(_TYPE_)-1))) : ((_TYPE_)0))

#ifndef minimumof
#   define minimumof(_TYPE_) diminuto_minimumof(_TYPE_)
#endif

#define diminuto_maximumof(_TYPE_) (diminuto_issigned(_TYPE_) ? ((_TYPE_)(~(((_TYPE_)1)<<(diminuto_widthof(_TYPE_)-1)))) : ((_TYPE_)(~((_TYPE_)0))))

#ifndef maximumof
#   define maximumof(_TYPE_) diminuto_maximumof(_TYPE_)
#endif

#endif
