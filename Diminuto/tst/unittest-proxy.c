/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2015-2023 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is a unit test of the Proxy feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is a unit test of the Proxy feature.
 *
 * The remainder of the proxy macros are tested in the heap unit test.
 */

#include "com/diag/diminuto/diminuto_types.h"
#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_proxy.h"
#include <stdlib.h>

DIMINUTO_PROXY_POINTER_H(heap, malloc, void *, (size_t size), return, (size))
DIMINUTO_PROXY_POINTER_H(heap, free, void, (void * ptr), (void), (ptr))

DIMINUTO_PROXY_SETTOR_H(heap, malloc, void *, (size_t size), return, (size))
DIMINUTO_PROXY_SETTOR_H(heap, free, void, (void * ptr), (void), (ptr))

DIMINUTO_PROXY_FUNCTION_S(heap, malloc, void *, (size_t size), return, (size))
DIMINUTO_PROXY_FUNCTION_S(heap, free, void, (void * ptr), (void), (ptr))

static uint64_t data = 0;

static void * myalloc(size_t size /* UNUSED */)
{
    return &data;
}

static void myfree(void * pointer)
{
    ASSERT(pointer == (void *)&data);
    *(uint64_t *)pointer = 1;
}

int main(void)
{
    void * p1;
    void * p2;
    void * p3;

    {
        TEST();

        p1 = diminuto_heap_malloc(sizeof(int));
        ASSERT(p1 != (void *)0);
        ASSERT(p1 != (void *)&data);

        p2 = diminuto_heap_malloc(sizeof(int));
        ASSERT(p2 != (void *)0);
        ASSERT(p2 != (void *)&data);
        ASSERT(p2 != p1);

        diminuto_heap_free(p1);
        diminuto_heap_free(p2);

        STATUS();
    }

    {
        TEST();

        diminuto_heap_malloc_set(myalloc);
        p3 = diminuto_heap_malloc(sizeof(int));
        ASSERT(p3 == (void *)&data);

        data = 0;
        diminuto_heap_free_set(myfree);
        ASSERT(data == 0);
        diminuto_heap_free(p3);
        ASSERT(data == 1);

        STATUS();
    }

    {
        TEST();

        diminuto_heap_malloc_set((diminuto_heap_malloc_func_t *)0);
        diminuto_heap_free_set((diminuto_heap_free_func_t *)0);

        p1 = diminuto_heap_malloc(sizeof(int));
        ASSERT(p1 != (void *)0);
        ASSERT(p1 != (void *)&data);
        data = 0;
        diminuto_heap_free(p1);
        ASSERT(data == 0);

        STATUS();
    }

    EXIT();
}
