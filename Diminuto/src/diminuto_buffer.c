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
 * although it is a completely different (and IMNSHO better) implementation.
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
        unsigned int index;
        char bytes[8]; /* To guarantee header size and payload alignment. */
    } header;
    char payload[0];
} diminuto_buffer_t;

/*
 * [0]: 2^3  =    8 + 8 =   16
 * [1]: 2^4  =   16 + 8 =   24
 * [2]: 2^5  =   32 + 8 =   40
 * [3]: 2^6  =   64 + 8 =   72
 * [4]: 2^7  =  128 + 8 =  136
 * [5]: 2^8  =  256 + 8 =  264
 * [6]: 2^9  =  512 + 8 =  520
 * [7]: 2^10 = 1024 + 8 = 1032
 * [8]: 2^11 = 2048 + 8 = 2056
 * [9]: 2^12 = 4096 + 8 = 4104
 */

static diminuto_buffer_t * pool[10] = { (diminuto_buffer_t *)0 };

static inline unsigned int hash(size_t requested, size_t * actualp)
{
    unsigned int index;
    size_t actual;

    for (index = 0, actual = (1 << 3); actual < requested; ++index, actual <<= 1) {
        continue;
    }

    if (index >= countof(pool)) {
        actual = requested;
    }

    *actualp = actual + sizeof(diminuto_buffer_t);

    return index;
}

static inline size_t effective(unsigned int index)
{
    /*
     * N.B. If index is greater than or equal to the number of slots in the
     * pool array then we can't compute the effective size of the memory block.
     */
    return sizeof(diminuto_buffer_t) + (1 << (index + 3));
}

/******************************************************************************/

void * diminuto_buffer_malloc(size_t size)
{
    unsigned int index;
    size_t actual;
    diminuto_buffer_t * here;

    index = hash(size, &actual);
    if ((index >= countof(pool)) || (pool[index] == (diminuto_buffer_t *)0)) {
        here = (diminuto_buffer_t *)malloc(actual);
    }  else {
        here = pool[index];
        pool[index] = here->header.next;
    }
    /* Consider clearing memory here for security purposes. */
    here->header.index = index;

    return here->payload;
}

void diminuto_buffer_free(void * ptr)
{
    if (ptr != (void *)0) {
        unsigned int index;
        diminuto_buffer_t * here;

        here = containerof(diminuto_buffer_t, payload, ptr);
        index = here->header.index;
        if (index >= countof(pool)) {
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
    return (void *)0;
}

void * diminuto_buffer_calloc(size_t nmemb, size_t size)
{
    return (void *)0;
}

char * diminuto_buffer_strdup(const char *s)
{
    return (char *)0;
}

char * diminuto_buffer_strndup(const char *s, size_t n)
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
    for (index = 0; index < countof(pool); ++index) {
        count = 0;
        for (here = pool[index]; here != (diminuto_buffer_t *)0; here = here->header.next) {
            ++count;
        }
        if (count > 0) {
            DIMINUTO_LOG_DEBUG("diminuto_buffer_log: pool#%u[%u]@%zubytes\n", index, count, size = effective(index));
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

    for (index = 0; index < countof(pool); ++index) {
        for (here = pool[index]; here != (diminuto_buffer_t *)0; here = there) {
            there = here->header.next; /* To make valgrind(1) happy. */
            free(here);
        }
        pool[index] = (diminuto_buffer_t *)0;
    }
}
