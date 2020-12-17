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
 */

/*******************************************************************************
 * PREREQUISITES
 ******************************************************************************/

#include "com/diag/diminuto/diminuto_types.h"
#include "com/diag/diminuto/diminuto_widthof.h"
#include <errno.h>
#include <stdint.h>
#include <pthread.h>

/*******************************************************************************
 * ENUMERATIONS
 ******************************************************************************/

enum DiminutoReaderWriterErrno {
    DIMINUTO_READERWRITER_ERROR     = EIO,
    DIMINUTO_READERWRITER_INVALID   = EINVAL,
    DIMINUTO_READERWRITER_TIMEDOUT  = ETIMEDOUT,
};

enum DiminutoReaderWriterType {
    DIMINUTO_READERWRITER_READER    = 0,
    DIMINUTO_READERWRITER_WRITER    = 1,
};

/*******************************************************************************
 * CONSTANTS
 ******************************************************************************/

static const diminuto_ticks_t DIMINUTO_READERWRITER_INFINITY = (~(diminuto_ticks_t)0);

/*******************************************************************************
 * TYPES
 ******************************************************************************/

typedef uint32_t diminuto_readerwriter_ring_t;

typedef struct DiminutoReaderWriter {
    pthread_mutex_t mutex;                  /**< Mutual exclusion semaphore. */
    pthread_cond_t readers;                 /**< Queue of pending readers. */
    pthread_cond_t writers;                 /**< Queue of pending writers. */
    uint32_t count;                         /**< Number of slots in ring. */
    uint32_t producer;                      /**< Index of unused slot in ring. */
    uint32_t consumer;                      /**< Index of used slot in ring. */
    diminuto_readerwriter_ring_t * ring;    /**< Ring buffer of bits. */
} diminuto_readerwriter_t;

#define DIMINUTO_READERWRITER_INITIALIZER(_RING_, _COUNT_) \
    { \
        PTHREAD_MUTEX_INITIALIZER, \
        PTHREAD_COND_INITIALIZER, \
        (_COUNT_), \
        0, \
        0, \
    }

#define DIMINUTO_READERWRITER_SIZE(_COUNT_) \
    (((_COUNT_) + diminuto_widthof(diminuto_readerwriter_ring_t) - 1) / diminuto_widthof(diminuto_readerwriter_ring_t))

/*******************************************************************************
 * STRUCTORS
 ******************************************************************************/

extern diminuto_readerwriter_t * diminuto_readerwriter_init(diminuto_readerwriter_t * rwp);

extern diminuto_readerwriter_t * diminuto_readerwriter_fini(diminuto_readerwriter_t * rwp);

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
