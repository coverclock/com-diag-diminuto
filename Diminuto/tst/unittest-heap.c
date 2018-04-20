/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2010 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in LICENSE.txt<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 */

#include "com/diag/diminuto/diminuto_heap.h"
#include "com/diag/diminuto/diminuto_types.h"
#include "com/diag/diminuto/diminuto_unittest.h"

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

    p1 = diminuto_heap_malloc(sizeof(int));
    ASSERT(p1 != (void *)0);
    ASSERT(p1 != (void *)&data);

    p2 = diminuto_heap_malloc(sizeof(int));
    ASSERT(p2 != (void *)0);
    ASSERT(p2 != (void *)&data);
    ASSERT(p2 != p1);

    diminuto_heap_free(p1);
    diminuto_heap_free(p2);

    diminuto_heap_malloc_set(myalloc);
    p3 = diminuto_heap_malloc(sizeof(int));
    ASSERT(p3 = (void *)&data);

    data = 0;
    diminuto_heap_free_set(myfree);
    ASSERT(data == 0);
    diminuto_heap_free(p3);
    ASSERT(data == 1);

    diminuto_heap_malloc_set((diminuto_heap_malloc_func_t *)0);
    diminuto_heap_free_set((diminuto_heap_free_func_t *)0);

    p1 = diminuto_heap_malloc(sizeof(int));
    ASSERT(p1 != (void *)0);
    ASSERT(p1 != (void *)&data);
    data = 0;
    diminuto_heap_free(p1);
    ASSERT(data == 0);

    EXIT();
}
