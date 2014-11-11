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

#define diminuto_containerof(_STRUCT_, _MEMBER_, _POINTER_)  ((_STRUCT_ *)(((char *)(_POINTER_)) - desperado_offsetof(_STRUCT_, _MEMBER_)))

#ifndef containerof
#   define containerof(_STRUCT_, _MEMBER_, _POINTER_) diminuto_containerof(_STRUCT_, _MEMBER_, _POINTER_)
#endif

#endif
