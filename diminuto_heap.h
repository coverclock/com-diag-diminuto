/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_HEAP_
#define _H_COM_DIAG_DIMINUTO_HEAP_

/**
 * @file
 *
 * Copyright 2010 Digital Aggregates Corporation, Arvada CO 80001-0587 USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "diminuto_types.h"
#include <stdlib.h>

typedef void * (diminuto_heap_alloc_func)(size_t size);

typedef void (diminuto_heap_free_func)(void * pointer);

extern diminuto_heap_alloc_func * diminuto_heap_alloc_funcp;
extern diminuto_heap_free_func * diminuto_heap_free_funcp;

#define diminuto_heap_alloc(_SIZE_) \
    (diminuto_heap_alloc_funcp != (diminuto_heap_alloc_func *)0) \
        ? (*diminuto_heap_alloc_funcp)(_SIZE_) \
        : malloc(_SIZE_)

#define diminuto_heap_free(_POINTER_) \
    (diminuto_heap_free_funcp != (diminuto_heap_free_func *)0) \
        ? (*diminuto_heap_free_funcp)(_POINTER_) \
        : free(_POINTER_)

#endif
