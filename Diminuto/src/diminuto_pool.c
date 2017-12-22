/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2010-2015 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 */

#include "com/diag/diminuto/diminuto_pool.h"
#include "com/diag/diminuto/diminuto_heap.h"
#include "com/diag/diminuto/diminuto_containerof.h"
#include "com/diag/diminuto/diminuto_log.h"

diminuto_pool_t * diminuto_pool_init(diminuto_pool_t * poolp, size_t size)
{
    uintptr_t allocation;

    allocation = (sizeof(diminuto_pool_object_t) + size + sizeof(((diminuto_pool_object_t *)0)->payload[0]) - 1) & ~(sizeof(((diminuto_pool_object_t *)0)->payload[0]) - 1);

    return diminuto_list_datainit(poolp, (void *)allocation);
}

diminuto_pool_t * diminuto_pool_fini(diminuto_pool_t * poolp)
{
    poolp = diminuto_list_root(poolp);
    while (!diminuto_list_isempty(poolp)) {
        diminuto_heap_free(diminuto_list_remove(diminuto_list_next(poolp)));
    }

    return poolp;
}

void * diminuto_pool_alloc(diminuto_pool_t * poolp)
{
    uintptr_t allocation;
    diminuto_pool_object_t * nodep;

    poolp = diminuto_list_root(poolp);
    if (diminuto_list_isempty(poolp)) {
        allocation = (uintptr_t)diminuto_list_data(poolp);
        nodep = (diminuto_pool_object_t *)diminuto_heap_malloc(allocation);
        if (nodep == (void *)0) {
            diminuto_perror("diminuto_heap_malloc");
            return nodep;
        }
        (void)diminuto_list_datainit(&nodep->link, poolp);
    } else {
        nodep = containerof(diminuto_pool_object_t, link, diminuto_list_remove(diminuto_list_next(poolp)));
    }

    return &nodep->payload;
}

void diminuto_pool_free(void * pointer)
{
    diminuto_pool_object_t * nodep;
    diminuto_pool_t * poolp;

    nodep = containerof(diminuto_pool_object_t, payload, pointer);
    poolp = diminuto_list_root((diminuto_list_t *)diminuto_list_data(&nodep->link));
    diminuto_list_insert(diminuto_list_prev(poolp), &nodep->link);
}
