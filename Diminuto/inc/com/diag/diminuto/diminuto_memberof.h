/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_MEMBEROF_
#define _H_COM_DIAG_DIMINUTO_MEMBEROF_

/**
 * @file
 *
 * Copyright 2015 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in LICENSE.txt<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 */

/**
 * @def diminuto_memberof
 * Create a reference to a member named @a _MEMBER_ of an structure of type @a
 * _STRUCT_. This can be used to take the sizeof() a member of a structure
 * without having a pointer to such a structure.
 */
#define diminuto_memberof(_STRUCT_, _MEMBER_)   (((_STRUCT_ *)0)->_MEMBER_)

#ifndef memberof
#   define memberof(_STRUCT_, _MEMBER_) diminuto_memberof(_STRUCT_, _MEMBER_)
#endif

#endif
