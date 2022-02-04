/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_READERWRITER_
#define _H_COM_DIAG_DIMINUTO_READERWRITER_

/**
 * @file
 * @copyright Copyright 2020-2022 Digital Aggregates Corporation, Colorado, USA.
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
 * The public API supports expedited requests by allowing the calling
 * thread to be queued at the head of the list instead of at the tail.
 *
 * The public API supports timeouts (using a relative duration,
 * NOT an absolute clocktime). Timeouts are not supported when using the
 * macros instead of the functions. This is in part because supporting
 * timeouts in the functions allowed me to test certain failure scenarios,
 * but I can't really come up with a compelling use case for their more
 * general application.
 *
 * Although I've made some effort to make this work correctly in the face
 * of thread cancellation, I haven't tested that specific scenario - and I
 * probably won't. I'm not really a believer in cancellation in general, and
 * asynchronous cancellation terrifies me (it should terrify you, too).
 * This is why Diminuto threads - on which Reader Writer does NOT depend -
 * provides a its own notification mechanism.
 *
 * REFERENCES
 *
 * P. Courtois, F. Heymans, D. Parnas, "Concurrent Control with ''Readers''
 * and ''Writers''", CACM, 14.10, 1971-10
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
 *
 * Wikipedia, "Readers-writer lock", 2020-11-16
 *
 * C. Overclock, "First-Come First-Served Readers and Writers in C using
 * Monitors", 2020-12
 *
 * C. Overclock, "Revisiting the First-Come First-Served Reader-Writer
 * Solution", 2022-02
 */

/*******************************************************************************
 * PREREQUISITES
 ******************************************************************************/

#include "com/diag/diminuto/diminuto_types.h"
#include "com/diag/diminuto/diminuto_widthof.h"
#include "com/diag/diminuto/diminuto_list.h"
#include <errno.h>
#include <stdint.h>
#include <pthread.h>
#include <stdio.h>

/*******************************************************************************
 * SYMBOLS
 ******************************************************************************/

/**
 * These are the error codes that Reader Writer might return itself, in
 * addition to those error codes that the underlying platform (like POSIX
 * threads) might generate. Most of these probably indicate a bug in Reader
 * Writer or in Diminuto at large.
 */
enum DiminutoReaderWriterError {
    DIMINUTO_READERWRITER_BUSY          = EBUSY,    /**< FIFO not empty. */
    DIMINUTO_READERWRITER_ERROR         = EIO,      /**< Unspecified error. */
    DIMINUTO_READERWRITER_UNEXPECTED    = EFAULT,   /**< Unexpected state. */
    DIMINUTO_READERWRITER_TIMEDOUT      = ETIMEDOUT,/**< Timed out. */
    DIMINUTO_READERWRITER_INTERRUPTED   = EINTR,    /**< Interrupted. */
};

/*******************************************************************************
 * TYPES
 ******************************************************************************/

/**
 * This is the Reader Writer object, which maintains the state of the Reader
 * Writer synchronization.
 */
typedef struct DiminutoReaderWriter {
    pthread_mutex_t mutex;                      /**< Mutex semaphore. */
    pthread_cond_t reader;                      /**< Waiting readers. */
    pthread_cond_t writer;                      /**< Waiting writers. */
    diminuto_list_t list;                       /**< FIFO of waiting threads. */
    int debugging;                              /**< Debug flag. */
    int reading;                                /**< Active (>=0) readers. */
    int writing;                                /**< Active {0,1} writers. */
    int waiting;                                /**< Waiting (>=0) threads. */
} diminuto_readerwriter_t;

/**
 * @def DIMINUTO_READERWRITER_INITIALIZER
 * This can be used for static initialization of a Reader Writer object.
 * If a Reader Writer object is initialized statically, further
 * initialization (for example, of the list object) will be done at
 * run time the first time it is used.
 */
#define DIMINUTO_READERWRITER_INITIALIZER \
    { \
        PTHREAD_MUTEX_INITIALIZER, \
        PTHREAD_COND_INITIALIZER, \
        PTHREAD_COND_INITIALIZER, \
        DIMINUTO_LIST_INITIALIZER, \
        0, \
        0, \
        0, \
        0, \
    }

