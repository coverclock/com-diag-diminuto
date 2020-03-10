/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_COUNTOF_
#define _H_COM_DIAG_DIMINUTO_COUNTOF_

/**
 * @file
 *
 * Copyright 2010, 2014 Digital Aggregates Corporation, Colorado, USA.
 * Licensed under the terms in LICENSE.txt.
 *
 * The Count Of feature returns the number of positions in an
 * array, providing the sizeof the total array and the sizeof the
 * array type are both known. This is based on a macro the author
 * encountered in VxWorks decades ago.
 */

/**
 * @def diminuto_countof
 * Given an array named @a _ARRAY_ whose dimension is known at compile time,
 * compute the number of elements in the array.
 */
#define diminuto_countof(_ARRAY_) (sizeof(_ARRAY_) / sizeof(_ARRAY_[0]))

#ifndef countof
#   define countof(_ARRAY_) diminuto_countof(_ARRAY_)
#endif

#endif
