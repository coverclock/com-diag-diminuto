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
#include "diminuto_buffer.h"
#include "com/diag/diminuto/diminuto_buffer.h"
#include "com/diag/diminuto/diminuto_countof.h"
#include "com/diag/diminuto/diminuto_containerof.h"
#include "com/diag/diminuto/diminuto_log.h"

/*******************************************************************************
 * OPTIONS
 ******************************************************************************/

static int debug = 0;

static int fail = 0;

/*******************************************************************************
 * HELPERS
 ******************************************************************************/

static inline unsigned int hash(size_t requested, size_t * actualp)
{
    size_t item;
    size_t actual;

    for (item = 0; (item < diminuto_buffer_countof) && (DIMINUTO_BUFFER_POOL[item] < requested); ++item) {
        continue;
    }

    actual = (item < diminuto_buffer_countof) ? DIMINUTO_BUFFER_POOL[item] : requested;

    *actualp = actual + sizeof(diminuto_buffer_t);

    return item;
}

static inline size_t effective(size_t item)
{
    return (item < diminuto_buffer_countof) ? DIMINUTO_BUFFER_POOL[item] + sizeof(diminuto_buffer_t) : item;
}

static inline void * buffer_malloc(size_t size)
{
    void * pointer;

    if (!fail) {
        pointer = (diminuto_buffer_t *)malloc(size);
    } else {
        pointer = (diminuto_buffer_t *)0;
        errno = ENOMEM;
    }

    return pointer;
}

static inline void buffer_free(diminuto_buffer_t * buffer, size_t item)
{
    buffer->header.next = diminuto_buffer_pool[item];
    diminuto_buffer_pool[item] = buffer;
}

/*******************************************************************************
 * <stdlib.h>-LIKE FUNCTIONS
 ******************************************************************************/

void * diminuto_buffer_malloc(size_t size)
{
    void * pointer;

    if (size == 0) {
        errno = 0;
        pointer = (void *)0;
    } else {
        size_t item;
        size_t actual;
        diminuto_buffer_t * here;

        item = hash(size, &actual);
        if (item >= diminuto_buffer_countof) {
            here = (diminuto_buffer_t *)buffer_malloc(actual);
        } else if (diminuto_buffer_pool[item] == (diminuto_buffer_t *)0) {
            here = (diminuto_buffer_t *)buffer_malloc(actual);
        }  else {
            here = diminuto_buffer_pool[item];
            diminuto_buffer_pool[item] = here->header.next;
        }
        if (here == (diminuto_buffer_t *)0) {
            pointer = (void *)0;
        } else {
            here->header.item = (item < diminuto_buffer_countof) ? item : actual;
            pointer = here->payload;
        }
    }

    if (debug) {
        DIMINUTO_LOG_DEBUG("diminuto_buffer_malloc: size=%zu pointer=%p\n", size, pointer);
    }

    return pointer;
}

void diminuto_buffer_free(void * ptr)
{
    if (debug) {
        DIMINUTO_LOG_DEBUG("diminuto_buffer_free: ptr=%p\n", ptr);
    }

    if (ptr != (void *)0) {
        size_t item;
        diminuto_buffer_t * here;

        here = containerof(diminuto_buffer_t, payload, ptr);
        item = here->header.item;
        if (item >= diminuto_buffer_countof) {
            free(here);
        } else {
            buffer_free(here, item);
        }
    }
}

void * diminuto_buffer_realloc(void * ptr, size_t size)
{
    void * pointer;

    if (ptr == (void *)0) {
        pointer = diminuto_buffer_malloc(size);
    } else if (size == 0) {
        diminuto_buffer_free(ptr);
        errno = 0;
        pointer = (void *)0;
    } else {
        diminuto_buffer_t * here;
        diminuto_buffer_t * there;
        size_t actual;
        size_t length;

        here = containerof(diminuto_buffer_t, payload, ptr);
        actual = effective(here->header.item);
        length = actual - sizeof(diminuto_buffer_t);
        if (size <= length) {
            pointer = ptr;
        } else {
            pointer = diminuto_buffer_malloc(size);
            if (pointer != (void *)0) {
                memcpy(pointer, ptr, length);
            }
            diminuto_buffer_free(ptr);
        }
    }

    if (debug) {
        DIMINUTO_LOG_DEBUG("diminuto_buffer_realloc: ptr=%p size=%zu pointer=%p\n", ptr, size, pointer);
    }

    return pointer;
}

void * diminuto_buffer_calloc(size_t nmemb, size_t size)
{
    void * pointer;
    size_t length;

    length = nmemb * size;
    pointer = diminuto_buffer_malloc(length);
    if (pointer != (void *)0) {
        memset(pointer, 0, length);
    }

    if (debug) {
        DIMINUTO_LOG_DEBUG("diminuto_buffer_calloc: nmemb=%zu size=%zu ptr=%p\n", nmemb, size, pointer);
    }

    return pointer;
}

/*******************************************************************************
 * <string.h>-LIKE FUNCTIONS
 ******************************************************************************/

char * diminuto_buffer_strdup(const char * s)
{
    char * pointer;
    size_t length;

    length = strlen(s);
    pointer = (char *)diminuto_buffer_malloc(length + 1);
    if (pointer != (char *)0) {
        strcpy(pointer, s);
    }

    return pointer;
}

char * diminuto_buffer_strndup(const char * s, size_t n)
{
    char * pointer;
    size_t length;

    length = strnlen(s, n);
    pointer = (char *)diminuto_buffer_malloc(length + 1);
    if (pointer != (char *)0) {
        strncpy(pointer, s, length);
        pointer[length] = '\0';
    }

    return pointer;
}

/*******************************************************************************
 * ANCILLARY FUNCTIONS
 ******************************************************************************/

void diminuto_buffer_fini(void)
{
    int item;
    diminuto_buffer_t * here;
    diminuto_buffer_t * there;

    for (item = 0; item < diminuto_buffer_countof; ++item) {
        for (here = diminuto_buffer_pool[item]; here != (diminuto_buffer_t *)0; here = there) {
            there = here->header.next; /* To make valgrind(1) happy. */
            free(here);
        }
        diminuto_buffer_pool[item] = (diminuto_buffer_t *)0;
    }
}

size_t diminuto_buffer_prealloc(size_t nmemb, size_t size)
{
    size_t total = 0;
    size_t item;
    size_t actual;
    diminuto_buffer_t * here;

    if (size == 0) {
        /* Do nothing. */
    } else if (nmemb == 0) {
        /* Do nothing. */
    } else if ((item = hash(size, &actual)) >= diminuto_buffer_countof) {
        /* Do nothing. */
    } else {
        while ((nmemb--) > 0) {
            here = (diminuto_buffer_t *)buffer_malloc(actual);
            if (here == (diminuto_buffer_t *)0) {
                break;
            }
            buffer_free(here, item);
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
    diminuto_buffer_t * here;
    diminuto_buffer_t * there;

    total = 0;
    for (item = 0; item < diminuto_buffer_countof; ++item) {
        count = 0;
        for (here = diminuto_buffer_pool[item]; here != (diminuto_buffer_t *)0; here = here->header.next) {
            ++count;
        }
        if (count > 0) {
            length = DIMINUTO_BUFFER_POOL[item];
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
    return hash(requested, actualp);
}

size_t diminuto_buffer_effective(unsigned int item)
{
    return effective(item);
}
