/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2010 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is the implementation of the Heap feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is the implementation of the Heap feature.
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
