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
 *
 * This translation unit contains Diminuto assert statements that check
 * invariants upon exit from the Reader and Writer Begin and End functions.
 * As long as the implementation does the right thing, these are benign.
 * Should the invariants be violated, the default behavior is for the
 * asserts to abort the application and produce a core dump. This behavior
 * can be overridden by compile time options - see the diminuto_assert.h
 * header file for more details. But the asserts failing indicates a very
 * serious bug in my code, implying the reader-writer synchronization is
 * unreliable.
 */

#include "com/diag/diminuto/diminuto_readerwriter.h"
#include "com/diag/diminuto/diminuto_assert.h"
#include "com/diag/diminuto/diminuto_criticalsection.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_time.h"
#include "com/diag/diminuto/diminuto_frequency.h"

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
    ANY     = '*',  /**< Any role. */
    NONE    = '-',  /**< No role. */
    READER  = 'R',  /**< Waiting reader role. */
    WRITER  = 'W',  /**< Waiting writer role. */
    IGNORE  = 'X',  /**< Cancelled, timed out, failed, etc. */
    READING = 'r',  /**< Pending reader role. */
    WRITING = 'w',  /**< Pending writer role. */
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
        /* Error message already emitted. */
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
        /* Error message already emitted. */
    } else if ((rc = pthread_cond_destroy(&(rwp->writer))) != 0) {
        errno = rc;
        diminuto_perror("diminuto_readerwriter_fini: pthread_cond_destroy: writer");
    } else if ((rc = pthread_cond_destroy(&(rwp->reader))) != 0) {
        errno = rc;
        diminuto_perror("diminuto_readerwriter_fini: pthread_cond_destroy: reader");
    } else if ((rc = pthread_mutex_destroy(&(rwp->mutex))) != 0) {
        errno = rc;
        diminuto_perror("diminuto_readerwriter_fini: pthread_mutex_destroy");
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
    extern pthread_mutex_t diminuto_log_mutex;
    char buffer[DIMINUTO_LOG_BUFFER_MAXIMUM];
    char * bp = (char *)0;
    size_t size = 0;
    int length = 0;
    unsigned int pending = 0;
    unsigned int readers = 0;
    unsigned int writers = 0;
    unsigned int ignored = 0;
    unsigned int unknown = 0;
    int count = 0;
    int index = -1;

    bp = &(buffer[0]);
    size = sizeof(buffer) - 1;

    length = snprintf(bp, size, "%s(%p):", label, rwp);
    if ((0 < length) && (length < size)) { bp += length; size -= length; }

    length = snprintf(bp, size, " %dreading", rwp->reading);
    if ((0 < length) && (length < size)) { bp += length; size -= length; }

    length = snprintf(bp, size, " %dwriting", rwp->writing);
    if ((0 < length) && (length < size)) { bp += length; size -= length; }

    index = diminuto_ring_consumer_peek(&(rwp->ring));
    count = diminuto_ring_used(&(rwp->ring));
    while ((count--) > 0) {
        switch (rwp->state[index]) {
        case READING: ++pending; break;
        case WRITING: ++pending; break;
        case READER:  ++readers; break;
        case WRITER:  ++writers; break;
        case IGNORE:  ++ignored; break;
        default:      ++unknown; break;
        }
        index = diminuto_ring_next(&(rwp->ring), index);
    }

    length = snprintf(bp, size, " %dpending", pending);
    if ((0 < length) && (length < size)) { bp += length; size -= length; }

    length = snprintf(bp, size, " %dreaders", readers);
    if ((0 < length) && (length < size)) { bp += length; size -= length; }

    length = snprintf(bp, size, " %dwriters", writers);
    if ((0 < length) && (length < size)) { bp += length; size -= length; }

    length = snprintf(bp, size, " %dignored", ignored);
    if ((0 < length) && (length < size)) { bp += length; size -= length; }

    length = snprintf(bp, size, " %dunknown", unknown);
    if ((0 < length) && (length < size)) { bp += length; size -= length; }

    index = diminuto_ring_consumer_peek(&(rwp->ring));
    count = diminuto_ring_used(&(rwp->ring));
    while ((count--) > 0) {
        length = snprintf(bp, size, " [%d]{%c}", index, rwp->state[index]);
        if ((0 < length) && (length < size)) { bp += length; size -= length; }
        index = diminuto_ring_next(&(rwp->ring), index);
    }

    *(bp++) = '\n';
    *(bp) = '\0';

    DIMINUTO_CRITICAL_SECTION_BEGIN(&diminuto_log_mutex);
        fputs(buffer, fp);
        fflush(fp);
    DIMINUTO_CRITICAL_SECTION_END;
}

