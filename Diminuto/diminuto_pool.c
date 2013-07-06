/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2010 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "com/diag/diminuto/diminuto_pool.h"
#include "com/diag/diminuto/diminuto_heap.h"
#include "com/diag/diminuto/diminuto_log.h"

#define diminuto_pool_root(_POINTER_)

diminuto_pool_t * diminuto_pool_init(diminuto_pool_t * poolp, size_t size)
{
    size = (sizeof(diminuto_list_t) + size + 7) & ~(size_t)7;
    return diminuto_list_datainit(poolp, size);
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
    void * voidp;
    diminuto_list_t * nodep;

    poolp = diminuto_list_root(poolp);
    if (!diminuto_list_isempty(poolp)) {
        nodep = diminuto_list_remove(diminuto_list_next(poolp));
    } else {
        voidp = diminuto_heap_alloc((size_t)diminuto_list_data(poolp));
        if (voidp == (void *)0) {
            diminuto_perror("malloc");
            return voidp;
        }
        nodep = diminuto_list_datainit((diminuto_list_t *)voidp, poolp);
    }

    return (void *)(((char *)nodep) + sizeof(diminuto_list_t));
}

void diminuto_pool_free(void * pointer)
{
    diminuto_list_t * nodep;
    diminuto_list_t * poolp;

    nodep = (diminuto_list_t *)(((char *)pointer) - sizeof(diminuto_list_t));
    poolp = diminuto_list_root((diminuto_list_t *)diminuto_list_data(nodep));
    diminuto_list_insert(diminuto_list_prev(poolp), nodep);
}
