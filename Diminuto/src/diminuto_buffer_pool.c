/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2015-2020 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is the implementation of the Buffer Pool feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is the implementation of the Buffer Pool feature.
 */

#include "com/diag/diminuto/diminuto_countof.h"
#include "com/diag/diminuto/diminuto_containerof.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "../src/diminuto_buffer_pool.h"

void * diminuto_buffer_pool_get(diminuto_buffer_pool_t * poolp, size_t size, int nomalloc)
{
    void * ptr = (void *)0;

    if (size == 0) {
        errno = 0;
    } else {
        diminuto_buffer_meta_t * that = (diminuto_buffer_meta_t *)0;
        size_t item = 0;
        size_t actual = 0;
        diminuto_buffer_t * buffer = (diminuto_buffer_t *)0;

        that = (diminuto_buffer_meta_t *)poolp;
        item = buffer_pool_hash(that, size, &actual);
        if (buffer_pool_isexternal(that, item)) {
            buffer = buffer_pool_malloc(that, actual, nomalloc);
        } else if (buffer_pool_first(that, item) == (diminuto_buffer_t *)0) {
            buffer = buffer_pool_malloc(that, actual, nomalloc);
        }  else {
            buffer = buffer_pool_get(that, item);
        }
        if (buffer != (diminuto_buffer_t *)0) {
            ptr = buffer_pool_init(that, buffer, item, actual);
        }
    }

    return ptr;
}

void diminuto_buffer_pool_put(diminuto_buffer_pool_t * poolp, void * ptr)
{
    if (ptr != (void *)0) {
        diminuto_buffer_meta_t * that = (diminuto_buffer_meta_t *)0;
        size_t item = 0;
        diminuto_buffer_t * buffer = (diminuto_buffer_t *)0;

        that = (diminuto_buffer_meta_t *)poolp;
        buffer = containerof(diminuto_buffer_t, payload, ptr);
        item = buffer->header.item;
        if (buffer_pool_isexternal(that, item)) {
            free(buffer);
        } else {
            buffer_pool_put(that, buffer, item);
        }
    }
}

void diminuto_buffer_pool_free(diminuto_buffer_pool_t * poolp)
{
    diminuto_buffer_meta_t * that = (diminuto_buffer_meta_t *)0;
    size_t item = 0;
    diminuto_buffer_t * buffer = (diminuto_buffer_t *)0;

    that = (diminuto_buffer_meta_t *)poolp;
    for (item = 0; !buffer_pool_isexternal(that, item); ++item) {
        while ((buffer = buffer_pool_get(that, item)) != (diminuto_buffer_t *)0) {
            free(buffer);
        }
    }
}

size_t diminuto_buffer_pool_prealloc(diminuto_buffer_pool_t * poolp, size_t nmemb, size_t size)
{
    size_t total = 0;
    diminuto_buffer_meta_t * that = (diminuto_buffer_meta_t *)0;
    size_t item = 0;
    size_t actual = 0;
    diminuto_buffer_t * buffer = (diminuto_buffer_t *)0;

    that = (diminuto_buffer_meta_t *)poolp;
    if (size == 0) {
        /* Do nothing. */
    } else if (nmemb == 0) {
        /* Do nothing. */
    } else if (buffer_pool_isexternal(that, item = buffer_pool_hash(that, size, &actual))) {
        /* Do nothing. */
    } else {
        while ((nmemb--) > 0) {
            buffer = (diminuto_buffer_t *)buffer_pool_malloc(that, actual, 0);
            if (buffer == (diminuto_buffer_t *)0) {
                break;
            }
            buffer_pool_put(that, buffer, item);
            total += actual;
        }
    }

    return total;
}

size_t diminuto_buffer_pool_log(diminuto_buffer_pool_t * poolp)
{
    size_t total = 0;
    diminuto_buffer_meta_t * that = (diminuto_buffer_meta_t *)0;
    size_t subtotal = 0;
    size_t item = 0;
    size_t count = 0;
    size_t subcount = 0;
    size_t length = 0;
    size_t size = 0;
    diminuto_buffer_t * buffer = (diminuto_buffer_t *)0;

    that = (diminuto_buffer_meta_t *)poolp;
    total = 0;
    count = 0;
    for (item = 0; !buffer_pool_isexternal(that, item); ++item) {
        subcount = 0;
        for (buffer = buffer_pool_first(that, item); buffer != (diminuto_buffer_t *)0; buffer = buffer->header.next) {
            ++subcount;
        }
        length = buffer_pool_size(that, item);
        size = length + sizeof(diminuto_buffer_t);
        subtotal = subcount * size;
        count += subcount;
        total += subtotal;
        if (subcount > 0) {
            DIMINUTO_LOG_INFORMATION("diminuto_buffer_pool_log: pool=%p index=%d length=%zubytes size=%zubytes subcount=%zu subtotal=%zubytes\n", poolp, item, length, size, subcount, subtotal);
        } else {
            DIMINUTO_LOG_DEBUG("diminuto_buffer_pool_log: pool=%p index=%d length=%zubytes size=%zubytes subcount=%zu subtotal=%zubytes\n", poolp, item, length, size, subcount, subtotal);
        }
    }

    DIMINUTO_LOG_INFORMATION("diminuto_buffer_pool_log: pool=%p count=%zu total=%zubytes\n", poolp, count, total);

    return total;
}
