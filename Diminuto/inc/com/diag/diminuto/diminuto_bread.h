/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_BREAD_
#define _H_COM_DIAG_DIMINUTO_BREAD_

/**
 * @file
 * @copyright Copyright 2021 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This defines the public API for the Buffered Read (BREAD) feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

typedef int (diminuto_bread_f)(void * vp, void * bp, size_t ll);

typedef struct DiminutoBread {
    diminuto_bread_f * bread_read;
    void * bread_void;
    uint8_t * bread_begin;
    uint8_t * bread_end;
    uint8_t * bread_producer;
    uint8_t * bread_consumer;
    size_t bread_used;
    size_t bread_free;
} diminuto_bread_t;

static inline diminuto_bread_t * diminuto_bread_init(diminuto_bread_t * sp, diminuto_bread_f * fp, void * vp, void * bp, size_t ll) {
    sp->bread_read = fp;
    sp->bread_void = vp;
    sp->bread_begin = (uint8_t *)bp;
    sp->bread_end = sp->bread_begin + ll;
    sp->bread_producer = sp->bread_begin;
    sp->bread_consumer = sp->bread_begin;
    sp->bread_used = 0;
    sp->bread_free = ll;
}

static inline diminuto_bread_t * diminuto_bread_fini(diminuto_bread_t * sp) {
    return (diminuto_bread_t *)0;
}

static inline diminuto_bread_t * diminuto_bread_alloc(diminuto_bread_f * fp, void * vp, size_t ll) {
    diminuto_bread_t * sp = (diminuto_bread_t *)0;
    uint8_t * bp = (uint8_t *)0;

    sp = (diminuto_bread_t *)malloc(sizeof(diminuto_bread_t));
    if (sp == (diminuto_bread_t *)0) {
       /* Do nothing. */
    } else {
        bp = (uint8_t *)malloc(ll);
        if (bp == (uint8_t *)0) {
            free(sp);
            sp = (diminuto_bread_t *)0;
        } else {
            (void)diminuto_bread_init(sp, fp, vp, bp, ll);
        }
    }

    return sp;
}

static void diminuto_bread_free(diminuto_bread_t * sp) {
    if (sp != (diminuto_bread_t *)0) {
        diminuto_bread_fini(sp);
        free(sp->bread_begin);
    }
    free(sp);
}

static bool diminuto_bread_ready(diminuto_bread_t * sp) {
    return (sp->bread_used > 0);
}

extern ssize_t diminuto_bread_read(diminuto_bread_t * sp, void * bp, size_t ll);

extern void diminuto_bread_dump(const diminuto_bread_t * sp);

#endif
