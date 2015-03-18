/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2015 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "diminuto_buffer_pool.h"
#include "com/diag/diminuto/diminuto_countof.h"
#include "com/diag/diminuto/diminuto_containerof.h"
#include "com/diag/diminuto/diminuto_log.h"

/*******************************************************************************
 * OPTIONS
 ******************************************************************************/

static int debug = 0;

static int fail = 0;

/*******************************************************************************
 * POOL
 ******************************************************************************/

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

diminuto_buffer_meta_t diminuto_buffer_pool = { countof(POOL), POOL, pool };

/*******************************************************************************
 * <stdlib.h>-LIKE FUNCTIONS
 ******************************************************************************/

void * diminuto_buffer_malloc(size_t size)
{
    void * ptr;

    ptr = diminuto_buffer_pool_get((diminuto_buffer_pool_t *)&diminuto_buffer_pool, size, fail);

    if (debug) {
        DIMINUTO_LOG_DEBUG("diminuto_buffer_malloc: size=%zu ptr=%p\n", size, ptr);
    }

    return ptr;
}

void diminuto_buffer_free(void * ptr)
{
    if (debug) {
        DIMINUTO_LOG_DEBUG("diminuto_buffer_free: ptr=%p\n", ptr);
    }

    diminuto_buffer_pool_put((diminuto_buffer_pool_t *)&diminuto_buffer_pool, ptr);
}

void * diminuto_buffer_realloc(void * ptr, size_t size)
{
    void * ptrprime;

    if (ptr == (void *)0) {
        ptrprime = diminuto_buffer_malloc(size);
    } else if (size == 0) {
        diminuto_buffer_free(ptr);
        errno = 0;
        ptrprime = (void *)0;
    } else {
        diminuto_buffer_t * buffer;
        size_t actual;
        size_t requested;

        buffer = containerof(diminuto_buffer_t, payload, ptr);
        actual = buffer_pool_effective((diminuto_buffer_pool_t *)&diminuto_buffer_pool, buffer->header.item);
        (void)buffer_pool_hash((diminuto_buffer_pool_t *)&diminuto_buffer_pool, size, &requested);
        if (actual == requested) {
            ptrprime = ptr;
        } else {
            ptrprime = diminuto_buffer_malloc(size);
            if (ptrprime != (void *)0) {
                size_t length;

                length = actual - sizeof(diminuto_buffer_t);
                memcpy(ptrprime, ptr, size < length ? size : length);
            }
            diminuto_buffer_free(ptr);
        }
    }

    if (debug) {
        DIMINUTO_LOG_DEBUG("diminuto_buffer_realloc: ptr=%p size=%zu pointer=%p\n", ptr, size, ptrprime);
    }

    return ptrprime;
}

void * diminuto_buffer_calloc(size_t nmemb, size_t size)
{
    void * ptr;
    size_t length;

    length = nmemb * size;
    ptr = diminuto_buffer_malloc(length);
    if (ptr != (void *)0) {
        memset(ptr, 0, length);
    }

    if (debug) {
        DIMINUTO_LOG_DEBUG("diminuto_buffer_calloc: nmemb=%zu size=%zu ptr=%p\n", nmemb, size, ptr);
    }

    return ptr;
}

/*******************************************************************************
 * <string.h>-LIKE FUNCTIONS
 ******************************************************************************/

char * diminuto_buffer_strdup(const char * s)
{
    char * ptr;
    size_t length;

    length = strlen(s);
    ptr = (char *)diminuto_buffer_malloc(length + 1);
    if (ptr != (char *)0) {
        strcpy(ptr, s);
    }

    return ptr;
}

char * diminuto_buffer_strndup(const char * s, size_t n)
{
    char * ptr;
    size_t length;

    length = strnlen(s, n);
    ptr = (char *)diminuto_buffer_malloc(length + 1);
    if (ptr != (char *)0) {
        strncpy(ptr, s, length);
        ptr[length] = '\0';
    }

    return ptr;
}

/*******************************************************************************
 * ANCILLARY FUNCTIONS
 ******************************************************************************/

void diminuto_buffer_fini(void)
{
    diminuto_buffer_pool_fini((diminuto_buffer_pool_t *)&diminuto_buffer_pool);
}

size_t diminuto_buffer_prealloc(size_t nmemb, size_t size)
{
    return diminuto_buffer_pool_prealloc((diminuto_buffer_pool_t *)&diminuto_buffer_pool, nmemb, size);
}

size_t diminuto_buffer_log(void)
{
    return diminuto_buffer_pool_log((diminuto_buffer_pool_t *)&diminuto_buffer_pool);
}

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

int diminuto_buffer_debug(int after)
{
    int before;

    before = debug;
    debug = after;

    return before;
}

int diminuto_buffer_nomalloc(int after)
{
    int before;

    before = fail;
    fail = after;

    return before;
}

/*******************************************************************************
 * PRIVATE FUNCTIONS
 ******************************************************************************/

size_t diminuto_buffer_hash(size_t requested, size_t * actualp)
{
    return buffer_pool_hash((diminuto_buffer_pool_t *)&diminuto_buffer_pool, requested, actualp);
}

size_t diminuto_buffer_effective(unsigned int item)
{
    return buffer_pool_effective((diminuto_buffer_pool_t *)&diminuto_buffer_pool, item);
}
