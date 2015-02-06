/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2010 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "com/diag/diminuto/diminuto_heap.h"

DIMINUTO_PROXY_POINTER_C(heap, malloc, void *, (size_t size), return, (size))
DIMINUTO_PROXY_POINTER_C(heap, free, void, (void * ptr), (void), (ptr))
DIMINUTO_PROXY_POINTER_C(heap, calloc, void *, (size_t nmemb, size_t size), return, (nmemb, size))
DIMINUTO_PROXY_POINTER_C(heap, realloc, void *, (void * ptr, size_t size), return, (ptr, size))

DIMINUTO_PROXY_SETTOR_C(heap, malloc, void *, (size_t size), return, (size))
DIMINUTO_PROXY_SETTOR_C(heap, free, void, (void * ptr), (void), (ptr))
DIMINUTO_PROXY_SETTOR_C(heap, calloc, void *, (size_t nmemb, size_t size), return, (nmemb, size))
DIMINUTO_PROXY_SETTOR_C(heap, realloc, void *, (void * ptr, size_t size), return, (ptr, size))

DIMINUTO_PROXY_FUNCTION_C(heap, malloc, void *, (size_t size), return, (size))
DIMINUTO_PROXY_FUNCTION_C(heap, free, void, (void * ptr), (void), (ptr))
DIMINUTO_PROXY_FUNCTION_C(heap, calloc, void *, (size_t nmemb, size_t size), return, (nmemb, size))
DIMINUTO_PROXY_FUNCTION_C(heap, realloc, void *, (void * ptr, size_t size), return, (ptr, size))
