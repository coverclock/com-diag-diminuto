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
 *
 * REFERENCES
 *
 * C. Hoare, "Monitors: An Operating System Structuring Concept", CACM,
 * 17.10, 1974-10
 *
 * L. Lamport, "Time, Clocks, and the Ordering of Events in a Distributed
 * System", CACM, 21.7, 1978-07
 *
 * B. Lampson, D. Redell, "Experience with Processes and Monitors in Mesa",
 * CACM, 23.2, 1980-02
 *
 * S. Tardieu, "The third readers-writers problem", rfc1149.net, 2011-11-07
 *
 * V. Popov, O. Mazonka, "Faster Fair Solution for the Reader-Writer Problem",
 * 2013
 *
 * pthread_cond_broadcast, pthead_cond_signal, Open Group Base Specification
 * Issue 7, 2018 edition, IEEE Std. 1003.1-2017, 2018
 *
 * pthread_cond_timedwait, pthead_cond_wait, Open Group Base Specification
 * Issue 7, 2018 edition, IEEE Std. 1003.1-2017, 2018
 *
 * Wikipedia, "Readers-writers problem", 2020-11-23
 */

/*******************************************************************************
 * PREREQUISITES
 ******************************************************************************/

#include "com/diag/diminuto/diminuto_widthof.h"
#include "com/diag/diminuto/diminuto_ring.h"
#include <errno.h>
#include <stdint.h>
#include <pthread.h>
#include <stdio.h>

/*******************************************************************************
 * CONSTANTS
 ******************************************************************************/

enum DiminutoReaderWriterError {
    DIMINUTO_READERWRITER_ERROR         = EIO,      /**< Unspecified error. */
    DIMINUTO_READERWRITER_FULL          = ENOSPC,   /**< Ring too small. */
    DIMINUTO_READERWRITER_UNEXPECTED    = EFAULT,   /**< Unexpected state. */
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
    FILE * fp;                                  /**< Debug file pointer. */
    int reading;                                /**< Active (>=0) readers. */
    int writing;                                /**< Active {0,1} writers. */
} diminuto_readerwriter_t;

#define DIMINUTO_READERWRITER_INITIALIZER(_BUFFER_, _CAPACITY_) \
    { \
        PTHREAD_MUTEX_INITIALIZER, \
        PTHREAD_COND_INITIALIZER, \
        PTHREAD_COND_INITIALIZER, \
        DIMINUTO_RING_INITIALIZER(_CAPACITY_), \
        &((_BUFFER_)[0]), \
        (FILE *)0,  \
        0, \
        0, \
    }

/*******************************************************************************
 * STRUCTORS
 ******************************************************************************/

extern diminuto_readerwriter_t * diminuto_readerwriter_init(diminuto_readerwriter_t * rwp, diminuto_readerwriter_state_t * state, size_t capacity);

extern diminuto_readerwriter_t * diminuto_readerwriter_fini(diminuto_readerwriter_t * rwp);

/*******************************************************************************
 * GENERATORS
 ******************************************************************************/

#define DIMINUTO_READER_BEGIN(_RWP_) \
    do { \
        extern int diminuto_reader_begin(diminuto_readerwriter_t * rwp); \
        extern void diminuto_reader_cleanup(void * vp); \
        diminuto_readerwriter_t * diminuto_reader_rwp = (diminuto_readerwriter_t *)0; \
        diminuto_reader_rwp = (_RWP_); \
        if (diminuto_reader_begin(diminuto_reader_rwp) == 0) { \
            pthread_cleanup_push(diminuto_reader_cleanup, diminuto_reader_rwp)

#define DIMINUTO_READER_END \
            pthread_cleanup_pop(!0); \
        } \
    } while (0)

#define DIMINUTO_WRITER_BEGIN(_RWP_) \
    do { \
        extern int diminuto_writer_begin(diminuto_readerwriter_t * rwp); \
        extern void diminuto_writer_cleanup(void * vp); \
        diminuto_readerwriter_t * diminuto_writer_rwp = (diminuto_readerwriter_t *)0; \
        diminuto_writer_rwp = (_RWP_); \
        if (diminuto_writer_begin(diminuto_writer_rwp) == 0) { \
            pthread_cleanup_push(diminuto_writer_cleanup, diminuto_writer_rwp)

#define DIMINUTO_WRITER_END \
            pthread_cleanup_pop(!0); \
        } \
    } while (0)

#endif

/*******************************************************************************
 * DEBUGGING
 ******************************************************************************/

extern FILE * diminuto_readerwriter_debug(diminuto_readerwriter_t * rwp, FILE * fp);
