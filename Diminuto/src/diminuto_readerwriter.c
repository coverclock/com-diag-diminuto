/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2020 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is the implementation of the Reader Writer feature.
 */

#include "com/diag/diminuto/diminuto_readerwriter.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_assert.h"

/*******************************************************************************
 * CONSTANTS
 ******************************************************************************/

/**
 * Reader Writer calling threads may assume the following "roles". These role
 * values are used as parameter to the Reader Writer scheduling functions
 * (below), or as tokens inserted and removed from the state ring buffer.
 * "Pending" means the thread has been awakened from the condition variable but
 * has not yet removed its token from the ring buffer.
 */
typedef enum Role {
    NONE    = '-',  /**< No role. */
    READER  = 'R',  /**< Waiting reader role. */
    READING = 'r',  /**< Pending reader role. */
    WRITER  = 'W',  /**< Waiting writer role. */
    WRITING = 'w',  /**< Pending writer role. */
    ANY     = '*',  /**< Any role. */
} role_t;

/*******************************************************************************
 * STRUCTORS
 ******************************************************************************/

diminuto_readerwriter_t * diminuto_readerwriter_init(diminuto_readerwriter_t * rwp, diminuto_readerwriter_state_t * state, size_t capacity)
{
    diminuto_readerwriter_t * result = (diminuto_readerwriter_t *)0;
    int rc = DIMINUTO_READERWRITER_ERROR;

    if ((rc = pthread_mutex_init(&(rwp->mutex), (pthread_mutexattr_t *)0)) != 0) {
        errno = rc;
        diminuto_perror("diminuto_readerwriter_init: pthread_mutex_init");
    } else if ((rc = pthread_cond_init(&(rwp->reader), (pthread_condattr_t *)0)) != 0) {
        errno = rc;
        diminuto_perror("diminution_readerwriter_init: pthread_cond_init: reader");
    } else if ((rc = pthread_cond_init(&(rwp->writer), (pthread_condattr_t *)0)) != 0) {
        errno = rc;
        diminuto_perror("diminution_readerwriter_init: pthread_cond_init: writer");
    } else if (diminuto_ring_init(&(rwp->ring), capacity) != &(rwp->ring)) {
        /* Failed! */
    } else {
        rwp->state = state;
        rwp->fp = (FILE *)0;
        rwp->reading = 0;
        rwp->writing = 0;
        result = rwp;
    }

    return result;

}

diminuto_readerwriter_t * diminuto_readerwriter_fini(diminuto_readerwriter_t * rwp)
{
    diminuto_readerwriter_t * result = rwp;
    int rc = DIMINUTO_READERWRITER_ERROR;

    if (diminuto_ring_fini(&(rwp->ring)) != (diminuto_ring_t *)0) {
        /* Failed! */
    } else if ((rc = pthread_cond_destroy(&(rwp->writer))) != 0) {
        errno = rc;
        diminuto_perror("diminuto_condition_fini: pthread_cond_destroy: writer");
    } else if ((rc = pthread_cond_destroy(&(rwp->reader))) != 0) {
        errno = rc;
        diminuto_perror("diminuto_condition_fini: pthread_cond_destroy: reader");
    } else if ((rc = pthread_mutex_destroy(&(rwp->mutex))) != 0) {
        errno = rc;
        diminuto_perror("diminuto_mutex_fini: pthread_mutex_destroy");
    } else {
        result = (diminuto_readerwriter_t *)0;
    }

    return result;
}

/*******************************************************************************
 * HELPERS
 ******************************************************************************/

/**
 * Display information about the internal state of the Reader Writer object
 * to a file stream (like stderr).
 * @param fp points to the file stream.
 * @param rwp points to the Reader Writer object.
 * @param label is a string displayed to identify who is calling the function.
 */
