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
 *
 * This simple little dependency injection mechanism allows you to replace
 * malloc() and free() from <stdlib.h> with alternatives. I use this for
 * unit testing, but you could just as easily install versions of those
 * functions that track memory usage. (But go get the valgrind tool off
 * the web if you don't already have it. It's the best thing since sliced
 * bread.)
 */

#include "diminuto_types.h"
#include <stdlib.h>

typedef void * (diminuto_heap_alloc_func)(size_t size);

typedef void (diminuto_heap_free_func)(void * pointer);

extern diminuto_heap_alloc_func * diminuto_heap_alloc_funcp;
extern diminuto_heap_free_func * diminuto_heap_free_funcp;

/**
 * diminuto_heap_alloc_set(_ALLOC_)
 * Install an alternative heap memory allocation function pointed to
 * by @a _ALLOC_.
 */
#define diminuto_heap_alloc_set(_ALLOC_) \
    (diminuto_heap_alloc_funcp = (_ALLOC_))

/**
 * diminuto_heap_free_set(_ALLOC_)
 * Install an alternative heap memory free function pointed to
 * by @a _ALLOC_.
 */
#define diminuto_heap_free_set(_FREE_) \
    (diminuto_heap_free_funcp = (_FREE_))

/**
 * diminuto_heap_alloc(_SIZE_)
 * Allocate a block of memory of size @a _SIZE_ bytes from the heap and
 * return its address or NULL if the allocation failed.
 */
#define diminuto_heap_alloc(_SIZE_) \
    ((*(diminuto_heap_alloc_funcp ? diminuto_heap_alloc_funcp : &malloc))(_SIZE_))

/**
 * diminuto_heap_free(_POINTER_)
 * Free a block of previously allocated memory pointed to by @a _POINTER_
 * back to the heap.
 */
#define diminuto_heap_free(_POINTER_) \
    ((*(diminuto_heap_free_funcp ? diminuto_heap_free_funcp : &free))(_POINTER_))

#endif
