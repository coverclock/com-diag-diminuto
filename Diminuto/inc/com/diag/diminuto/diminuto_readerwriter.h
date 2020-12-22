/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_READERWRITER_
#define _H_COM_DIAG_DIMINUTO_READERWRITER_

/**
 * @file
 * @copyright Copyright 2020 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * Work In Progress!
 *
 * REFERENCES
 *
 * Wikipedia, "Readers-writers problem", 2020-11-23
 *
 * V. Popov, O. Mazonka, "Faster Fair Solution for the Reader-Writer Problem",
 * 2013
 *
 * S. Tardieu, "The third readers-writers problem", rfc1149.net, 2011-11-07
 */

/*******************************************************************************
 * PREREQUISITES
 ******************************************************************************/

#include "com/diag/diminuto/diminuto_widthof.h"
#include "com/diag/diminuto/diminuto_bits.h"
#include "com/diag/diminuto/diminuto_ring.h"
#include <errno.h>
#include <stdint.h>
#include <pthread.h>

/*******************************************************************************
 * CONSTANTS
 ******************************************************************************/

enum DiminutoReaderWriterError {
    DIMINUTO_READERWRITER_ERROR     = EIO,      /**< ReaderWriter bug. */
    DIMINUTO_READERWRITER_ORDER     = EINVAL,   /**< Calling order wrong. */
    DIMINUTO_READERWRITER_FULL      = ENOSPC,   /**< Ring too small. */
    DIMINUTO_READERWRITER_STATE     = ENOENT,   /**< Unexpected state. */
};

enum DiminutoReaderWriterType {
    DIMINUTO_READERWRITER_NONE      = -1,
    DIMINUTO_READERWRITER_READER    =  0,
    DIMINUTO_READERWRITER_WRITER    =  1,
};

/*******************************************************************************
 * TYPES
 ******************************************************************************/

typedef uint64_t diminuto_readerwriter_buffer_t;

#define DIMINUTO_READERWRITER_COUNT(_CAPACITY_) \
    DIMINUTO_BITS_COUNT(diminuto_readerwriter_buffer_t, _CAPACITY_)

typedef struct DiminutoReaderWriter {
    diminuto_readerwriter_buffer_t * buffer;    /**< Ring buffer. */
    pthread_mutex_t mutex;                      /**< Mutex semaphore. */
    pthread_cond_t reader;                      /**< Waiting readers. */
    pthread_cond_t writer;                      /**< Waiting writers. */
    diminuto_ring_t ring;                       /**< Ring metadata. */
    /*
     * If (active > 0) it is the number of active readers.
     * If (active == -1) it indicates a single active writer.
     */
    int active;                                 /**< Number active. */
} diminuto_readerwriter_t;

#define DIMINUTO_READERWRITER_INITIALIZER(_BUFFER_, _CAPACITY_) \
    { \
        (_BUFFER_), \
        PTHREAD_MUTEX_INITIALIZER, \
        PTHREAD_COND_INITIALIZER, \
        PTHREAD_COND_INITIALIZER, \
        DIMINUTO_RING_INITIALIZER(_CAPACITY_), \
        0, \
    }

/*******************************************************************************
 * STRUCTORS
 ******************************************************************************/

extern diminuto_readerwriter_t * diminuto_readerwriter_init(diminuto_readerwriter_t * rwp, diminuto_readerwriter_buffer_t * buffer, size_t capacity);

extern diminuto_readerwriter_t * diminuto_readerwriter_fini(diminuto_readerwriter_t * rwp);

/*******************************************************************************
 * HELPERS
 ******************************************************************************/

static inline unsigned int diminuto_readerwriter_waiting(diminuto_readerwriter_t * rwp) {
    return diminuto_ring_used(&(rwp->ring));
}

static inline int diminuto_readerwriter_active(diminuto_readerwriter_t * rwp) {
    return rwp->active;
}

/*******************************************************************************
 * ACTIONS
 ******************************************************************************/

extern int diminuto_reader_begin(diminuto_readerwriter_t * rwp);

extern int diminuto_reader_end(diminuto_readerwriter_t * rwp);

extern int diminuto_writer_begin(diminuto_readerwriter_t * rwp);

extern int diminuto_writer_end(diminuto_readerwriter_t * rwp);

/*******************************************************************************
 * GENERATORS
 ******************************************************************************/

#define DIMINUTO_READER_BEGIN(_RWP_) \
    do { \
        diminuto_readerwriter_t * diminuto_readerwriter_rwp = (diminuto_readerwriter_t *)0; \
        int diminuto_readerwriter_reader = !0; \
        diminuto_readerwriter_rwp = (_RWP_); \
        if (diminuto_reader_begin(diminuto_readerwriter_rwp) == 0) { \
            do { \
                (void)0

#define DIMINUTO_READER_END \
            } while (0); \
            diminuto_reader_end(diminuto_readerwriter_rwp); \
        } \
        diminuto_readerwriter_reader = 0; \
    } while (0)

#define DIMINUTO_WRITER_BEGIN(_RWP_) \
    do { \
        diminuto_readerwriter_t * diminuto_readerwriter_rwp = (diminuto_readerwriter_t *)0; \
        int diminuto_readerwriter_writer = !0; \
        diminuto_readerwriter_rwp = (_RWP_); \
        if (diminuto_writer_begin(diminuto_readerwriter_rwp) == 0) { \
            do { \
                (void)0

#define DIMINUTO_WRITER_END \
            } while (0); \
            diminuto_writer_end(diminuto_readerwriter_rwp); \
        } \
        diminuto_readerwriter_writer = 0; \
    } while (0)

#endif
