/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2010-2015 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is a unit test of the Pool feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is a unit test of the Pool feature.
 *
 * It is really helpful to run this with the valgrind(1) tool.
 */

#include "com/diag/diminuto/diminuto_pool.h"
#include "com/diag/diminuto/diminuto_heap.h"
#include "com/diag/diminuto/diminuto_countof.h"
#include "com/diag/diminuto/diminuto_unittest.h"
#include <stdlib.h>

static int count = 0;
static size_t total = 0;

static void * myalloc(size_t size)
{
    ++count;
    ASSERT(size == (sizeof(diminuto_list_t) + 8));
    total += size;
    ASSERT(total == ((sizeof(diminuto_list_t) + 8)) * count);
    return malloc(size);
}

static void myfree(void * pointer)
{
    free(pointer);
    --count;
    ASSERT(count >= 0);
}

int main(void)
{
    diminuto_pool_t pool;
    void *pa[5];
    void *pb[countof(pa)];
    int ii;
    int jj;

    diminuto_heap_malloc_set(&myalloc);
    diminuto_heap_free_set(&myfree);

    ASSERT(diminuto_pool_init(&pool, 1) == &pool);

    ASSERT(count == 0);
    ASSERT(total == 0);

    for (ii = 0; ii < countof(pa); ++ii) {
        pa[ii] = diminuto_pool_alloc(&pool);
        ASSERT(pa[ii] != (void *)0);
        ASSERT((((uintptr_t)pa[ii]) & (sizeof(uint64_t) - 1)) == 0);
    }

    ASSERT(count == countof(pa));
    ASSERT(total == ((sizeof(diminuto_list_t) + 8) * countof(pa)));

    for (ii = 0; ii < countof(pa); ++ii) {
        for (jj = 0; jj < countof(pa); ++jj) {
            if (jj != ii) {
                ASSERT(pa[ii] != pa[jj]);
            }
        }
    }

    for (ii = 0; ii < countof(pa); ++ii) {
        diminuto_pool_free(pa[ii]);
    }

    ASSERT(count == countof(pa));

    for (ii = 0; ii < countof(pb); ++ii) {
        pb[ii] = diminuto_pool_alloc(&pool);
        ASSERT(pb[ii] != (void *)0);
        ASSERT(pb[ii] == pa[ii]);
    }

    ASSERT(count == countof(pa));

    for (ii = 0; ii < countof(pb); ++ii) {
        diminuto_pool_free(pb[ii]);
    }

    ASSERT(count == countof(pa));

    diminuto_pool_fini(&pool);

    ASSERT(count == 0);

    EXIT();
}
