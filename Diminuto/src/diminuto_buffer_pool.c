/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2015 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "diminuto_buffer.h"
#include "com/diag/diminuto/diminuto_countof.h"

/******************************************************************************/

/*
 * These are the sizes I chose for each quanta in the buffer pool, but you
 * should be able to put any monotonically increasing sizes here and it should
 * work. You can also increase the number of quanta in the pool just by adding
 * more entries. (The unit test will have to change, since it uses knowledge
 * about the default pool implementation to test it.)
 */
static const size_t POOL[] = {
    1 << 3,  /* [0]: 2^3  =    8 + 8 =   16 */
    1 << 4,  /* [1]: 2^4  =   16 + 8 =   24 */
    1 << 5,  /* [2]: 2^5  =   32 + 8 =   40 */
    1 << 6,  /* [3]: 2^6  =   64 + 8 =   72 */
    1 << 7,  /* [4]: 2^7  =  128 + 8 =  136 */
    1 << 8,  /* [5]: 2^8  =  256 + 8 =  264 */
    1 << 9,  /* [6]: 2^9  =  512 + 8 =  520 */
    1 << 10, /* [7]: 2^10 = 1024 + 8 = 1032 */
    1 << 11, /* [8]: 2^11 = 2048 + 8 = 2056 */
    1 << 12, /* [9]: 2^12 = 4096 + 8 = 4104 */
};

static diminuto_buffer_t * pool[countof(POOL)] = { (diminuto_buffer_t *)0 };

/******************************************************************************/

diminuto_buffer_meta_t diminuto_buffer_pool = { countof(POOL), POOL, pool };

/******************************************************************************/

int diminuto_buffer_set(diminuto_buffer_pool_t * poolp)
{
    int rc;

    rc = (poolp != (diminuto_buffer_pool_t *)0);
    if (rc) {
        diminuto_buffer_pool.count = poolp->count;
        diminuto_buffer_pool.sizes = poolp->sizes;
        diminuto_buffer_pool.pool = (diminuto_buffer_t **)(poolp->pool);
    } else {
        diminuto_buffer_pool.count = countof(POOL);
        diminuto_buffer_pool.sizes = POOL;
        diminuto_buffer_pool.pool = pool;
    }

    return rc;
}