static void dump(FILE * fp, diminuto_readerwriter_t * rwp, const char * label)
{
    unsigned int used = 0;
    unsigned int count = 0;
    unsigned int pending = 0;
    unsigned int readers = 0;
    unsigned int writers = 0;
    int index = -1;

    fprintf(fp, "%s(%p):", label, rwp);
    fprintf(fp, " %dreading", rwp->reading);
    fprintf(fp, " %dwriting", rwp->writing);
    fprintf(fp, " %dwaiting", used = diminuto_ring_used(&(rwp->ring)));
    if (used > 0) {
        index = diminuto_ring_consumer_peek(&(rwp->ring));
        for (count = 0; count < used; ++count) {
            switch (rwp->state[index]) {
            case READING: ++pending; break;
            case WRITING: ++pending; break;
            case READER:  ++readers; break;
            case WRITER:  ++writers; break;
            default:                 break;
            }
            index = diminuto_ring_next(&(rwp->ring), index);
        }
    }
    fprintf(fp, " %dpending", pending);
    fprintf(fp, " %dreaders", readers);
    fprintf(fp, " %dwriters", writers);
    if (used > 0) {
        index = diminuto_ring_consumer_peek(&(rwp->ring));
        for (count = 0; count < used; ++count) {
            fprintf(fp, " [%d]{%c}", index, rwp->state[index]);
            index = diminuto_ring_next(&(rwp->ring), index);
        }
    }
    fputc('\n', fp);
    fflush(fp);
}

/*******************************************************************************
 * SCHEDULING
 ******************************************************************************/

/**
 * Place the calling thread in a wait on the condition variable in the
 * Reader Writer object identified by the specified role. The role token
 * is inserted into the tail of the state ring buffer before the thread
 * waits. When the thread awakens, it will remove its token (which will have
 * been changed from a "waiting" token to a "pending" token) from the head
 * of the state ring buffer. The index of the token of the thread in the state
 * ring buffer is passed back when the thread awakens purely for purposes of
 * debugging.
 * @param rwp points to the Reader Writer object.
 * @param role is the role of the calling thread: READER or WRITER.
 * @param indexp is a value-result parameter into which the index is placed.
 * @return 0 for success, or an errno number otherwise.
 */
static int suspend(diminuto_readerwriter_t * rwp, role_t role, int * indexp)
{
    int rc = DIMINUTO_READERWRITER_ERROR;
    int index = -1;

    if ((index = diminuto_ring_producer_request(&(rwp->ring), 1)) < 0) {

        /*
         * Botch! There isn't enough room in the ring that the application
         * provided.
         */

        rc = DIMINUTO_READERWRITER_FULL;
        errno = rc;
        diminuto_perror("suspend: producer");

    } else if (role == READER) {

        /*
         * Insert this reader onto the tail of the ring.
         */

        rwp->state[index] = READER;
        DIMINUTO_LOG_DEBUG("Reader %d WAITING %dreading %dwriting %dwaiting", index, rwp->reading, rwp->writing, diminuto_ring_used(&(rwp->ring)));
        *indexp = index; /* Just for logging. */

        /*
         * Wait until this reader is signaled, at the head of the ring, and
         * specifically selected. Note that POSIX doesn't guarantee FIFO
         * behavior on the part of condition variables.
         */

        do {
            if ((rc = pthread_cond_wait(&(rwp->reader), &(rwp->mutex))) != 0) {
                errno = rc;
                diminuto_perror("suspend: pthread_cond_wait");
                if (diminuto_ring_producer_revoke(&(rwp->ring), 1) < 0) {
                    rc = DIMINUTO_READERWRITER_UNEXPECTED;
                    errno = rc;
                    diminuto_perror("suspend: revoke");
                }
                break;
            }
        } while ((diminuto_ring_consumer_peek(&(rwp->ring)) != index) || (rwp->state[index] != READING));

        /*
         * Consume this position and let the next waiter become the head
         * of the ring.
         */

        if (rc != 0) {
            /* Failed! */
        } else if (diminuto_ring_consumer_request(&(rwp->ring), 1) != index) {
            rc = DIMINUTO_READERWRITER_UNEXPECTED;
            errno = rc;
            diminuto_perror("suspend: consumer");
        } else {
            DIMINUTO_LOG_DEBUG("Reader %d RUNNING %dreading %dwriting %dwaiting", index, rwp->reading, rwp->writing, diminuto_ring_used(&(rwp->ring)));
        }

    } else if (role == WRITER) {

        /*
         * Insert this writer onto the tail of the ring.
         */

        rwp->state[index] = WRITER;
        DIMINUTO_LOG_DEBUG("Writer %d WAITING %dreading %dwriting %dwaiting", index, rwp->reading, rwp->writing, diminuto_ring_used(&(rwp->ring)));
        *indexp = index; /* Just for logging. */

        /*
         * Wait until this writer is signaled, at the head of the ring, and
         * specifically selected. Note that POSIX doesn't guarantee FIFO
         * behavior on the part of condition variables.
         */

        do {
            if ((rc = pthread_cond_wait(&(rwp->writer), &(rwp->mutex))) != 0) {
                errno = rc;
                diminuto_perror("suspend: pthread_cond_wait");
                if (diminuto_ring_producer_revoke(&(rwp->ring), 1) < 0) {
                    rc = DIMINUTO_READERWRITER_UNEXPECTED;
                    errno = rc;
                    diminuto_perror("suspend: revoke");
                }
                break;
            }
        } while ((diminuto_ring_consumer_peek(&(rwp->ring)) != index) || (rwp->state[index] != WRITING));

        /*
         * Consume this position and let the next waiter become the head
         * of the ring.
         */

        if (rc != 0) {
            /* Failed! */
        } else if (diminuto_ring_consumer_request(&(rwp->ring), 1) != index) {
            rc = DIMINUTO_READERWRITER_UNEXPECTED;
            errno = rc;
            diminuto_perror("suspend: consumer");
        } else {
            DIMINUTO_LOG_DEBUG("Writer %d RUNNING %dreading %dwriting %dwaiting", index, rwp->reading, rwp->writing, diminuto_ring_used(&(rwp->ring)));
        }

    } else {

        diminuto_assert((role == READER) || (role == WRITER));

    }

    return rc;
}

