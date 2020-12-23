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
#include "com/diag/diminuto/diminuto_frequency.h"
#include "com/diag/diminuto/diminuto_log.h"

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
        rwp->readers = 0;
        rwp->writers = 0;
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

static int resume_either(diminuto_readerwriter_t * rwp)
{
    int rc = DIMINUTO_READERWRITER_ERROR;
    int index = -1;
    diminuto_readerwriter_state_t ss = -1;

    if ((index = diminuto_ring_consumer_peek(&(rwp->ring))) < 0) {
        /*
         * There are no readers or writers waiting. We're done.
         */
        rc = 0;
    } else if ((ss = rwp->state[index]) == DIMINUTO_READERWRITER_READER) {
        /*
         * The next waiter is a reader. Activate it. It will resume
         * the next waiter if it is also a reader. We do a broadcast
         * because POSIX doesn't guarantee FIFO behavior on the part
         * of condition variables. The siganled reader only activates
         * if it is at the head of the ring.
         */
        if ((rc = pthread_cond_broadcast(&(rwp->reader))) != 0) {
            errno = rc;
            diminuto_perror("resume_either: pthread_cond_broadcast: reader");
        } else {
            /*
             * It is important (and subtle) for fairness that we increment
             * the reader count on behalf of the reader we just signalled.
             * Otherwise we may exit the critical section and reenter it
             * before the signalled reader has a chance to enter it.
             */
            rwp->readers += 1;
            DIMINUTO_LOG_DEBUG("Reader %d SIGNALED", index);
        }
    } else if (ss == DIMINUTO_READERWRITER_WRITER) {
        /*
         * The next waiter is a writer. Activate it. When it is done
         * writing it will resume the next head of the ring,
         * whatever it is. We do a broadcast because POSIX doesn't
         * guarantee FIFO behavior on the part of condition variables.
         * The signaled writer only activates if it is at the head
         * of the ring.
         */
        if ((rc = pthread_cond_broadcast(&(rwp->writer))) != 0) {
            errno = rc;
            diminuto_perror("resume_either: pthread_cond_broadcast: writer");
        } else {
            /*
             * It is important (and subtle) for fairness that we increment
             * the writer count on behalf of the writer we just signalled.
             * Otherwise we may exit the critical section and reenter it
             * before the signalled writer has a chance to enter it.
             */
            rwp->writers += 1;
            DIMINUTO_LOG_DEBUG("Writer %d SIGNALED", index);
        }
    } else {
        diminuto_perror("resume_either: index");
    }

    return rc;
}

static int resume_reader(diminuto_readerwriter_t * rwp)
{
    int rc = DIMINUTO_READERWRITER_ERROR;
    int index = -1;
    diminuto_readerwriter_state_t ss = -1;

    if ((index = diminuto_ring_consumer_peek(&(rwp->ring))) < 0) {
        /*
         * There are no readers or writers waiting. We're done.
         */
        rc = 0;
    } else if ((ss = rwp->state[index]) == DIMINUTO_READERWRITER_READER) {
        /*
         * The next waiter is a reader. Activate it. It will continue
         * continue to resume successive readers iff they are at the head
         * of the ring. We do a broadcast because POSIX doesn't guarantee
         * FIFO behavior on the part of conditionl variables. The signaled
         * reader only activates if it is at the head of the ring.
         */
        if ((rc = pthread_cond_broadcast(&(rwp->reader))) != 0) {
            errno = rc;
            diminuto_perror("resume_reader: pthread_cond_broadcast: reader");
        } else {
            /*
             * It is important (and subtle) for fairness that we increment
             * the reader count on behalf of the reader we just signalled.
             * Otherwise we may exit the critical section and reenter it
             * before the signalled reader has a chance to enter it.
             */
            rwp->readers += 1;
            DIMINUTO_LOG_DEBUG("Reader %d SIGNALED", index);
        }
    } else if (ss == DIMINUTO_READERWRITER_WRITER) {
        /*
         * The next waiter was a writer. We're only activating readers.
         * We're done.
         */
        rc = 0;
    } else {
        diminuto_perror("resume_reader: index");
    }

    return rc;
}

