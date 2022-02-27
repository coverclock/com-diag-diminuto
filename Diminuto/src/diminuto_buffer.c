/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2015-2020 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is the implementation of the Buffer feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is the implementation of the Buffer feature.
 */

#include "com/diag/diminuto/diminuto_countof.h"
#include "com/diag/diminuto/diminuto_containerof.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_criticalsection.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include "../src/diminuto_buffer_pool.h"

/*******************************************************************************
 * OPTIONS
 ******************************************************************************/

static int debug = 0;

static int fail = 0;

/*******************************************************************************
 * POOL
 ******************************************************************************/

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

/*
 * These are the sizes I chose for each quanta in the buffer pool, but you
 * should be able to put any monotonically increasing sizes here and it should
 * work. You can also increase the number of quanta in the pool just by adding
 * more entries. (The unit test will have to change, since it uses knowledge
 * about the default pool implementation to test it.)
 */
static const size_t POOL[] = {
    1 << 3,  /* [0]: (2^3  =    8) + sizeof(diminuto_buffer_t) =   16 */
    1 << 4,  /* [1]: (2^4  =   16) + sizeof(diminuto_buffer_t) =   24 */
    1 << 5,  /* [2]: (2^5  =   32) + sizeof(diminuto_buffer_t) =   40 */
    1 << 6,  /* [3]: (2^6  =   64) + sizeof(diminuto_buffer_t) =   72 */
    1 << 7,  /* [4]: (2^7  =  128) + sizeof(diminuto_buffer_t) =  136 */
    1 << 8,  /* [5]: (2^8  =  256) + sizeof(diminuto_buffer_t) =  264 */
    1 << 9,  /* [6]: (2^9  =  512) + sizeof(diminuto_buffer_t) =  520 */
    1 << 10, /* [7]: (2^10 = 1024) + sizeof(diminuto_buffer_t) = 1032 */
    1 << 11, /* [8]: (2^11 = 2048) + sizeof(diminuto_buffer_t) = 2056 */
    1 << 12, /* [9]: (2^12 = 4096) + sizeof(diminuto_buffer_t) = 4104 */
};

static diminuto_buffer_t * pool[countof(POOL)] = { (diminuto_buffer_t *)0 };

static diminuto_buffer_meta_t meta = { countof(POOL), POOL, pool };

/*******************************************************************************
 * <stdlib.h>-LIKE FUNCTIONS
 ******************************************************************************/

void * diminuto_buffer_malloc(size_t size)
{
    void * ptr = (void *)0;

    if (size > 0) {

        DIMINUTO_CRITICAL_SECTION_BEGIN(&mutex);

            ptr = diminuto_buffer_pool_get((diminuto_buffer_pool_t *)&meta, size, fail);

        DIMINUTO_CRITICAL_SECTION_END;

    } else {

        errno = 0;

    }

    if (debug) {
        diminuto_log_emit("diminuto_buffer_malloc: size=%zu ptr=%p\n", size, ptr);
    }

    return ptr;
}

void diminuto_buffer_free(void * ptr)
{
    if (debug) {
        diminuto_log_emit("diminuto_buffer_free: ptr=%p\n", ptr);
    }

    if (ptr != (void *)0) {

        DIMINUTO_CRITICAL_SECTION_BEGIN(&mutex);

            diminuto_buffer_pool_put((diminuto_buffer_pool_t *)&meta, ptr);

        DIMINUTO_CRITICAL_SECTION_END;

    }
}

void * diminuto_buffer_realloc(void * ptr, size_t size)
{
    void * ptrprime = (void *)0;

    if (ptr == (void *)0) {
        ptrprime = diminuto_buffer_malloc(size);
    } else if (size == 0) {
        diminuto_buffer_free(ptr);
        errno = 0;
        ptrprime = (void *)0;
    } else {
        diminuto_buffer_t * buffer = (diminuto_buffer_t *)0;
        size_t actual = 0;
        size_t requested = 0;

        buffer = containerof(diminuto_buffer_t, payload, ptr);

        DIMINUTO_CRITICAL_SECTION_BEGIN(&mutex);

            actual = buffer_pool_effective(&meta, buffer->header.item);
            (void)buffer_pool_hash(&meta, size, &requested);

        DIMINUTO_CRITICAL_SECTION_END;

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
        diminuto_log_emit("diminuto_buffer_realloc: ptr=%p size=%zu pointer=%p\n", ptr, size, ptrprime);
    }

    return ptrprime;
}