/**
 * Signal a waiting thread whose state token at the head of the state
 * ring buffer (if anyway)  matches that of the specified role, or any
 * waiting thread if the role is ANY. Prior to signaling the waiting
 * thread, the state token is changed from a waiting token to a pending
 * token.
 * @param rwp points to the Reader Writer object.
 * @param role is the required role: READER, WRITER, or ANY.
 * @return the role of the thread signaled: READER or WRITER.
 */
static role_t resume(diminuto_readerwriter_t * rwp, role_t role)
{
    role_t result = NONE;
    int rc = DIMINUTO_READERWRITER_ERROR;
    int index = -1;

    if ((index = diminuto_ring_consumer_peek(&(rwp->ring))) < 0) {

        /*
         * There are no one waiting. We're done.
         */

    } else if ((rwp->state[index] == READER) && ((role == READER) || (role == ANY))) {

        /*
         * The next waiter is a reader. Signal it. It will resume
         * the next waiter if it is also a reader. We do a broadcast
         * because POSIX doesn't guarantee FIFO behavior on the part
         * of condition variables. The signaled reader only resumes
         * if it is at the head of the ring and it was placed into the
         * pending state.
         */

        rwp->state[index] = READING;

        if ((rc = pthread_cond_broadcast(&(rwp->reader))) != 0) {
            errno = rc;
            diminuto_perror("resume: pthread_cond_broadcast: reader");
        } else {
            DIMINUTO_LOG_DEBUG("Reader %d SIGNALED %dreading %dwriting %dwaiting", index, rwp->reading, rwp->writing, diminuto_ring_used(&(rwp->ring)));
            result = READER;
        }

    } else if ((rwp->state[index] == WRITER) && ((role == WRITER) || (role == ANY))) {

        /*
         * The next waiter is a writer. Signal it. When it is done
         * writing it will resume the next head of the ring,
         * whatever it is. We do a broadcast because POSIX doesn't
         * guarantee FIFO behavior on the part of condition variables.
         * The signaled writer only resumes if it is at the head
         * of the ring and it was placed into the pending state.
         */

        rwp->state[index] = WRITING;

        if ((rc = pthread_cond_broadcast(&(rwp->writer))) != 0) {
            errno = rc;
            diminuto_perror("resume: pthread_cond_broadcast: writer");
        } else {
            DIMINUTO_LOG_DEBUG("Writer %d SIGNALED %dreading %dwriting %dwaiting", index, rwp->reading, rwp->writing, diminuto_ring_used(&(rwp->ring)));
            result = WRITER;
        }

    } else {

        diminuto_assert((role == READER) || (role == WRITER) || (role == ANY));

    }

    return result;
}

