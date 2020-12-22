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
        rwp->active = 0;
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

static int dequeue_next(diminuto_readerwriter_t * rwp)
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
         * The next waiter is a reader. Activate it. It will activate
         * the next waiter if it is also a reader.
         */
        rwp->state[index] = DIMINUTO_READERWRITER_READY;
        if ((rc = pthread_cond_broadcast(&(rwp->reader))) != 0) {
            errno = rc;
            diminuto_perror("dequeue_next: pthread_cond_broadcast: reader");
        } else {
            DIMINUTO_LOG_DEBUG("Reader %d SIGNALED", index);
        }
    } else if (ss == DIMINUTO_READERWRITER_WRITER) {
        /*
         * The next waiter is a writer. Activate it. When it is done
         * writing it will activate the next waiter.
         */
        rwp->state[index] = DIMINUTO_READERWRITER_READY;
        if ((rc = pthread_cond_broadcast(&(rwp->writer))) != 0) {
            errno = rc;
            diminuto_perror("dequeue_next: pthread_cond_broadcast: writer");
        } else {
            DIMINUTO_LOG_DEBUG("Writer %d SIGNALED", index);
        }
    } else {
        diminuto_perror("dequeue_next: index");
    }

    return rc;
}

static int dequeue_reader(diminuto_readerwriter_t * rwp)
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
         * The next waiter was a reader. It will become active and
         * continue to activate successive readers.
         */
        rwp->state[index] = DIMINUTO_READERWRITER_READY;
        if ((rc = pthread_cond_broadcast(&(rwp->reader))) != 0) {
            errno = rc;
            diminuto_perror("dequeue_reader: pthread_cond_broadcast: reader");
        } else {
            DIMINUTO_LOG_DEBUG("Reader %d SIGNALED", index);
        }
    } else if (ss == DIMINUTO_READERWRITER_WRITER) {
        /*
         * The next waiter was a writer. We're only activating readers.
         * We're done.
         */
        rc = 0;
    } else {
        diminuto_perror("dequeue_reader: index");
    }

    return rc;
}

static int queue_reader(diminuto_readerwriter_t * rwp, int * indexp)
{
    int rc = DIMINUTO_READERWRITER_ERROR;
    int index = -1;

    if ((index = diminuto_ring_producer_request(&(rwp->ring), 1)) < 0) {
        rc = DIMINUTO_READERWRITER_FULL;
        errno = rc;
        diminuto_perror("queue_reader: producer");
    } else {

        rwp->state[index] = DIMINUTO_READERWRITER_READER;
        DIMINUTO_LOG_DEBUG("Reader %d WAITING", index);
        *indexp = index; /* Just for logging. */

        do {
            if ((rc = pthread_cond_wait(&(rwp->reader), &(rwp->mutex))) != 0) {
                errno = rc;
                diminuto_perror("queue_reader: pthread_cond_wait");
                if (diminuto_ring_producer_revoke(&(rwp->ring), 1) < 0) {
                    rc = DIMINUTO_READERWRITER_STATE;
                    errno = rc;
                    diminuto_perror("queue_reader: revoke");
                }
                break;
            }
        } while (rwp->state[index] != DIMINUTO_READERWRITER_READY);

        if (rc != 0) {
            /* Failed! */
        } else if ((diminuto_ring_consumer_request(&(rwp->ring), 1)) != index) {
            rc = DIMINUTO_READERWRITER_STATE;
            errno = rc;
            diminuto_perror("queue_reader: consumer");
        } else if ((rc = dequeue_reader(rwp)) != 0) {
            /* Failed!. */
        } else {
            DIMINUTO_LOG_DEBUG("Reader %d READY", index);
        }

    }

    return rc;
}

static int queue_writer(diminuto_readerwriter_t * rwp, int * indexp)
{
    int rc = DIMINUTO_READERWRITER_ERROR;
    int index = -1;

    if ((index = diminuto_ring_producer_request(&(rwp->ring), 1)) < 0) {
        rc = DIMINUTO_READERWRITER_FULL;
        errno = rc;
        diminuto_perror("queue_writer: producer");
    } else {

        rwp->state[index] = DIMINUTO_READERWRITER_WRITER;
        DIMINUTO_LOG_DEBUG("Writer %d WAITING", index);
        *indexp = index; /* Just for logging. */

        do {
            if ((rc = pthread_cond_wait(&(rwp->writer), &(rwp->mutex))) != 0) {
                errno = rc;
                diminuto_perror("queue_writer: pthread_cond_wait");
                if (diminuto_ring_producer_revoke(&(rwp->ring), 1) < 0) {
                    rc  = DIMINUTO_READERWRITER_STATE;
                    errno = rc;
                    diminuto_perror("queue_writer: revoke");
                }
                break;
            }
        } while (rwp->state[index] != DIMINUTO_READERWRITER_READY);

        if (rc != 0) {
            /* Failed! */
        } else if ((diminuto_ring_consumer_request(&(rwp->ring), 1)) != index) {
            rc = DIMINUTO_READERWRITER_STATE;
            errno = rc;
            diminuto_perror("queue_reader: consumer");
        } else {
            DIMINUTO_LOG_DEBUG("Writer %d READY", index);
        }

    }

    return rc;
}

/*******************************************************************************
 * CALLBACKS
 ******************************************************************************/

static void reader_begin_cleanup(void * vp)
{
    diminuto_readerwriter_t * rwp = (diminuto_readerwriter_t *)vp;
    int rc = DIMINUTO_READERWRITER_ERROR;

    if ((rc = pthread_mutex_unlock(&(rwp->mutex))) != 0) {
        errno = rc;
        diminuto_perror("reader_begin_cleanup: pthread_mutex_unlock");
    }
}