void * diminuto_buffer_calloc(size_t nmemb, size_t size)
{
    void * ptr = (void *)0;
    size_t length = 0;

    length = nmemb * size;
    ptr = diminuto_buffer_malloc(length);
    if (ptr != (void *)0) {
        memset(ptr, 0, length);
    }

    if (debug) {
        diminuto_log_emit("diminuto_buffer_calloc: nmemb=%zu size=%zu ptr=%p\n", nmemb, size, ptr);
    }

    return ptr;
}

/*******************************************************************************
 * <string.h>-LIKE FUNCTIONS
 ******************************************************************************/

char * diminuto_buffer_strdup(const char * s)
{
    char * ptr = (char *)0;
    size_t length = 0;

    length = strlen(s);
    ptr = (char *)diminuto_buffer_malloc(length + 1);
    if (ptr != (char *)0) {
        strcpy(ptr, s);
    }

    return ptr;
}

char * diminuto_buffer_strndup(const char * s, size_t n)
{
    char * ptr = (char *)0;
    size_t length = 0;

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
    DIMINUTO_CRITICAL_SECTION_BEGIN(&mutex);

        diminuto_buffer_pool_free((diminuto_buffer_pool_t *)&meta);

    DIMINUTO_CRITICAL_SECTION_END;
}

size_t diminuto_buffer_prealloc(size_t nmemb, size_t size)
{
    size_t result = 0;

    DIMINUTO_CRITICAL_SECTION_BEGIN(&mutex);

        result = diminuto_buffer_pool_prealloc((diminuto_buffer_pool_t *)&meta, nmemb, size);

    DIMINUTO_CRITICAL_SECTION_END;

    return result;
}

size_t diminuto_buffer_log(void)
{
    size_t result = 0;

    DIMINUTO_CRITICAL_SECTION_BEGIN(&mutex);

        result = diminuto_buffer_pool_log((diminuto_buffer_pool_t *)&meta);

    DIMINUTO_CRITICAL_SECTION_END;

    return result;
}

int diminuto_buffer_set(diminuto_buffer_pool_t * poolp)
{
    int result = 0;

    DIMINUTO_CRITICAL_SECTION_BEGIN(&mutex);

        result = (poolp != (diminuto_buffer_pool_t *)0);
        if (result) {
            meta.count = poolp->count;
            meta.sizes = poolp->sizes;
            meta.pool = (diminuto_buffer_t **)(poolp->pool);
        } else {
            meta.count = countof(POOL);
            meta.sizes = POOL;
            meta.pool = pool;
        }

    DIMINUTO_CRITICAL_SECTION_END;

    return result;
}

int diminuto_buffer_debug(int after)
{
    int before = 0;

    before = debug;
    debug = after;

    return before;
}

int diminuto_buffer_nomalloc(int after)
{
    int before = 0;

    before = fail;
    fail = after;

    return before;
}

/*******************************************************************************
 * EXPOSED FOR UNIT TESTING - NOT THREAD SAFE
 ******************************************************************************/

size_t diminuto_buffer_hash(size_t requested, size_t * actualp)
{
    return buffer_pool_hash(&meta, requested, actualp);
}

size_t diminuto_buffer_effective(size_t item)
{
    return buffer_pool_effective(&meta, item);
}

size_t diminuto_buffer_count(void)
{
    return buffer_pool_count(&meta);
}

size_t diminuto_buffer_size(size_t item)
{
    return buffer_pool_size(&meta, item);
}

size_t diminuto_buffer_isempty(size_t item)
{
    return (buffer_pool_isexternal(&meta, item) || (buffer_pool_first(&meta, item) == (diminuto_buffer_t *)0));
}