static int suspend_reader(diminuto_readerwriter_t * rwp, int * indexp)
{
    int rc = DIMINUTO_READERWRITER_ERROR;
    int index = -1;

    if ((index = diminuto_ring_producer_request(&(rwp->ring), 1)) < 0) {
        rc = DIMINUTO_READERWRITER_FULL;
        errno = rc;
        diminuto_perror("suspend_reader: producer");
    } else {

        rwp->state[index] = DIMINUTO_READERWRITER_READER;
        DIMINUTO_LOG_DEBUG("Reader %d WAITING", index);
        *indexp = index; /* Just for logging. */

        /*
         * Wait until this reader is signaled and at the head of the ring.
         * We have to do this because POSIX doesn't guarantee FIFO behavior
         * on the part of condition variables.
         */

        do {
            if ((rc = pthread_cond_wait(&(rwp->reader), &(rwp->mutex))) != 0) {
                errno = rc;
                diminuto_perror("suspend_reader: pthread_cond_wait");
                if (diminuto_ring_producer_revoke(&(rwp->ring), 1) < 0) {
                    rc = DIMINUTO_READERWRITER_UNEXPECTED;
                    errno = rc;
                    diminuto_perror("suspend_reader: revoke");
                }
                break;
            }
        } while (diminuto_ring_consumer_peek(&(rwp->ring)) != index);

        /*
         * Reader resumed. The only way this could have occurred
         * is if the reader is at the head of the ring. We will resume
         * the next head of the ring if it is also a reader.
         */

        if (rc != 0) {
            /* Failed! */
        } else if (diminuto_ring_consumer_request(&(rwp->ring), 1) != index) {
            rc = DIMINUTO_READERWRITER_UNEXPECTED;
            errno = rc;
            diminuto_perror("suspend_reader: consumer");
        } else if ((rc = resume_reader(rwp)) != 0) {
            /* Failed!. */
        } else {
            DIMINUTO_LOG_DEBUG("Reader %d READY", index);
        }

    }

    return rc;
}

static int suspend_writer(diminuto_readerwriter_t * rwp, int * indexp)
{
    int rc = DIMINUTO_READERWRITER_ERROR;
    int index = -1;

    if ((index = diminuto_ring_producer_request(&(rwp->ring), 1)) < 0) {
        rc = DIMINUTO_READERWRITER_FULL;
        errno = rc;
        diminuto_perror("suspend_writer: producer");
    } else {

        rwp->state[index] = DIMINUTO_READERWRITER_WRITER;
        DIMINUTO_LOG_DEBUG("Writer %d WAITING", index);
        *indexp = index; /* Just for logging. */

        /*
         * Wait until this writer is signaled and at the head of the ring.
         * We have to do this because POSIX doesn't guarantee FIFIO behavior
         * on the part of condition variables.
         */

        do {
            if ((rc = pthread_cond_wait(&(rwp->writer), &(rwp->mutex))) != 0) {
                errno = rc;
                diminuto_perror("suspend_writer: pthread_cond_wait");
                if (diminuto_ring_producer_revoke(&(rwp->ring), 1) < 0) {
                    rc = DIMINUTO_READERWRITER_UNEXPECTED;
                    errno = rc;
                    diminuto_perror("suspend_writer: revoke");
                }
                break;
            }
        } while (diminuto_ring_consumer_peek(&(rwp->ring)) != index);

        /*
         * Writer resumed. The only way this could have occurred
         * is if the writer is at the head of the ring.
         */

        if (rc != 0) {
            /* Failed! */
        } else if (diminuto_ring_consumer_request(&(rwp->ring), 1) != index) {
            rc = DIMINUTO_READERWRITER_UNEXPECTED;
            errno = rc;
            diminuto_perror("suspend_reader: consumer");
        } else {
            DIMINUTO_LOG_DEBUG("Writer %d READY", index);
        }

    }

    return rc;
}

/*******************************************************************************
 * CALLBACKS
 ******************************************************************************/

static void critical_section_cleanup(void * vp)
{
    diminuto_readerwriter_t * rwp = (diminuto_readerwriter_t *)vp;
    int rc = DIMINUTO_READERWRITER_ERROR;

    if ((rc = pthread_mutex_unlock(&(rwp->mutex))) != 0) {
        errno = rc;
        diminuto_perror("reader_begin_cleanup: pthread_mutex_unlock");
    }
}