static void reader_end_cleanup(void * vp)
{
    diminuto_readerwriter_t * rwp = (diminuto_readerwriter_t *)vp;
    int rc = DIMINUTO_READERWRITER_ERROR;

    rwp->active -= 1;
    if ((rc = pthread_mutex_unlock(&(rwp->mutex))) != 0) {
        errno = rc;
        diminuto_perror("reader_end_cleanup: pthread_mutex_unlock");
    }
}

static void writer_begin_cleanup(void * vp)
{
    diminuto_readerwriter_t * rwp = (diminuto_readerwriter_t *)vp;
    int rc = DIMINUTO_READERWRITER_ERROR;

    if ((rc = pthread_mutex_unlock(&(rwp->mutex))) != 0) {
        errno = rc;
        diminuto_perror("writer_begin_cleanup: pthread_mutex_unlock");
    }
}

static void writer_end_cleanup(void * vp)
{
    diminuto_readerwriter_t * rwp = (diminuto_readerwriter_t *)vp;
    int rc = DIMINUTO_READERWRITER_ERROR;

    rwp->active += 1;
    if ((rc = pthread_mutex_unlock(&(rwp->mutex))) != 0) {
        errno = rc;
        diminuto_perror("writer_end_cleanup: pthread_mutex_unlock");
    }
}

/*******************************************************************************
 * ACTIONS
 ******************************************************************************/

int diminuto_reader_begin(diminuto_readerwriter_t * rwp)
{
    int result = -1;
    int rc = DIMINUTO_READERWRITER_ERROR;
    int index = -1;

    if ((rc = pthread_mutex_lock(&(rwp->mutex))) != 0) {
        errno = rc;
        diminuto_perror("diminuto_reader_begin: pthread_mutex_lock");
    } else {
        pthread_cleanup_push(reader_begin_cleanup, rwp);

            if ((rwp->active >= 0) && (diminuto_readerwriter_waiting(rwp) <= 0)) {
                /*
                 * There are zero or more active readers and no one is waiting.
                 * Reader can proceeed.
                 */
                rwp->active += 1;
                result = 0;
                DIMINUTO_LOG_DEBUG("Reader - ACTIVE %d", rwp->active);
            } else if (queue_reader(rwp, &index) == 0) {
                /*
                 * Either there was an active writer or someone is waiting.
                 * If someone is waiting, it is presumably a writer, since
                 * a reader would not have waited.
                 */
                rwp->active += 1;
                result = 0;
                DIMINUTO_LOG_DEBUG("Reader %d ACTIVE %d", index, rwp->active);
            } else {
                /* Failed! */
            }

        pthread_cleanup_pop(!0);
    }

    return result;
}

int diminuto_reader_end(diminuto_readerwriter_t * rwp)
{
    int result = -1;
    int rc = DIMINUTO_READERWRITER_ERROR;

    if ((rc = pthread_mutex_lock(&(rwp->mutex))) != 0) {
        errno = rc;
        diminuto_perror("diminuto_reader_end: pthread_mutex_lock");
    } else {
        pthread_cleanup_push(reader_end_cleanup, rwp);

            if (rwp->active != 1) {
                result = 0;
                DIMINUTO_LOG_DEBUG("Reader - IDLING %d", rwp->active);
            } else if ((rc = dequeue_next(rwp)) == 0) {
                /*
                 * This was the last active reader. If anyones is waiting,
                 * the first waiter must be a writer, since a reader would
                 * not have waited.
                 */
                result = 0;
                DIMINUTO_LOG_DEBUG("Reader - IDLE %d", rwp->active);
            } else {
                /* Failed! */
            }

        pthread_cleanup_pop(!0);
    }

    return result;
}

int diminuto_writer_begin(diminuto_readerwriter_t * rwp)
{
    int result = -1;
    int rc = DIMINUTO_READERWRITER_ERROR;
    int index = -1;

    if ((rc = pthread_mutex_lock(&(rwp->mutex))) != 0) {
        errno = rc;
        diminuto_perror("diminuto_writer_begin: pthread_mutex_lock");
    } else {
        pthread_cleanup_push(writer_begin_cleanup, rwp);

            if (rwp->active == 0) {
                /*
                 * There are zero active readers or writers.
                 * Writer can proceeed.
                 */
                rwp->active -= 1;
                result = 0;
                DIMINUTO_LOG_DEBUG("Writer - ACTIVE %d", rwp->active);
            } else if (queue_writer(rwp, &index) == 0) {
                /*
                 * Either there was one or more active readers or an
                 * active writer.
                 */
                rwp->active -= 1;
                result = 0;
                DIMINUTO_LOG_DEBUG("Writer %d ACTIVE %d", index, rwp->active);
            } else {
                /* Failed! */
            }

        pthread_cleanup_pop(!0);
    }

    return result;
}

int diminuto_writer_end(diminuto_readerwriter_t * rwp)
{
    int result = -1;
    int rc = DIMINUTO_READERWRITER_ERROR;

    if ((rc = pthread_mutex_lock(&(rwp->mutex))) != 0) {
        errno = rc;
        diminuto_perror("diminuto_writer_end: pthread_mutex_lock");
    } else {
        pthread_cleanup_push(writer_end_cleanup, rwp);

            if (dequeue_next(rwp) == 0) {
                DIMINUTO_LOG_DEBUG("Writer - IDLE %d", rwp->active);
                result = 0;
            } else {
                /* Failed! */
            }

        pthread_cleanup_pop(!0);
    }

    return result;
}
