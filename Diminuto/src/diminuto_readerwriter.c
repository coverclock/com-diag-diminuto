    /* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2020 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * Work In Progress!
 */

#include "com/diag/diminuto/diminuto_readerwriter.h"
#include "com/diag/diminuto/diminuto_log.h"

/*******************************************************************************
 * CONSTANTS
 ******************************************************************************/

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
 * SCHEDULING
 ******************************************************************************/

static role_t resume(diminuto_readerwriter_t * rwp, role_t role)
{
    role_t result = NONE;
    int rc = DIMINUTO_READERWRITER_ERROR;
    int index = -1;

    if ((index = diminuto_ring_consumer_peek(&(rwp->ring))) < 0) {

        /*
         * There are no readers or writers waiting. We're done.
         */

    } else if ((rwp->state[index] == READER) && ((role == READER) || (role == ANY))) {

        /*
         * The next waiter is a reader. Signal it. It will resume
         * the next waiter if it is also a reader. We do a broadcast
         * because POSIX doesn't guarantee FIFO behavior on the part
         * of condition variables. The signaled reader only resumes
         * if it is at the head of the ring.
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
         * of the ring.
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

        /* Do nothing. */

    }

    return result;
}

static int suspend(diminuto_readerwriter_t * rwp, role_t role, int * indexp)
{
    int rc = DIMINUTO_READERWRITER_ERROR;
    int index = -1;

    if ((index = diminuto_ring_producer_request(&(rwp->ring), 1)) < 0) {

        rc = DIMINUTO_READERWRITER_FULL;
        errno = rc;
        diminuto_perror("suspend: producer");

    } else if (role == READER) {

        rwp->state[index] = READER;
        DIMINUTO_LOG_DEBUG("Reader %d WAITING %dreading %dwriting %dwaiting", index, rwp->reading, rwp->writing, diminuto_ring_used(&(rwp->ring)));
        *indexp = index; /* Just for logging. */

        /*
         * Wait until this reader is signaled and at the head of the ring.
         * We have to do this because POSIX doesn't guarantee FIFO behavior
         * on the part of condition variables.
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
        } while ((diminuto_ring_consumer_peek(&(rwp->ring)) != index) && (rwp->state[index] != READING));

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

        rwp->state[index] = WRITER;
        DIMINUTO_LOG_DEBUG("Writer %d WAITING %dreading %dwriting %dwaiting", index, rwp->reading, rwp->writing, diminuto_ring_used(&(rwp->ring)));
        *indexp = index; /* Just for logging. */

        /*
         * Wait until this writer is signaled and at the head of the ring.
         * We have to do this because POSIX doesn't guarantee FIFO behavior
         * on the part of condition variables.
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
        } while ((diminuto_ring_consumer_peek(&(rwp->ring)) != index) && (rwp->state[index] != WRITING));;

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

        rc = DIMINUTO_READERWRITER_ERROR;
        errno = rc;
        diminuto_perror("suspend: role");

    }

    return rc;
}

/*******************************************************************************
 * GENERATORS
 ******************************************************************************/

static void mutex_cleanup(void * vp)
{
    diminuto_readerwriter_t * rwp = (diminuto_readerwriter_t *)vp;
    int rc = DIMINUTO_READERWRITER_ERROR;

    if ((rc = pthread_mutex_unlock(&(rwp->mutex))) != 0) {
        errno = rc;
        diminuto_perror("mutex_cleanup: pthread_mutex_unlock");
    }
}

#define BEGIN_CRITICAL_SECTION(_RWP_) \
    do { \
        int critical_section_rc = DIMINUTO_READERWRITER_ERROR; \
        if ((critical_section_rc = pthread_mutex_lock(&((_RWP_)->mutex))) != 0) { \
            errno = critical_section_rc; \
            diminuto_perror("BEGIN_CRITICAL_SECTION: pthread_mutex_lock"); \
        } else { \
            pthread_cleanup_push(mutex_cleanup, _RWP_)

#define END_CRITICAL_SECTION \
            pthread_cleanup_pop(!0); \
            critical_section_rc = 0; \
        } \
    } while (0)

void diminuto_reader_cleanup(void * vp)
{
    diminuto_readerwriter_t * rwp = (diminuto_readerwriter_t *)vp;

    BEGIN_CRITICAL_SECTION(rwp);

        rwp->reading -= 1;

    END_CRITICAL_SECTION;
}

void diminuto_writer_cleanup(void * vp)
{
    diminuto_readerwriter_t * rwp = (diminuto_readerwriter_t *)vp;

    BEGIN_CRITICAL_SECTION(rwp);

        rwp->writing -= 1;

    END_CRITICAL_SECTION;
}

/*******************************************************************************
 * POLICY
 ******************************************************************************/

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
             * Reader can proceeed.
             */
            rwp->reading += 1;
            result = 0;
        } else if (suspend(rwp, READER, &index) == 0) {
            /*
             * Either there was an active writer or someone is waiting.
             * If someone is waiting, it is presumably a writer, since
             * a reader would not have waited. The reader count has
             * already been incremented by the thread that signalled
             * us.
             */
            result = 0;
        } else {
            /* Failed! */
        }

        /*
         * If we are running, a reader waiting immediately behind us in
         * the ring can run as well.
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
             * reader if there is one.
             */
            rwp->reading += 1;
        } else {
            /* Do nothing. */
        }

        if (index < 0) {
            DIMINUTO_LOG_DEBUG("Reader - BEGIN exit %dreading %dwriting %dwaiting", rwp->reading, rwp->writing, diminuto_ring_used(&(rwp->ring)));
        } else {
            DIMINUTO_LOG_DEBUG("Reader %d BEGIN exit %dreading %dwriting %dwaiting", index, rwp->reading, rwp->writing, diminuto_ring_used(&(rwp->ring)));
        }

    END_CRITICAL_SECTION;

    return result;
}