/*******************************************************************************
 * SCHEDULING
 ******************************************************************************/

/**
 * Return the index of the head of the ring, without consuming it,
 * after first discarding any leading ignored tokens. This can also be
 * used just to discard leading ignored tokens.
 * @param rwp points to the Reader Writer object.
 * @return the index of the head of the ring or -1 if empty.
 */
static int head(diminuto_readerwriter_t * rwp)
{
    int index = -1;

    while (!0) {
       if ((index = diminuto_ring_consumer_peek(&(rwp->ring))) < 0) {
            break;
        } else if (rwp->state[index] != IGNORE) {
            break;
        } else if (diminuto_ring_consumer_request(&(rwp->ring), 1) != index) {
            errno = DIMINUTO_READERWRITER_UNEXPECTED;
            diminuto_perror("head: consumer");
            index = -1;
            break;
        } else {
            DIMINUTO_LOG_DEBUG("Failed %d IGNORED %dreading %dwriting %dwaiting", index, rwp->reading, rwp->writing, diminuto_ring_used(&(rwp->ring)));
        }
    }

    return index;
}

/**
 * Perform either an infinite wait or a timed wait as specified. If a timed
 * wait, compute the absolute clocktime for the timed wait based on the
 * relative timeout duration. This allows the caller to specify a relative
 * timeout duration that applies no matter how many times the timed wait
 * has to be repeated after receiving a broadcast.
 * @param rwp points to the Reader Writer object.
 * @param index is the index at the tail of the ring.
 * @param pending is the pending role to be used: READING or WRITING.
 * @param conditionp points to the condition variable in the object to use.
 * @param timeout is a timeout duration in ticks.
 * @return 0 for success, or an error number otherwise.
 */
static int wait_timed(diminuto_readerwriter_t * rwp, int index, role_t pending, pthread_cond_t * conditionp, diminuto_ticks_t timeout)
{
    int rc = DIMINUTO_READERWRITER_ERROR;
    diminuto_sticks_t clocktime = 0;

    if (timeout == DIMINUTO_READERWRITER_INFINITY) {

        do {
            if ((rc = pthread_cond_wait(conditionp, &(rwp->mutex))) != 0) {
                break;
            }
        } while ((head(rwp) != index) || (rwp->state[index] != pending));

    } else if ((clocktime = diminuto_time_clock()) < 0) {

        rc = errno;

    } else {
        static const diminuto_ticks_t NANOSECONDS = 1000000000;
        struct timespec absolutetime = { 0, };

        clocktime += timeout;
        absolutetime.tv_sec = diminuto_frequency_ticks2wholeseconds(clocktime);
        absolutetime.tv_nsec = diminuto_frequency_ticks2fractionalseconds(clocktime, NANOSECONDS);

        do {
            if ((rc = pthread_cond_timedwait(conditionp, &(rwp->mutex), &absolutetime)) != 0) {
                break;
            }
        } while ((head(rwp) != index) || (rwp->state[index] != pending));

    }

    return rc;
}

/**
 * Clean up the ring if an abnormal wait occurs.
 * @param vp points to the slot representing the caller in the ring.
 */
static void wait_cleanup(void * vp)
{
    diminuto_readerwriter_state_t * sp = (diminuto_readerwriter_state_t *)vp;

    *sp = IGNORE;
}

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
 * @param label is a string used for logging.
 * @param index is the index at the tail of the ring.
 * @param waiting is the waiting role to be used: READER or WRITER.
 * @param pending is the pending role to be used: READING or WRITING.
 * @param conditionp points to the condition variable in the object to use.
 * @param timeout is a timeout duration in ticks.
 * @return 0 for success, or an error number otherwise.
 */