/*******************************************************************************
 * GENERATORS
 ******************************************************************************/

/**
 * Clean up the mutex if an abnormal termination occurrs.
 * @param vp points to the Reader Writer object.
 */
static void mutex_cleanup(void * vp)
{
    diminuto_readerwriter_t * rwp = (diminuto_readerwriter_t *)vp;
    int rc = DIMINUTO_READERWRITER_ERROR;

    if ((rc = pthread_mutex_unlock(&(rwp->mutex))) != 0) {
        errno = rc;
        diminuto_perror("mutex_cleanup: pthread_mutex_unlock");
    }
}

/**
 * @def BEGIN_CRITICAL_SECTION
 * This is the opening bracket of a critical section using the mutex
 * in the Reader Writer object pointed to by @a _RWP_.
 */
#define BEGIN_CRITICAL_SECTION(_RWP_) \
    do { \
        diminuto_readerwriter_t * critical_section_rwp = (diminuto_readerwriter_t *)0; \
        critical_section_rwp = (_RWP_); \
        if ((errno = pthread_mutex_lock(&(critical_section_rwp->mutex))) != 0) { \
            diminuto_perror("BEGIN_CRITICAL_SECTION: pthread_mutex_lock"); \
        } else { \
            pthread_cleanup_push(mutex_cleanup, critical_section_rwp)

/**
 * @def END_CRITICAL_SECTION
 * This is the closing bracket of a critical section using the mutex
 * in the Reader Writer object pointed to by @a _RWP_.
 */
#define END_CRITICAL_SECTION \
            pthread_cleanup_pop(!0); \
            critical_section_rwp = (diminuto_readerwriter_t *)0; \
        } \
    } while (0)

/*******************************************************************************
 * POLICY
 ******************************************************************************/

/**
 * This function is called to begin a Reader segment of code.
 * @param rwp points to the Reader Writer object.
 * @return 0 for success, <0 otherwise.
 */
int diminuto_reader_begin(diminuto_readerwriter_t * rwp)
{
    int result = -1;
    int index = -1;
    role_t role = NONE;

    BEGIN_CRITICAL_SECTION(rwp);

        DIMINUTO_LOG_DEBUG("Reader - BEGIN enter %dreading %dwriting %dwaiting", rwp->reading, rwp->writing, diminuto_ring_used(&(rwp->ring)));

        if ((rwp->writing <= 0) && (diminuto_ring_used(&(rwp->ring)) <= 0)) {
            /*
             * There are zero or more active writers and no one is waiting.
             * Reader can proceeed. INCREMENT!
             */
            rwp->reading += 1;
            result = 0;
        } else if (suspend(rwp, READER, &index) == 0) {
            /*
             * Either there was an active writer or someone is waiting.
             * If someone is waiting, it is presumably a writer, since
             * a reader would not have waited. The reader count has
             * already been incremented by the thread that resumed
             * us.
             */
            result = 0;
        } else {
            /* Failed! */
        }

        /*
         * If we are running, a reader at the head of the ring can
         * run as well. When such a reader runs, it will also check
         * for another reader at the head of the ring.
         */

        if (result < 0) {
            /* Do nothing */
        } else if ((role = resume(rwp, READER)) == NONE) {
            /*
             * There isn't a reader at the head of the ring to resume.
             */
        } else if (role == READER) {
            /*
             * There's another reader. It will in turn resume the following
             * reader if there is one. This is where a formerly suspended
             * and now resumed reader may get its INCREMENT!
             */
            rwp->reading += 1;
        } else {
            /* Do nothing. */
        }

        /*
         * Is is important for fairness and correctness (and fairly subtle
         * IMO) that the reader count already be incremented for any waiting
         * Reader that we resumed before we exit the critical section. Any
         * Reader that we resume will itself resume a Reader if it follows
         * that Reader in the state ring buffer.
         */

        if (index < 0) {
            DIMINUTO_LOG_DEBUG("Reader - BEGIN exit %dreading %dwriting %dwaiting", rwp->reading, rwp->writing, diminuto_ring_used(&(rwp->ring)));
        } else {
            DIMINUTO_LOG_DEBUG("Reader %d BEGIN exit %dreading %dwriting %dwaiting", index, rwp->reading, rwp->writing, diminuto_ring_used(&(rwp->ring)));
        }

        if (rwp->fp != (FILE *)0) {
            dump(rwp->fp, rwp, "diminuto_reader_begin");
        }

        diminuto_assert((rwp->reading > 0) && (rwp->writing == 0));

    END_CRITICAL_SECTION;

    return result;
}