/*******************************************************************************
 * GENERATORS
 ******************************************************************************/

#define BEGIN_CRITICAL_SECTION(_RWP_) \
    do { \
        int critical_section_rc = DIMINUTO_READERWRITER_ERROR; \
        if ((critical_section_rc = pthread_mutex_lock(&((_RWP_)->mutex))) != 0) { \
            errno = critical_section_rc; \
            diminuto_perror("pthread_mutex_lock"); \
        } else { \
            pthread_cleanup_push(critical_section_cleanup, _RWP_)

#define END_CRITICAL_SECTION \
            pthread_cleanup_pop(!0); \
            critical_section_rc = 0; \
        } \
    } while (0)

/*******************************************************************************
 * ACTIONS
 ******************************************************************************/

int diminuto_reader_begin(diminuto_readerwriter_t * rwp)
{
    int result = -1;
    int index = -1;

    BEGIN_CRITICAL_SECTION(rwp);

        if ((rwp->writers <= 0) && (diminuto_ring_used(&(rwp->ring)) <= 0)) {
            /*
             * There are zero or more active writers and no one is waiting.
             * Reader can proceeed.
             */
            rwp->readers += 1;
            DIMINUTO_LOG_DEBUG("Reader - ACTIVE %dreaders %dwriters", rwp->readers, rwp->writers);
            result = 0;
        } else if (suspend_reader(rwp, &index) == 0) {
            /*
             * Either there was an active writer or someone is waiting.
             * If someone is waiting, it is presumably a writer, since
             * a reader would not have waited. The reader count has
             * already been incremented by the thread that signalled
             * us.
             */
            DIMINUTO_LOG_DEBUG("Reader %d ACTIVE %dreaders %dwriters", index, rwp->readers, rwp->writers);
            result = 0;
        } else {
            /* Failed! */
        }

    END_CRITICAL_SECTION;

    return result;
}

int diminuto_reader_end(diminuto_readerwriter_t * rwp)
{
    int result = -1;

    BEGIN_CRITICAL_SECTION(rwp);

        rwp->readers -= 1;
        if (rwp->readers > 0) {
            DIMINUTO_LOG_DEBUG("Reader - IDLING %dreaders %dwriters", rwp->readers, rwp->writers);
            result = 0;
        } else if (resume_either(rwp) == 0) {
            /*
             * This was the last active reader. If anyones is waiting,
             * the first waiter must be a writer, since a reader would
             * not have waited.
             */
            DIMINUTO_LOG_DEBUG("Reader - IDLE %dreaders %dwriters", rwp->readers, rwp->writers);
            result = 0;
        } else {
            /* Failed! */
        }

    END_CRITICAL_SECTION;

    return result;
}

int diminuto_writer_begin(diminuto_readerwriter_t * rwp)
{
    int result = -1;
    int index = -1;

    BEGIN_CRITICAL_SECTION(rwp);

        if ((rwp->readers <= 0) && (rwp->writers <= 0)) {
            /*
             * There are zero active readers or writers.
             * Writer can proceeed.
             */
            rwp->writers += 1;
            DIMINUTO_LOG_DEBUG("Writer - ACTIVE %dreaders %dwriters", rwp->readers, rwp->writers);
            result = 0;
        } else if (suspend_writer(rwp, &index) == 0) {
            /*
             * Either there was one or more active readers or an
             * active writer. The writer count has already been
             * incremented by the thread that signaled us.
             */
            DIMINUTO_LOG_DEBUG("Writer %d ACTIVE %dreaders %dwriters", index, rwp->readers, rwp->writers);
            result = 0;
        } else {
            /* Failed! */
        }

    END_CRITICAL_SECTION;

    return result;
}

int diminuto_writer_end(diminuto_readerwriter_t * rwp)
{
    int result = -1;

    BEGIN_CRITICAL_SECTION(rwp);

        rwp->writers -= 1;
        if (resume_either(rwp) == 0) {
            DIMINUTO_LOG_DEBUG("Writer - IDLE %dreaders %dwriters", rwp->readers, rwp->writers);
            result = 0;
        } else {
            /* Failed! */
        }

    END_CRITICAL_SECTION;

    return result;
}
