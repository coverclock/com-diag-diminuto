/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2015 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 *
 * This feature is unit tested as part of the buffer unit test.
 */

#include "diminuto_buffer_pool.h"
#include "com/diag/diminuto/diminuto_countof.h"
#include "com/diag/diminuto/diminuto_containerof.h"
#include "com/diag/diminuto/diminuto_log.h"

void * diminuto_buffer_pool_get(diminuto_buffer_pool_t * poolp, size_t size, int nomalloc)
{
    void * ptr;

    if (size == 0) {
        errno = 0;
        ptr = (void *)0;
    } else {
        diminuto_buffer_meta_t * that;
        size_t item;
        size_t actual;
        diminuto_buffer_t * buffer;

        that = (diminuto_buffer_meta_t *)poolp;
        item = buffer_pool_hash(that, size, &actual);
        if (buffer_pool_isexternal(that, item)) {
            buffer = buffer_pool_malloc(that, actual, nomalloc);
        } else if (buffer_pool_first(that, item) == (diminuto_buffer_t *)0) {
            buffer = buffer_pool_malloc(that, actual, nomalloc);
        }  else {
            buffer = buffer_pool_get(that, item);
        }
        if (buffer == (diminuto_buffer_t *)0) {
            ptr = (void *)0;
        } else {
            ptr = buffer_pool_init(that, buffer, item, actual);
        }
    }

    return ptr;
}

void diminuto_buffer_pool_put(diminuto_buffer_pool_t * poolp, void * ptr)
{
    if (ptr != (void *)0) {
        diminuto_buffer_meta_t * that;
        size_t item;
        diminuto_buffer_t * buffer;

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

void diminuto_buffer_pool_fini(diminuto_buffer_pool_t * poolp)
{
    diminuto_buffer_meta_t * that;
    int item;
    diminuto_buffer_t * buffer;

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
    diminuto_buffer_meta_t * that;
    size_t item;
    size_t actual;
    diminuto_buffer_t * buffer;

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
    size_t total;
    diminuto_buffer_meta_t * that;
    size_t subtotal;
    size_t item;
    size_t count;
    size_t length;
    size_t size;
    diminuto_buffer_t * buffer;

    that = (diminuto_buffer_meta_t *)poolp;
    total = 0;
    for (item = 0; !buffer_pool_isexternal(that, item); ++item) {
        count = 0;
        for (buffer = buffer_pool_first(that, item); buffer != (diminuto_buffer_t *)0; buffer = buffer->header.next) {
            ++count;
        }
        if (count > 0) {
            length = buffer_pool_size(that, item);
            size = length + sizeof(diminuto_buffer_t);
            subtotal = count * size;
            total += subtotal;
            DIMINUTO_LOG_DEBUG("diminuto_buffer_pool_log: pool=%p, length=%zubytes size=%zubytes count=%u subtotal=%zubytes\n", poolp, length, size, count, subtotal);
        }
    }

    DIMINUTO_LOG_DEBUG("diminuto_buffer_pool_log: pool=%p total=%zubytes\n", poolp, total);

    return total;
}
