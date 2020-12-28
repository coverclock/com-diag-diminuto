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
 * Reader Writer is an implementation of a synchronization mechanism for
 * the classic Reader-Writer (a.k.a."Readers-Writers") problem. This
 * version not only treats readers and writers fairly, preventing starvation
 * of either role, but preserves the relative order of the reader and writer
 * requests. The order is preserved independently of whether the underlying
 * implementation of condition variables implements a FIFO queue, or what the
 * priorities of the calling threads may be.
 *
 * Although I've made some effort to make this work correctly in the face
 * of thread cancellation, I haven't tested that scenario (and probably
 * won't). Diminuto Threads specifically disable cancellation, although
 * Reader Writer doesn't depend upon Diminuto Threads.
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

/**
 * These are the error codes that Reader Writer might return itself, in
 * addition to those error codes that the underlying platform (like POSIX
 * threads) might generate. The FULL error code is an application error, while
 * ERROR and UNEXPECTED are probably indicitive of bugs in Reader Writer.
 */
enum DiminutoReaderWriterError {
    DIMINUTO_READERWRITER_ERROR         = EIO,      /**< Unspecified error. */
    DIMINUTO_READERWRITER_FULL          = ENOSPC,   /**< Ring too small. */
    DIMINUTO_READERWRITER_UNEXPECTED    = EFAULT,   /**< Unexpected state. */
};

/*******************************************************************************
 * TYPES
 ******************************************************************************/

/**
 * The application is responsible for allocating an array of this type for use
 * by Reader Writer. No initilization of the array is necesary. The dimension of
 * the array should be as large as the maximum number of threads using the Reader
 * Writer object. This array is used to implement a ring (circular) buffer used
 * to managing waiting threads.
 */
typedef int8_t diminuto_readerwriter_state_t;

/**
 * This is the Reader Writer object, which maintains the state of the Reader
 * Writer synchronization.
 */
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

/**
 * @def DIMINUTO_READERWRITER_INITIALIZER
 * This can be used for static initialization of a Reader Writer object.
 * The application is responsible for allocating a state array and
 * passing a pointer to it as @a _STATE_ and its capacity (dimension)
 * as @a _CAPACITY_.
 */
#define DIMINUTO_READERWRITER_INITIALIZER(_STATE_, _CAPACITY_) \
    { \
        PTHREAD_MUTEX_INITIALIZER, \
        PTHREAD_COND_INITIALIZER, \
        PTHREAD_COND_INITIALIZER, \
        DIMINUTO_RING_INITIALIZER(_CAPACITY_), \
        &((_STATE_)[0]), \
        (FILE *)0,  \
        0, \
        0, \
    }

/*******************************************************************************
 * STRUCTORS
 ******************************************************************************/

/**
 * This performs a run-time initialization of a Reader Writer object. The
 * state array must be at least as large as the maximum number of threads
 * that will use the Reader Writer object.
 * @param rwp points to the Reader Writer object.
 * @param state points to the state array.
 * @param capacity is the dimension of the state array.
 * @return a pointer to the object if successful, NULL if an error occurred.
 */
extern diminuto_readerwriter_t * diminuto_readerwriter_init(diminuto_readerwriter_t * rwp, diminuto_readerwriter_state_t * state, size_t capacity);

/**
 * This releases any dynamically allocated resources held by the Reader Writer
 * object.
 * @param rwp points to the Reader Writer object.
 * @return NULL if successful, a pointer to the object if an error occurred.
 */
extern diminuto_readerwriter_t * diminuto_readerwriter_fini(diminuto_readerwriter_t * rwp);

/*******************************************************************************
 * GENERATORS
 ******************************************************************************/

/**
 * @def DIMINUTO_READER_BEGIN
 * This is the opening bracket of a Reader section of code. The calling thread
 * will wait if necessary until the resource is available. Once access to the
 * resource is granted, the calling thread will signal (broadcast) other reader
 * threads that are waiting behind it.
 * @a _RWP_ is a pointer to an initialized Reader Writer object.
 */
#define DIMINUTO_READER_BEGIN(_RWP_) \
    do { \
        extern int diminuto_reader_begin(diminuto_readerwriter_t * rwp); \
        extern void diminuto_reader_cleanup(void * vp); \
        diminuto_readerwriter_t * diminuto_reader_rwp = (diminuto_readerwriter_t *)0; \
        diminuto_reader_rwp = (_RWP_); \
        if (diminuto_reader_begin(diminuto_reader_rwp) == 0) { \
            pthread_cleanup_push(diminuto_reader_cleanup, diminuto_reader_rwp)

/**
 * @def DIMINUTO_READER_END
 * This is the closing bracket of a Reader section of code. The calling thread
 * will signal (broadcast) another thread that is waiting for the resource.
 */
#define DIMINUTO_READER_END \
            pthread_cleanup_pop(!0); \
        } \
    } while (0)

/**
 * @def DIMINUTO_WRITER_BEGIN
 * This is the opening bracket of a Writer section of code. The calling thread
 * will wait if necessary until the resource is available.
 * @a _RWP_ is a pointer to an initialized Reader Writer object.
 */
#define DIMINUTO_WRITER_BEGIN(_RWP_) \
    do { \
        extern int diminuto_writer_begin(diminuto_readerwriter_t * rwp); \
        extern void diminuto_writer_cleanup(void * vp); \
        diminuto_readerwriter_t * diminuto_writer_rwp = (diminuto_readerwriter_t *)0; \
        diminuto_writer_rwp = (_RWP_); \
        if (diminuto_writer_begin(diminuto_writer_rwp) == 0) { \
            pthread_cleanup_push(diminuto_writer_cleanup, diminuto_writer_rwp)

/**
 * @def DIMINUTO_WRITER_END
 * This is the closing bracket of a Writer section of code. The calling thread
 * will signal (broadcast) another thread that is waiting for the resource.
 */
#define DIMINUTO_WRITER_END \
            pthread_cleanup_pop(!0); \
        } \
    } while (0)

#endif

/*******************************************************************************
 * DEBUGGING
 ******************************************************************************/

/**
 * This function enables (or disables) additional debugging information to be
 * emitted to the specified file stream. This is specific to the Reader Writer
 * object passed as an argument.
 * @param rwp points to the Reader Writer object.
 * @param fp is the file stream, or NULL to disable the debugging output.
 * @return the prior value of the file stream used for debugging.
 */
extern FILE * diminuto_readerwriter_debug(diminuto_readerwriter_t * rwp, FILE * fp);
