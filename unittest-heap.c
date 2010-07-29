/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2010 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "diminuto_heap.h"
#include "diminuto_unittest.h"

static uint64_t data = 0;

static void * myalloc(size_t size)
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
    void * p4;

    p1 = diminuto_heap_alloc(sizeof(int));
    ASSERT(p1 != (void *)0);
    ASSERT(p1 != (void *)&data);

    p2 = diminuto_heap_alloc(sizeof(int));
    ASSERT(p2 != (void *)0);
    ASSERT(p2 != (void *)&data);
    ASSERT(p2 != p1);

    diminuto_heap_free(p1);
    diminuto_heap_free(p2);

    diminuto_heap_alloc_set(myalloc);
    p3 = diminuto_heap_alloc(sizeof(int));
    ASSERT(p3 = (void *)&data);

    data = 0;
    diminuto_heap_free_set(myfree);
    ASSERT(data == 0);
    diminuto_heap_free(p3);
    ASSERT(data == 1);

    diminuto_heap_alloc_set((diminuto_heap_alloc_func *)0);
    diminuto_heap_free_set((diminuto_heap_free_func *)0);

    p1 = diminuto_heap_alloc(sizeof(int));
    ASSERT(p1 != (void *)0);
    ASSERT(p1 != (void *)&data);
    data = 0;
    diminuto_heap_free(p1);
    ASSERT(data == 0);
}
