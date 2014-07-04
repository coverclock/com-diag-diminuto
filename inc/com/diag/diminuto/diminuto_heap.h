/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_HEAP_
#define _H_COM_DIAG_DIMINUTO_HEAP_

/**
 * @file
 *
 * Copyright 2010-2013 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 *
 * This simple little dependency injection mechanism allows you to replace
 * malloc() and free() from <stdlib.h> with alternatives. I use this for
 * unit testing, but you could just as easily install versions of those
 * functions that track memory usage. (But go get the valgrind tool off
 * the web if you don't already have it. It's the best thing since sliced
 * bread.)
 */

#include "com/diag/diminuto/diminuto_proxy.h"
#include <stdlib.h>

COM_DIAG_DIMINUTO_PROXY_POINTER_H(heap, malloc, void *, (size_t size), return, (size))
COM_DIAG_DIMINUTO_PROXY_POINTER_H(heap, free, void, (void * ptr), (void), (ptr))
COM_DIAG_DIMINUTO_PROXY_POINTER_H(heap, calloc, void *, (size_t nmemb, size_t size), return, (nmemb, size))
COM_DIAG_DIMINUTO_PROXY_POINTER_H(heap, realloc, void *, (void * ptr, size_t size), return, (ptr, size))

COM_DIAG_DIMINUTO_PROXY_SETTOR_H(heap, malloc, void *, (size_t size), return, (size))
COM_DIAG_DIMINUTO_PROXY_SETTOR_H(heap, free, void, (void * ptr), (void), (ptr))
COM_DIAG_DIMINUTO_PROXY_SETTOR_H(heap, calloc, void *, (size_t nmemb, size_t size), return, (nmemb, size))
COM_DIAG_DIMINUTO_PROXY_SETTOR_H(heap, realloc, void *, (void * ptr, size_t size), return, (ptr, size))

COM_DIAG_DIMINUTO_PROXY_FUNCTION_H(heap, malloc, void *, (size_t size), return, (size))
COM_DIAG_DIMINUTO_PROXY_FUNCTION_H(heap, free, void, (void * ptr), (void), (ptr))
COM_DIAG_DIMINUTO_PROXY_FUNCTION_H(heap, calloc, void *, (size_t nmemb, size_t size), return, (nmemb, size))
COM_DIAG_DIMINUTO_PROXY_FUNCTION_H(heap, realloc, void *, (void * ptr, size_t size), return, (ptr, size))

#endif
