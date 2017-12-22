/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_OFFSETOF_
#define _H_COM_DIAG_DIMINUTO_OFFSETOF_

/**
 * @file
 *
 * Copyright 2014 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
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