/**
 * This function is called to end a Reader segment of code.
 * @param rwp points to the Reader Writer object.
 * @return 0 for success, <0 otherwise.
 */
int diminuto_reader_end(diminuto_readerwriter_t * rwp)
{
    int result = -1;
    role_t role = NONE;

    BEGIN_CRITICAL_SECTION(rwp);

        DIMINUTO_LOG_DEBUG("Reader - END enter %dreading %dwriting %dwaiting", rwp->reading, rwp->writing, diminuto_ring_used(&(rwp->ring)));

        diminuto_assert(rwp->reading > 0);

        /*
         * DECREMENT!
         */

        rwp->reading -= 1;

        /*
         * Try to schedule the head of the ring to run.
         */

        if (rwp->reading > 0) {
            /*
             * There are still readers running.
             */
            result = 0;
        } else if ((role = resume(rwp, ANY)) == NONE) {
            /*
             * This was the last reader, and there are no other readers
             * or writers waiting.
             */
            result = 0;
        } else if (role == READER) {
            /*
             * This shouldn't happen: a reader should never have waited.
             * We'll handle it anyway, but whine about it. This is where
             * a formerly suspended and now resumed reader may get its
             * INCREMENT!
             */
            errno = DIMINUTO_READERWRITER_UNEXPECTED;
            diminuto_perror("diminuto_reader_end: reader");
            rwp->reading += 1;
            result = 0;
        } else if (role == WRITER) {
            /*
             * We're resuming a writer who has been waiting. This is
             * where a formerly suspended and now resumed writer may get
             * its INCREMENT!
             */
            rwp->writing += 1;
            result = 0;
        } else {
            /* Do nothing. */
        }

        /*
         * Is is important for fairness and correctness (and fairly subtle IMO)
         * that the reader or writer count already be incremented for any waiting
         * Reader or Writer that we resumed before we exit the critical section.
         */

        DIMINUTO_LOG_DEBUG("Reader - END exit %dreading %dwriting %dwaiting", rwp->reading, rwp->writing, diminuto_ring_used(&(rwp->ring)));

        if (rwp->fp != (FILE *)0) {
            dump(rwp->fp, rwp, "diminuto_reader_end");
        }

        diminuto_assert(((rwp->reading >= 0) && (rwp->writing == 0)) || ((rwp->reading == 0) && (rwp->writing == 1)));

    END_CRITICAL_SECTION;

    return result;
}

/**
 * This function is called to begin a Writer segment of code.
 * @param rwp points to the Reader Writer object.
 * @return 0 for success, <0 otherwise.
 */
int diminuto_writer_begin(diminuto_readerwriter_t * rwp)
{
    int result = -1;
    int index = -1;

    BEGIN_CRITICAL_SECTION(rwp);

        DIMINUTO_LOG_DEBUG("Writer - BEGIN enter %dreading %dwriting %dwaiting", rwp->reading, rwp->writing, diminuto_ring_used(&(rwp->ring)));

        if ((rwp->reading <= 0) && (rwp->writing <= 0) && (diminuto_ring_used(&(rwp->ring)) <= 0)) {
            /*
             * There are no active readers or writers and no one waiting.
             * Writer can proceed. INCREMENT!
             */
            rwp->writing += 1;
            result = 0;
        } else if (suspend(rwp, WRITER, &index) == 0) {
            /*
             * Either there was at least active readers, an active
             * writer, or someone is waiting. (A waiter could be a thread
             * that has been resumed but has not run yet to remove itself
             * from the ring.) The writer count has already been incremented
             * by the thread that resumed us.
             */
            result = 0;
        } else {
            /* Failed! */
        }

        if (index < 0) {
            DIMINUTO_LOG_DEBUG("Writer - BEGIN exit %dreading %dwriting %dwaiting", rwp->reading, rwp->writing, diminuto_ring_used(&(rwp->ring)));
        } else {
            DIMINUTO_LOG_DEBUG("Writer %d BEGIN exit %dreading %dwriting %dwaiting", index, rwp->reading, rwp->writing, diminuto_ring_used(&(rwp->ring)));
        }

        if (rwp->fp != (FILE *)0) {
            dump(rwp->fp, rwp, "diminuto_writer_begin");
        }

        diminuto_assert((rwp->reading == 0) && (rwp->writing == 1));

    END_CRITICAL_SECTION;

    return result;
}

