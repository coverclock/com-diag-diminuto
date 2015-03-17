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
 * <stdlib.h>-LIKE FUNCTIONS
 ******************************************************************************/

void * diminuto_buffer_malloc(size_t size)
{
    void * ptr;

    if (size == 0) {
        errno = 0;
        ptr = (void *)0;
    } else {
        size_t item;
        size_t actual;
        diminuto_buffer_t * buffer;

        item = buffer_hash(size, &actual);
        if (buffer_isexternal(item)) {
            buffer = buffer_malloc(actual, fail);
        } else if (buffer_first(item) == (diminuto_buffer_t *)0) {
            buffer = buffer_malloc(actual, fail);
        }  else {
            buffer = buffer_get(item);
        }
        if (buffer == (diminuto_buffer_t *)0) {
            ptr = (void *)0;
        } else {
            ptr = buffer_init(buffer, item, actual);
        }
    }

    if (debug) {
        DIMINUTO_LOG_DEBUG("diminuto_buffer_malloc: size=%zu pointer=%p\n", size, ptr);
    }

    return ptr;
}

void diminuto_buffer_free(void * ptr)
{
    if (debug) {
        DIMINUTO_LOG_DEBUG("diminuto_buffer_free: ptr=%p\n", ptr);
    }

    if (ptr != (void *)0) {
        size_t item;
        diminuto_buffer_t * buffer;

        buffer = containerof(diminuto_buffer_t, payload, ptr);
        item = buffer->header.item;
        if (buffer_isexternal(item)) {
            free(buffer);
        } else {
            buffer_put(buffer, item);
        }
    }
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
        actual = buffer_effective(buffer->header.item);
        (void)buffer_hash(size, &requested);
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
    int item;
    diminuto_buffer_t * buffer;

    for (item = 0; !buffer_isexternal(item); ++item) {
        while ((buffer = buffer_get(item)) != (diminuto_buffer_t *)0) {
            free(buffer);
        }
    }
}

size_t diminuto_buffer_prealloc(size_t nmemb, size_t size)
{
    size_t total = 0;
    size_t item;
    size_t actual;
    diminuto_buffer_t * buffer;

    if (size == 0) {
        /* Do nothing. */
    } else if (nmemb == 0) {
        /* Do nothing. */
    } else if (buffer_isexternal(item = buffer_hash(size, &actual))) {
        /* Do nothing. */
    } else {
        while ((nmemb--) > 0) {
            buffer = (diminuto_buffer_t *)buffer_malloc(actual, fail);
            if (buffer == (diminuto_buffer_t *)0) {
                break;
            }
            buffer_put(buffer, item);
            total += actual;
        }
    }

    return total;
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

size_t diminuto_buffer_log(void)
{
    size_t total;
    size_t subtotal;
    size_t item;
    size_t count;
    size_t length;
    size_t size;
    diminuto_buffer_t * buffer;

    total = 0;
    for (item = 0; !buffer_isexternal(item); ++item) {
        count = 0;
        for (buffer = buffer_first(item); buffer != (diminuto_buffer_t *)0; buffer = buffer->header.next) {
            ++count;
        }
        if (count > 0) {
            length = buffer_size(item);
            size = length + sizeof(diminuto_buffer_t);
            subtotal = count * size;
            total += subtotal;
            DIMINUTO_LOG_DEBUG("diminuto_buffer_log: length=%zubytes size=%zubytes count=%u subtotal=%zubytes\n", length, size, count, subtotal);
        }
    }

    DIMINUTO_LOG_DEBUG("diminuto_buffer_log: total=%zubytes\n", total);

    return total;
}

/*******************************************************************************
 * PRIVATE FUNCTIONS
 ******************************************************************************/

unsigned int diminuto_buffer_hash(size_t requested, size_t * actualp)
{
    return buffer_hash(requested, actualp);
}

size_t diminuto_buffer_effective(unsigned int item)
{
    return buffer_effective(item);
}
