/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_OFFSETOF_
#define _H_COM_DIAG_DIMINUTO_OFFSETOF_

/**
 * @file
 * @copyright Copyright 2014 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief Implements a compile-time offsetof operator.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 *
 * The Offset Of feature computes the offset of a field from the
 * beginning of its structure without actually having an instance
 * of that structure. See: this is where the Member Of feature is
 * useful.
 */

#include "com/diag/diminuto/diminuto_types.h"
#include "com/diag/diminuto/diminuto_memberof.h"

/**
 * @def diminuto_offsetof
 * Compute the offset in bytes of the member @a _MEMBER_ in the structure
 * @a _STRUCT_.
 */
#define diminuto_offsetof(_STRUCT_, _MEMBER_) ((size_t)(((char *)&diminuto_memberof(_STRUCT_, _MEMBER_)) - ((char *)((_STRUCT_ *)0))))

#ifndef offsetof
#   define offsetof(_STRUCT_, _MEMBER_) diminuto_offsetof(_STRUCT_, _MEMBER_)
#endif

#endif
