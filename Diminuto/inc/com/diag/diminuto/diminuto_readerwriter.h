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
 * S, Tardieu, "The third readers-writers problem", rfc1149.net, 2011-11-07
 */

/*******************************************************************************
 * PREREQUISITES
 ******************************************************************************/

#include "com/diag/diminuto/diminuto_types.h"
#include "com/diag/diminuto/diminuto_widthof.h"
#include "com/diag/diminuto/diminuto_bits.h"
#include "com/diag/diminuto/diminuto_ring.h"
#include <errno.h>
#include <stdint.h>
#include <pthread.h>

/*******************************************************************************
 * CONSTANTS
 ******************************************************************************/

enum DiminutoReaderWriterErrno {
    DIMINUTO_READERWRITER_ERROR     = EIO,
    DIMINUTO_READERWRITER_INVALID   = EINVAL,
    DIMINUTO_READERWRITER_FULL      = EXFULL,
    DIMINUTO_READERWRITER_EMPTY     = ENOMEM,
};

enum DiminutoReaderWriterType {
    DIMINUTO_READERWRITER_NONE      = -1,
    DIMINUTO_READERWRITER_READER    =  0,
    DIMINUTO_READERWRITER_WRITER    =  1,
};

/*******************************************************************************
 * TYPES
 ******************************************************************************/

typedef uint64_t diminuto_readerwriter_bits_t;

#define DIMINUTO_READERWRITER_COUNT(_CAPACITY_) \
    diminuto_bits_count(diminuto_readerwriter_bits_t, _CAPACITY_)

typedef struct DiminutoReaderWriter {
    diminuto_readerwriter_bits_t * buffer;  /**< Pointer to ring buffer data. */
    pthread_mutex_t mutex;                  /**< Mutual exclusion semaphore. */
    pthread_cond_t reader;                  /**< Queue of pending readers. */
    pthread_cond_t writer;                  /**< Queue of pending writers. */
    diminuto_ring_t ring;                   /**< Ring buffer metadata. */
    /*
     * If (active > 0) it is the number of active readers.
     * If (active == -1) it indicates a single active writer.
     */
    int active;                             /**< Number of active threads. */
} diminuto_readerwriter_t;

#define DIMINUTO_READERWRITER_INITIALIZER(_BUFFER_, _CAPACITY_) \
    { \
        (_BUFFER_), \
        PTHREAD_MUTEX_INITIALIZER, \
        PTHREAD_COND_INITIALIZER, \
        PTHREAD_COND_INITIALIZER, \
        DIMINUTO_RING_INITIALIZER(_CAPACITY_), \
        0, \
        0, \
        0, \
    }

/*******************************************************************************
 * STRUCTORS
 ******************************************************************************/

extern diminuto_readerwriter_t * diminuto_readerwriter_init(diminuto_readerwriter_t * rwp, diminuto_readerwriter_bits_t * buffer, size_t capacity);

extern diminuto_readerwriter_t * diminuto_readerwriter_fini(diminuto_readerwriter_t * rwp);

/*******************************************************************************
 * HELPERS
 ******************************************************************************/

static inline size_t diminuto_readerwriter_total(diminuto_readerwriter_t * rwp) {
    return diminuto_ring_used(&(rwp->ring));
}

/*******************************************************************************
 * ACTIONS
 ******************************************************************************/

extern int diminuto_reader_begin(diminuto_readerwriter_t * rwp, diminuto_ticks_t clocktime);

extern int diminuto_reader_end(diminuto_readerwriter_t * rwp);

extern int diminuto_writer_begin(diminuto_readerwriter_t * rwp, diminuto_ticks_t clocktime);

extern int diminuto_writer_end(diminuto_readerwriter_t * rwp);

/*******************************************************************************
 * GENERATORS
 ******************************************************************************/

#define DIMINUTO_READER_BEGIN(_RWP_) \
    do { \
        diminuto_readerwriter_t * diminuto_readerwriter_rwp = (diminuto_readerwriter_t *)0; \
        diminuto_readerwriter_rwp = (_RWP_); \
        if (diminuto_reader_begin(diminuto_readerwriter_rwp, DIMINUTO_READERWRITER_INFINITY) == 0) { \
            do { \
                (void)0

#define DIMINUTO_READER_END \
            } while (0); \
            diminuto_reader_end(diminuto_readerwriter_rwp); \
        } \
    } while (0)

#define DIMINUTO_WRITER_BEGIN(_RWP_) \
    do { \
        diminuto_readerwriter_t * diminuto_readerwriter_rwp = (diminuto_readerwriter_t *)0; \
        diminuto_readerwriter_rwp = (_RWP_); \
        if (diminuto_writer_begin(diminuto_readerwriter_rwp, DIMINUTO_READERWRITER_INFINITY) == 0) { \
            do { \
                (void)0

#define DIMINUTO_WRITER_END \
            } while (0); \
            diminuto_writer_end(diminuto_readerwriter_rwp); \
        } \
    } while (0)

#endif
