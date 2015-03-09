/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_CONTAINEROF_
#define _H_COM_DIAG_DIMINUTO_CONTAINEROF_

/**
 * @file
 *
 * Copyright 2014 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "com/diag/diminuto/diminuto_offsetof.h"

/**
 * @def diminuto_containerof
 * Given a pointer @a _POINTER_ to a member named @a _MEMBER_ of an object of
 * type @a _STRUCT_, compute the address of the start of the object. This can be
 * used to recover the address of an object that is in a container that uses
 * a field within the object to manage the object in the container. Wackiness
 * ensues of the _POINTER_ is null!
 */
#define diminuto_containerof(_STRUCT_, _MEMBER_, _POINTER_)  ((_STRUCT_ *)(((char *)(_POINTER_)) - diminuto_offsetof(_STRUCT_, _MEMBER_)))

#ifndef containerof
#   define containerof(_STRUCT_, _MEMBER_, _POINTER_) diminuto_containerof(_STRUCT_, _MEMBER_, _POINTER_)
#endif

#endif