/***********************************************************************
 * CONSTANTS
 **********************************************************************/

/**
 * This value when used as a timeout specifies that the caller polls and
 * does not wait.
 */
static const diminuto_ticks_t DIMINUTO_READERWRITER_POLL = 0;

/**
 * This value when used as a timeout specifies that the caller waits
 * indefinitely.
 */
static const diminuto_ticks_t DIMINUTO_READERWRITER_INFINITY = (~(diminuto_ticks_t)0);

/*******************************************************************************
 * STRUCTORS
 ******************************************************************************/

/**
 * This performs a run-time initialization of a Reader Writer object.
 * @param rwp points to the Reader Writer object.
 * @return a pointer to the object if successful, NULL if an error occurred.
 */
extern diminuto_readerwriter_t * diminuto_readerwriter_init(diminuto_readerwriter_t * rwp);

/**
 * This releases any dynamically allocated resources held by the Reader Writer
 * object.
 * @param rwp points to the Reader Writer object.
 * @return NULL if successful, a pointer to the object if an error occurred.
 */
extern diminuto_readerwriter_t * diminuto_readerwriter_fini(diminuto_readerwriter_t * rwp);

/*******************************************************************************
 * POLICY
 ******************************************************************************/

/**
 * This function is called to begin a Reader segment of code with a relative
 * duration timeout (NOT an absolute clocktime). A timeout of zero (POLL)
 * returns immediately, with failure if the lock was not available. A timeout
 * of the maximum value (INFINITY) causes the caller to wait indefinitely.
 * A priority argument determines whether the caller is suspended at the end
 * of the list (low priority) or at the front of the list (high priority).
 * @param rwp points to the Reader Writer object.
 * @param timeout is the timeout duration in ticks (POLL to INFINITY).
 * @param priority is true for high, false for low.
 * @return 0 for success, <0 otherwise.
 */
extern int diminuto_reader_begin_f(diminuto_readerwriter_t * rwp, diminuto_ticks_t timeout, int priority);

/**
 * This function is called to end a Reader segment of code.
 * @param rwp points to the Reader Writer object.
 * @return 0 for success, <0 otherwise.
 */
extern int diminuto_reader_end(diminuto_readerwriter_t * rwp);

/**
 * This function is called to begin a Writer segment of code with a relative
 * duration timeout (NOT an absolute clocktime). A timeout of zero (POLL)
 * returns immediately, with failure if the lock was not available. A timeout
 * of the maximum value (INFINITY) causes the caller to wait indefinitely.
 * A priority argument determines whether the caller is suspended at the end
 * of the list (low priority) or at the front of the list (high priority).
 * @param rwp points to the Reader Writer object.
 * @param timeout is the timeout duration in ticks (POLL to INFINITY).
 * @param priority is true for high, false for low.
 * @return 0 for success, <0 otherwise.
 */
extern int diminuto_writer_begin_f(diminuto_readerwriter_t * rwp, diminuto_ticks_t timeout, int priority);

/**
 * This function is called to end a Writer segment of code.
 * @param rwp points to the Reader Writer object.
 * @return 0 for success, <0 otherwise.
 */
extern int diminuto_writer_end(diminuto_readerwriter_t * rwp);

/*******************************************************************************
 * SPECIALIZATIONS
 ******************************************************************************/

/**
 * This function is called to begin a Reader segment of code with a relative
 * duration timeout (NOT an absolute clocktime). A timeout of zero (POLL)
 * returns immediately, with failure if the lock was not available. A timeout
 * of the maximum value (INFINITY) causes the caller to wait indefinitely.
 * @param rwp points to the Reader Writer object.
 * @param timeout is the timeout duration in ticks (POLL to INFINITY).
 * @return 0 for success, <0 otherwise.
 */
static inline int diminuto_reader_begin_timed(diminuto_readerwriter_t * rwp, diminuto_ticks_t timeout) {
    return diminuto_reader_begin_f(rwp, timeout, 0);
}

