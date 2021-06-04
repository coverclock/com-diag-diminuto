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
 * The Buffered Read function works similarly to the equivalent functions
 * in the standard I/O library except instead of a file descriptor the
 * underlying read function is an abstract read function provided by the
 * caller.
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

/**
 * This defines the prototype of the abstract read function that is
 * provided by the caller.
 */
typedef int (diminuto_bread_f)(void * vp, void * bp, size_t ll);

/**
 * This structure describes the format of the Buffered Read object.
 */
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

/**
 * Initialize a Buffered Read object.
 * @param sp points to the uninitialized object.
 * @param fp points to the abstract read function.
 * @param vp is passed to the abstract read function for its own state.
 * @param bp points to the feature (not caller) buffer.
 * @param ll is the size of of the feature buffer in bytes.
 * @return a pointer ot the Buffered Read object.
 */
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

/**
 * Finalize an initialized Buffered Read object.
 * @param sp points to the Buffered Read object.
 * @return a null pointer or a pointer to the object if an error occurred.
 */
static inline diminuto_bread_t * diminuto_bread_fini(diminuto_bread_t * sp) {
    return (diminuto_bread_t *)0;
}

/**
 * Allocate and initialize a Buffered Read object and its feature buffer.
 * @param fp points to the abstract read function.
 * @param vp is passed to the abstract read function for its own state.
 * @param ll is the size of of the feature buffer in bytes to allocate.
 * @return a pointer ot the Buffered Read object.
 */
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

/**
 * Free an allocated and initialized Buffered Read object and its feature
 * buffer.
 * @param sp points to the allocated object.
 */
static void diminuto_bread_free(diminuto_bread_t * sp) {
    if (sp != (diminuto_bread_t *)0) {
        diminuto_bread_fini(sp);
        free(sp->bread_begin);
    }
    free(sp);
}

/**
 * Return true if a Buffered Read object contains data.
 * @param sp points to the object.
 * @return true if it contains data, false otherwise.
 */
static bool diminuto_bread_ready(diminuto_bread_t * sp) {
    return (sp->bread_used > 0);
}

/**
 * Read data from a Buffered Read object. The caller will block if and only
 * if there is no data in the object.
 * @param sp points to the object.
 * @param bp points to the caller (not the feature) buffer.
 * @param ll is the size of the caller buffer in octets.
 * @return the number of bytes put in the caller buffer, 0 if EOF, <0 for error.
 */
extern ssize_t diminuto_bread_read(diminuto_bread_t * sp, void * bp, size_t ll);

/**
 * Dump the state of a Buffered Read object to standard error or the system log.
 * @param sp points to the object.
 */
extern void diminuto_bread_dump(const diminuto_bread_t * sp);

#endif