int diminuto_reader_end(diminuto_readerwriter_t * rwp)
{
    int result = -1;
    role_t role = NONE;

    BEGIN_CRITICAL_SECTION(rwp);

        DIMINUTO_LOG_DEBUG("Reader - END enter %dreading %dwriting %dwaiting", rwp->reading, rwp->writing, diminuto_ring_used(&(rwp->ring)));

        rwp->reading -= 1;

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
             * We'll handle it anyway, but whine about it.
             */
            errno = DIMINUTO_READERWRITER_UNEXPECTED;
            diminuto_perror("diminuto_reader_end: reader");
            rwp->reading += 1;
            result = 0;
        } else if (role == WRITER) {
            /*
             * We're resuming a writer who has been waiting.
             */
            rwp->writing += 1;
            result = 0;
        } else {
            /* Do nothing. */
        }

        DIMINUTO_LOG_DEBUG("Reader - END exit %dreading %dwriting %dwaiting", rwp->reading, rwp->writing, diminuto_ring_used(&(rwp->ring)));

    END_CRITICAL_SECTION;

    return result;
}

int diminuto_writer_begin(diminuto_readerwriter_t * rwp)
{
    int result = -1;
    int index = -1;

    BEGIN_CRITICAL_SECTION(rwp);

        DIMINUTO_LOG_DEBUG("Writer - BEGIN enter %dreading %dwriting %dwaiting", rwp->reading, rwp->writing, diminuto_ring_used(&(rwp->ring)));

        if ((rwp->reading <= 0) && (rwp->writing <= 0) && (diminuto_ring_used(&(rwp->ring)) <= 0)) {
            /*
             * There are no active readers or writers and no one waiting.
             * Writer can proceed. (Waiters may be threads that have
             * been signalled but have not yet removed themselves from
             * the ring.)
             */
            rwp->writing += 1;
            result = 0;
        } else if (suspend(rwp, WRITER, &index) == 0) {
            /*
             * Either there was one or more active readers or an
             * active writer. The writer count has already been
             * incremented by the thread that signaled us.
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

    END_CRITICAL_SECTION;

    return result;
}

int diminuto_writer_end(diminuto_readerwriter_t * rwp)
{
    int result = -1;
    role_t role = NONE;

    BEGIN_CRITICAL_SECTION(rwp);

        DIMINUTO_LOG_DEBUG("Writer - END enter %dreading %dwriting %dwaiting", rwp->reading, rwp->writing, diminuto_ring_used(&(rwp->ring)));

        rwp->writing -= 1;
        if ((role = resume(rwp, ANY)) == NONE) {
            /*
             * There are no other readers or writers waiting.
             */
            result = 0;
        } else if (role == READER) {
            /*
             * We're activating a reader who has been waiting.
             */
            rwp->reading += 1;
            result = 0;
        } else if (role == WRITER) {
            /*
             * We're activating a writer who has been waiting.
             */
            rwp->writing += 1;
            result = 0;
        } else {
            /* Do nothing. */
        }

        DIMINUTO_LOG_DEBUG("Writer - END exit %dreading %dwriting %dwaiting", rwp->reading, rwp->writing, diminuto_ring_used(&(rwp->ring)));

    END_CRITICAL_SECTION;

    return result;
}
