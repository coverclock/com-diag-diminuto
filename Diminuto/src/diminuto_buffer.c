/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2015 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 *
 * This was inspired by the buffer feature in the NCAR libtools package,
 * although it is a completely different implementation.
 *
 * REFERENCES
 *
 * J. L. Sloan, "Parcels with TAGS", NCAR TECHNICAL NOTE, NCAR/TN-377+IA,
 * National Center for Atmospheric Research, 1992-10, section 4, "Storage
 * Management", p. 10, http://www.diag.com/ftp/NCAR_Sloan_Parcels.pdf
 */

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "com/diag/diminuto/diminuto_buffer.h"
#include "com/diag/diminuto/diminuto_countof.h"
#include "com/diag/diminuto/diminuto_containerof.h"
#include "com/diag/diminuto/diminuto_log.h"

/******************************************************************************/

typedef struct DiminutoBuffer {
    union {
        struct DiminutoBuffer * next;
        size_t index;
        char bytes[8]; /* Guarantees header size and payload alignment. */
    } header;
    char payload[0];
} diminuto_buffer_t;

/*
 * These are the sizes I chose for each quanta in the buffer pool, but you
 * should be able to put any monotonically increasing sizes here and it should
 * work. You can also increase the number of quanta in the pool just by adding
 * more entries.
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

static inline unsigned int hash(size_t requested, size_t * actualp)
{
    size_t index;
    size_t actual;

    for (index = 0; (index < countof(POOL)) && (POOL[index] < requested); ++index) {
        continue;
    }

    actual = (index < countof(POOL)) ? POOL[index] : requested;

    *actualp = actual + sizeof(diminuto_buffer_t);

    return index;
}

static inline size_t effective(size_t index)
{
    return (index < countof(POOL)) ? POOL[index] + sizeof(diminuto_buffer_t) : index;
}

/******************************************************************************/

void * diminuto_buffer_malloc(size_t size)
{
    size_t index;
    size_t actual;
    diminuto_buffer_t * here;

    index = hash(size, &actual);
    if (index >= countof(POOL)) {
        here = (diminuto_buffer_t *)malloc(actual);
        here->header.index = actual;
    } else if (pool[index] == (diminuto_buffer_t *)0) {
        here = (diminuto_buffer_t *)malloc(actual);
        here->header.index = index;
    }  else {
        here = pool[index];
        pool[index] = here->header.next;
        here->header.index = index;
    }
    /* Consider clearing payload here for security purposes. */

    return here->payload;
}

void diminuto_buffer_free(void * ptr)
{
    if (ptr != (void *)0) {
        size_t index;
        diminuto_buffer_t * here;

        here = containerof(diminuto_buffer_t, payload, ptr);
        index = here->header.index;
        if (index >= countof(POOL)) {
            free(here);
        } else {
            here->header.next = pool[index];
            pool[index] = here;
        }
    }
}

/******************************************************************************/

void * diminuto_buffer_realloc(void * ptr, size_t size)
{
    diminuto_buffer_t * here;
    diminuto_buffer_t * there;
    size_t index;
    size_t length;

    here = containerof(diminuto_buffer_t, payload, ptr);
    index = here->header.index;
    there = diminuto_buffer_malloc(size);
    size += sizeof(diminuto_buffer_t);
    length = effective(index);
    if (length > size) { length = size; }
    memcpy(there, here, length);
    diminuto_buffer_free(here);

    return there->payload;
}

void * diminuto_buffer_calloc(size_t nmemb, size_t size)
{
    return diminuto_buffer_malloc(nmemb * size);
}

char * diminuto_buffer_strdup(const char * s)
{
    return (char *)0;
}

char * diminuto_buffer_strndup(const char * s, size_t n)
{
    return (char *)0;
}

/******************************************************************************/

unsigned int diminuto_buffer_hash(size_t requested, size_t * actualp)
{
    return hash(requested, actualp);
}

size_t diminuto_buffer_effective(unsigned int index)
{
    return effective(index);
}

void diminuto_buffer_log(void)
{
    unsigned int index;
    unsigned int count;
    size_t size;
    size_t total;
    diminuto_buffer_t * here;
    diminuto_buffer_t * there;

    total = 0;
    for (index = 0; index < countof(POOL); ++index) {
        count = 0;
        for (here = pool[index]; here != (diminuto_buffer_t *)0; here = here->header.next) {
            ++count;
        }
        if (count > 0) {
            DIMINUTO_LOG_DEBUG("diminuto_buffer_log: index=%u count=%u size=%zubytes\n", index, count, size = effective(index));
            total += size * count;
        }
    }

    DIMINUTO_LOG_DEBUG("diminuto_buffer_log: total=%zubytes\n", total);
}

void diminuto_buffer_fini(void)
{
    int index;
    diminuto_buffer_t * here;
    diminuto_buffer_t * there;

    for (index = 0; index < countof(POOL); ++index) {
        for (here = pool[index]; here != (diminuto_buffer_t *)0; here = there) {
            there = here->header.next; /* To make valgrind(1) happy. */
            free(here);
        }
        pool[index] = (diminuto_buffer_t *)0;
    }
}
