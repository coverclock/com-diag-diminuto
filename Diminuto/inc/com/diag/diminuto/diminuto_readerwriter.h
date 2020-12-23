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
#include "com/diag/diminuto/diminuto_ring.h"
#include <errno.h>
#include <stdint.h>
#include <pthread.h>

/*******************************************************************************
 * CONSTANTS
 ******************************************************************************/

enum DiminutoReaderWriterError {
    DIMINUTO_READERWRITER_ERROR         = EIO,      /**< Unspecified error. */
    DIMINUTO_READERWRITER_FULL          = ENOSPC,   /**< Ring too small. */
    DIMINUTO_READERWRITER_UNEXPECTED    = EFAULT,   /**< Unexpected state. */
};

enum DiminutoReaderWriterType {
    DIMINUTO_READERWRITER_WRITER    = -1,   /**< Thread is a writer. */
    DIMINUTO_READERWRITER_NONE      =  0,   /**< No thread. */
    DIMINUTO_READERWRITER_READER    =  1,   /**< Thread is a reader. */
};

/*******************************************************************************
 * TYPES
 ******************************************************************************/

typedef int8_t diminuto_readerwriter_state_t;

typedef struct DiminutoReaderWriter {
    pthread_mutex_t mutex;                      /**< Mutex semaphore. */
    pthread_cond_t reader;                      /**< Waiting readers. */
    pthread_cond_t writer;                      /**< Waiting writers. */
    diminuto_ring_t ring;                       /**< Ring metadata. */
    diminuto_readerwriter_state_t * state;      /**< Ring buffer. */
    int readers;                                /**< (>=0) active readers. */
    int writers;                                /**< {0,1} active writers. */
} diminuto_readerwriter_t;

#define DIMINUTO_READERWRITER_INITIALIZER(_BUFFER_, _CAPACITY_) \
    { \
        PTHREAD_MUTEX_INITIALIZER, \
        PTHREAD_COND_INITIALIZER, \
        PTHREAD_COND_INITIALIZER, \
        DIMINUTO_RING_INITIALIZER(_CAPACITY_), \
        &((_BUFFER_)[0]), \
        0, \
        0, \
    }

/*******************************************************************************
 * STRUCTORS
 ******************************************************************************/

extern diminuto_readerwriter_t * diminuto_readerwriter_init(diminuto_readerwriter_t * rwp, diminuto_readerwriter_state_t * state, size_t capacity);

extern diminuto_readerwriter_t * diminuto_readerwriter_fini(diminuto_readerwriter_t * rwp);

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