/**
 * This function is called to begin a Reader segment of code. If the reader
 * has to wait, it does so at the beginning of the list.
 * @param rwp points to the Reader Writer object.
 * @return 0 for success, <0 otherwise.
 */
static inline int diminuto_reader_begin_priority(diminuto_readerwriter_t * rwp) {
    return diminuto_reader_begin_f(rwp, DIMINUTO_READERWRITER_INFINITY, !0);
}

/**
 * This function is called to begin a Reader segment of code.
 * @param rwp points to the Reader Writer object.
 * @return 0 for success, <0 otherwise.
 */
static inline int diminuto_reader_begin(diminuto_readerwriter_t * rwp) {
    return diminuto_reader_begin_f(rwp, DIMINUTO_READERWRITER_INFINITY, 0);
}

/**
 * This function is called to begin a Writer segment of code with a relative
 * duration timeout (NOT an absolute clocktime). A timeout of zero (POLL)
 * returns immediately, with failure if the lock was not available. A timeout
 * of the maximum value (INFINITY) causes the caller to wait indefinitely.
 * @param rwp points to the Reader Writer object.
 * @param timeout is the timeout duration in ticks (POLL to INFINITY).
 * @return 0 for success, <0 otherwise.
 */
static inline int diminuto_writer_begin_timed(diminuto_readerwriter_t * rwp, diminuto_ticks_t timeout) {
    return diminuto_writer_begin_f(rwp, timeout, 0);
}

/**
 * This function is called to begin a Writer segment of code. If the writer
 * has to wait, it does so at the beginning of the list.
 * @param rwp points to the Reader Writer object.
 * @return 0 for success, <0 otherwise.
 */
static inline int diminuto_writer_begin_priority(diminuto_readerwriter_t * rwp) {
    return diminuto_writer_begin_f(rwp, DIMINUTO_READERWRITER_INFINITY, !0);
}

/**
 * This function is called to begin a Writer segment of code.
 * @param rwp points to the Reader Writer object.
 * @return 0 for success, <0 otherwise.
 */
static inline int diminuto_writer_begin(diminuto_readerwriter_t * rwp) {
    return diminuto_writer_begin_f(rwp, DIMINUTO_READERWRITER_INFINITY, 0);
}

/*******************************************************************************
 * CALLBACKS
 ******************************************************************************/

/**
 * Cleanup a Reader segment of code by calling the end reader function.
 * @param vp points to the Reader Writer object.
 */
extern void diminuto_reader_cleanup(void * vp);

/**
 * Cleanup a Writer segment of code by calling the end writer function.
 * @param vp points to the Reader Writer object.
 */
extern void diminuto_writer_cleanup(void * vp);

/*******************************************************************************
 * GENERATORS
 ******************************************************************************/

/**
 * @def DIMINUTO_READER_BEGIN
 * This is the opening bracket of a Reader section of code. The calling thread
 * will wait if necessary until the resource is available. Once access to the
 * resource is granted, the calling thread will signal (broadcast) other reader
 * threads that are waiting behind it. @a _RWP_ points to an initialized
 * Reader Writer object; the argument is dereferenced once and only once.
 */
#define DIMINUTO_READER_BEGIN(_RWP_) \
    do { \
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
 * will wait if necessary until the resource is available. @a _RWP_ points to
 * an initialized Reader Writer object; the argument is dereferenced once and
 * only once.
 */
#define DIMINUTO_WRITER_BEGIN(_RWP_) \
    do { \
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

/*******************************************************************************
 * DEBUGGING
 ******************************************************************************/

/**
 * This function enables (or disables) additional diagnostic information to be
 * emitted to the log at DEBUG level for the specified Reader Writer object.
 * The generation and emission of the debug information is done within the
 * critical section of the object's mutex during execution of the API. This
 * also audits the object; failure of the audit can cause an assertion
 * exception.
 * @param rwp points to the Reader Writer object.
 * @param debug is true to enable debugging, false otherwise.
 * @return the prior value of the debugging flag.
 */
extern int diminuto_readerwriter_debug(diminuto_readerwriter_t * rwp, int debugging);

#endif