static int condition(diminuto_readerwriter_t * rwp, const char * label, int index, role_t waiting, role_t pending, pthread_cond_t * conditionp, diminuto_ticks_t timeout)
{
    int rc = DIMINUTO_READERWRITER_ERROR;

    /*
     * Insert the token onto the tail of the ring.
     */

    rwp->state[index] = waiting;
    DIMINUTO_LOG_DEBUG("%s %d WAITING %dreading %dwriting %dwaiting", label, index, rwp->reading, rwp->writing, diminuto_ring_used(&(rwp->ring)));

    /*
     * Wait until this thread is signaled, while at the head of the ring, and
     * specifically selected. Note that POSIX doesn't guarantee FIFO
     * behavior on the part of condition variables. Use a cleanup handler to
     * reconcile the slot in the ring if the caller is cancelled, interrupted,
     * timedout, etc.
     */

    pthread_cleanup_push(wait_cleanup, &(rwp->state[index]));

        rc = wait_timed(rwp, index, pending, conditionp, timeout);

    pthread_cleanup_pop(rc != 0);

    /*
     * Consume this token and let the next waiter become the head
     * of the ring. Normally I would suppress error messages for
     * ETIMEDOUT and EINTR because those can be normal occurrences
     * in many circumstances; but not this one.
     */

    if (rc != 0) {
        errno = rc;
        diminuto_perror("condition: pthread_cond_wait");
    } else if (diminuto_ring_consumer_request(&(rwp->ring), 1) != index) {
        rc = DIMINUTO_READERWRITER_UNEXPECTED;
        errno = rc;
        diminuto_perror("condition: consumer");
    } else {
        DIMINUTO_LOG_DEBUG("%s %d RUNNING %dreading %dwriting %dwaiting", label, index, rwp->reading, rwp->writing, diminuto_ring_used(&(rwp->ring)));
    }

    return rc;
}

/**
 * Signal a waiting thread whose state token at the head of the state
 * ring buffer (if anyway). Prior to signaling the waiting thread,
 * the state token is changed from a waiting token to a pending token.
 * @param rwp points to the Reader Writer object.
 * @param label is a string used for logging.
 * @param index is the index at the tail of the ring.
 * @param pending is the pending role to be used: READING or WRITING.
 * @param conditionp points to the condition variable in the object to use.
 * @return 0 for success, or an error number otherwise.
 */
static int broadcast(diminuto_readerwriter_t * rwp, const char * label, int index, role_t pending, pthread_cond_t * conditionp)
{
    int rc = DIMINUTO_READERWRITER_ERROR;

    rwp->state[index] = pending;

    if ((rc = pthread_cond_broadcast(conditionp)) != 0) {
        errno = rc;
        diminuto_perror("broadcast: pthread_cond_broadcast");
    } else {
        DIMINUTO_LOG_DEBUG("%s %d SIGNALED %dreading %dwriting %dwaiting", label, index, rwp->reading, rwp->writing, diminuto_ring_used(&(rwp->ring)));
    }

    return rc;
}

/*******************************************************************************
 * ROLES
 ******************************************************************************/

/**
 * Suspend the caller until the desired role is at the head of the ring.
 * @param rwp points to the Reader Writer object.
 * @param role is the role of the calling thread: READER or WRITER.
 * @param indexp is a value-result parameter into which the index is placed.
 * @param timeout is a timeout duration in ticks.
 * @return 0 for success, or an errno number otherwise.
 */
static int suspend(diminuto_readerwriter_t * rwp, role_t role, int * indexp, diminuto_ticks_t timeout)
{
    int rc = DIMINUTO_READERWRITER_ERROR;
    int index = -1;

    if ((index = diminuto_ring_producer_request(&(rwp->ring), 1)) < 0) {

        /*
         * There isn't enough room in the ring that the application provided.
         */

        rc = DIMINUTO_READERWRITER_FULL;
        errno = rc;
        diminuto_perror("suspend: producer");

    } else if (role == READER) {

        if ((rc = condition(rwp, "Reader", index, READER, READING, &(rwp->reader), timeout)) == 0) {
            *indexp = index; /* Just for logging. */
        }

    } else if (role == WRITER) {

        if ((rc = condition(rwp, "Writer", index, WRITER, WRITING, &(rwp->writer), timeout)) == 0) {
            *indexp = index; /* Just for logging. */
        }

    } else {

        /*
         * The caller passed a role that was neither READER nor WRITER.
         */

        errno = rc;
        diminuto_perror("suspend: role");

    }

    return rc;
}

/**
 * Resume the thread at the head of the ring whose token matches that
 * of the specified role, or any role if the role is ANY.
 * @param rwp points to the Reader Writer object.
 * @param role is the required role: READER, WRITER, or ANY.
 * @return the role of the thread signaled: READER or WRITER.
 */
