/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_OFFSETOF_
#define _H_COM_DIAG_DIMINUTO_OFFSETOF_

/**
 * @file
 *
 * Copyright 2014 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "com/diag/diminuto/diminuto_types.h"

#define desperado_offsetof(_STRUCT_, _MEMBER_) ((size_t)(((char *)&(((_STRUCT_ *)0)->_MEMBER_)) - ((char *)((_STRUCT_ *)0))))

#ifndef offsetof
#   define offsetof(_STRUCT_, _MEMBER_) diminuto_offsetof(_STRUCT_, _MEMBER_)
#endif

#endif