/**
 * This function is called to end a Writer segment of code.
 * @param rwp points to the Reader Writer object.
 * @return 0 for success, <0 otherwise.
 */
int diminuto_writer_end(diminuto_readerwriter_t * rwp)
{
    int result = -1;
    role_t role = NONE;

    BEGIN_CRITICAL_SECTION(rwp);

        DIMINUTO_LOG_DEBUG("Writer - END enter %dreading %dwriting %dwaiting", rwp->reading, rwp->writing, diminuto_ring_used(&(rwp->ring)));

        diminuto_assert(rwp->writing == 1);

        /*
         * DECREMENT!
         */

        rwp->writing -= 1;

        /*
         * There should be no readers or writers running.
         * Try to schedule the head of the ring to run.
         */

        if ((role = resume(rwp, ANY)) == NONE) {
            /*
             * There are no other readers or writers waiting.
             */
            result = 0;
        } else if (role == READER) {
            /*
             * We're activating a reader who has been waiting. This is
             * where a formerly suspended and now resumed reader may
             * gets its INCREMENT!
             */
            rwp->reading += 1;
            result = 0;
        } else if (role == WRITER) {
            /*
             * We're activating a writer who has been waiting. This is
             * where a formerly suspended and now resumed reader may
             * get its INCREMENT!
             */
            rwp->writing += 1;
            result = 0;
        } else {
            /* Do nothing. */
        }

        /*
         * Is is important for fairness and correctness (and fairly subtle IMO)
         * that the reader or writer count already be incremented for any waiting
         * Reader or Writer that we resumed before we exit the critical section.
         */

        DIMINUTO_LOG_DEBUG("Writer - END exit %dreading %dwriting %dwaiting", rwp->reading, rwp->writing, diminuto_ring_used(&(rwp->ring)));

        if (rwp->fp != (FILE *)0) {
            dump(rwp->fp, rwp, "diminuto_writer_end");
        }

        diminuto_assert(((rwp->reading >= 0) && (rwp->writing == 0)) || ((rwp->reading == 0) && (rwp->writing == 1)));

    END_CRITICAL_SECTION;

    return result;
}

/*******************************************************************************
 * CALLBACKS
 ******************************************************************************/

/**
 * Cleanup a Reader segment of code by calling the end reader function.
 * @param vp points to the Reader Writer object.
 */
void diminuto_reader_cleanup(void * vp)
{
    diminuto_readerwriter_t * rwp = (diminuto_readerwriter_t *)vp;

    (void)diminuto_reader_end(rwp);
}

/**
 * Cleanup a Writer segment of code by calling the end writer function.
 * @param vp points to the Reader Writer object.
 */
void diminuto_writer_cleanup(void * vp)
{
    diminuto_readerwriter_t * rwp = (diminuto_readerwriter_t *)vp;

    (void)diminuto_writer_end(rwp);
}

/*******************************************************************************
 * DEBUGGING
 ******************************************************************************/

FILE * diminuto_readerwriter_debug(diminuto_readerwriter_t * rwp, FILE * fp)
{
    FILE * oldfp = (FILE *)0;

    BEGIN_CRITICAL_SECTION(rwp);

        oldfp = rwp->fp;
        rwp->fp = fp;

    END_CRITICAL_SECTION;

    return oldfp;
}