static role_t resume(diminuto_readerwriter_t * rwp, role_t role)
{
    role_t result = NONE;
    int index = -1;

    if ((index = head(rwp)) < 0) {

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

        if (broadcast(rwp, "Reader", index, READING, &(rwp->reader)) == 0) {
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

        if (broadcast(rwp, "Writer", index, WRITING, &(rwp->writer)) == 0) {
            result = WRITER;
        }

    } else {

        /*
         * The rest of this is just error checking because I'm paranoid.
         */

        switch (role) {
        case READER:
        case WRITER:
        case ANY:
            break;
        default:
            /*
             * The caller passed a role that wasn't READER, WRITER, or ANY,
             * We emit an error message, but still pass back NONE.
             */
            errno = DIMINUTO_READERWRITER_ERROR;
            diminuto_perror("resume: role");
            break;
        }

        switch (rwp->state[index]) {
        case READER:
        case WRITER:
        case READING:
        case WRITING:
            break;
        default:
            /*
             * The state is invalid. This should be impossible.
             * We emit an error message, but still pass back NONE.
             */
            errno = DIMINUTO_READERWRITER_UNEXPECTED;
            diminuto_perror("resume: state");
            break;
        }

        /*
         * If no error detected: either the token at the head of the ring was
         * not the role the caller wanted, or it represents a thread that is
         * pending and has not yet been scheduled to run. This is a possible
         * state, and is especially likely for the multiple concurrent readers
         * scenario in which a subsequent reader has ended after a prior
         * reader has resumed the reader at the head of the ring but that
         * reader has not yet run.
         */
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

int diminuto_reader_begin_timed(diminuto_readerwriter_t * rwp, diminuto_ticks_t timeout)
{
    int result = -1;
    int index = -1;
    role_t role = NONE;

    BEGIN_CRITICAL_SECTION(rwp);

        DIMINUTO_LOG_DEBUG("Reader - BEGIN enter %dreading %dwriting %dwaiting", rwp->reading, rwp->writing, diminuto_ring_used(&(rwp->ring)));

        if ((rwp->writing <= 0) && (diminuto_ring_used(&(rwp->ring)) <= 0)) {
            /*
             * There are zero or more active writers and no one is waiting.
             * Reader can proceeed. Increment.
             */
            rwp->reading += 1;
            result = 0;
        } else if (suspend(rwp, READER, &index, timeout) == 0) {
            /*
             * Either there was an active writer or someone is waiting.
             * If someone is waiting, it is presumably a writer, since
             * a reader would not have waited. The reader count has
             * already been incremented by the thread that resumed
             * us.
             */
            result = 0;
        } else {
            /* An error message has already been emitted. */
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
             * and now resumed reader may get its increment.
             */
            rwp->reading += 1;
        } else {
            errno = DIMINUTO_READERWRITER_UNEXPECTED;
            diminuto_perror("diminuto_reader_begin: role");
            result = -1;
        }

        /*
         * Is is important for fairness and correctness (and fairly subtle
         * IMO) that the reader count already be incremented for any waiting
         * Reader that we resumed before we exit the critical section. Any
         * Reader that we resume will itself resume a Reader if it follows
         * that Reader in the state ring buffer.
         */

        if (index < 0) {
            DIMINUTO_LOG_DEBUG("Reader - BEGIN exit %dreading %dwriting %dwaiting %d", rwp->reading, rwp->writing, diminuto_ring_used(&(rwp->ring)), result);
        } else {
            DIMINUTO_LOG_DEBUG("Reader %d BEGIN exit %dreading %dwriting %dwaiting %d", index, rwp->reading, rwp->writing, diminuto_ring_used(&(rwp->ring)), result);
        }

        if (rwp->fp != (FILE *)0) {
            dump(rwp->fp, rwp, "diminuto_reader_begin");
        }

        diminuto_assert(((result == 0) && (rwp->reading > 0) && (rwp->writing == 0)) || (result < 0));

    END_CRITICAL_SECTION;

    return result;
}

int diminuto_reader_end(diminuto_readerwriter_t * rwp)
{
    int result = -1;
    role_t role = NONE;

    BEGIN_CRITICAL_SECTION(rwp);

        DIMINUTO_LOG_DEBUG("Reader - END enter %dreading %dwriting %dwaiting", rwp->reading, rwp->writing, diminuto_ring_used(&(rwp->ring)));

        diminuto_assert((rwp->reading > 0) && (rwp->writing == 0));

        /*
         * Decrement.
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
             * increment.
             */
            errno = DIMINUTO_READERWRITER_UNEXPECTED;
            diminuto_perror("diminuto_reader_end: reader");
            rwp->reading += 1;
            result = 0;
        } else if (role == WRITER) {
            /*
             * We're resuming a writer who has been waiting. This is
             * where a formerly suspended and now resumed writer may get
             * its increment.
             */
            rwp->writing += 1;
            result = 0;
        } else {
            errno = DIMINUTO_READERWRITER_UNEXPECTED;
            diminuto_perror("diminuto_reader_end: role");
        }

        /*
         * Is is important for fairness and correctness (and fairly subtle IMO)
         * that the reader or writer count already be incremented for any
         * waiting Reader or Writer that we resumed before we exit the critical
         * section.
         */

        DIMINUTO_LOG_DEBUG("Reader - END exit %dreading %dwriting %dwaiting", rwp->reading, rwp->writing, diminuto_ring_used(&(rwp->ring)));

        if (rwp->fp != (FILE *)0) {
            dump(rwp->fp, rwp, "diminuto_reader_end");
        }

        diminuto_assert(((rwp->reading > 0) && (rwp->writing == 0)) || ((rwp->reading == 0) && (rwp->writing == 1)) || ((rwp->reading == 0) && (rwp->writing == 0) && (diminuto_ring_used(&(rwp->ring)) == 0)));

    END_CRITICAL_SECTION;

    return result;
}

int diminuto_writer_begin_timed(diminuto_readerwriter_t * rwp, diminuto_ticks_t timeout)
{
    int result = -1;
    int index = -1;

    BEGIN_CRITICAL_SECTION(rwp);

        DIMINUTO_LOG_DEBUG("Writer - BEGIN enter %dreading %dwriting %dwaiting", rwp->reading, rwp->writing, diminuto_ring_used(&(rwp->ring)));

        if ((rwp->reading <= 0) && (rwp->writing <= 0) && (diminuto_ring_used(&(rwp->ring)) <= 0)) {
            /*
             * There are no active readers or writers and no one waiting.
             * Writer can proceed. Increment.
             */
            rwp->writing += 1;
            result = 0;
        } else if (suspend(rwp, WRITER, &index, timeout) == 0) {
            /*
             * Either there was at least active readers, an active
             * writer, or someone is waiting. (A waiter could be a thread
             * that has been resumed but has not run yet to remove itself
             * from the ring.) The writer count has already been incremented
             * by the thread that resumed us.
             */
            result = 0;
        } else {
            /* An error message has already been emitted. */
        }

        if (index < 0) {
            DIMINUTO_LOG_DEBUG("Writer - BEGIN exit %dreading %dwriting %dwaiting %d", rwp->reading, rwp->writing, diminuto_ring_used(&(rwp->ring)), result);
        } else {
            DIMINUTO_LOG_DEBUG("Writer %d BEGIN exit %dreading %dwriting %dwaiting %d", index, rwp->reading, rwp->writing, diminuto_ring_used(&(rwp->ring)), result);
        }

        if (rwp->fp != (FILE *)0) {
            dump(rwp->fp, rwp, "diminuto_writer_begin");
        }

        diminuto_assert(((result == 0) && (rwp->reading == 0) && (rwp->writing == 1)) || (result < 0));

    END_CRITICAL_SECTION;

    return result;
}

int diminuto_writer_end(diminuto_readerwriter_t * rwp)
{
    int result = -1;
    role_t role = NONE;

    BEGIN_CRITICAL_SECTION(rwp);

        DIMINUTO_LOG_DEBUG("Writer - END enter %dreading %dwriting %dwaiting", rwp->reading, rwp->writing, diminuto_ring_used(&(rwp->ring)));

        diminuto_assert((rwp->reading == 0) && (rwp->writing == 1));

        /*
         * Decrement.
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
             * gets its increment.
             */
            rwp->reading += 1;
            result = 0;
        } else if (role == WRITER) {
            /*
             * We're activating a writer who has been waiting. This is
             * where a formerly suspended and now resumed writer may
             * get its increment.
             */
            rwp->writing += 1;
            result = 0;
        } else {
            errno = DIMINUTO_READERWRITER_UNEXPECTED;
            diminuto_perror("diminuto_writer_end: role");
        }

        /*
         * Is is important for fairness and correctness (and fairly subtle IMO)
         * that the reader or writer count already be incremented for any
         * waiting Reader or Writer that we resumed before we exit the critical
         * section.
         */

        DIMINUTO_LOG_DEBUG("Writer - END exit %dreading %dwriting %dwaiting", rwp->reading, rwp->writing, diminuto_ring_used(&(rwp->ring)));

        if (rwp->fp != (FILE *)0) {
            dump(rwp->fp, rwp, "diminuto_writer_end");
        }

        diminuto_assert(((rwp->reading > 0) && (rwp->writing == 0)) || ((rwp->reading == 0) && (rwp->writing == 1)) || ((rwp->reading == 0) && (rwp->writing == 0) && (diminuto_ring_used(&(rwp->ring)) == 0)));

    END_CRITICAL_SECTION;

    return result;
}

/*******************************************************************************
 * CALLBACKS
 ******************************************************************************/

void diminuto_reader_cleanup(void * vp)
{
    diminuto_readerwriter_t * rwp = (diminuto_readerwriter_t *)vp;

    (void)diminuto_reader_end(rwp);
}

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
