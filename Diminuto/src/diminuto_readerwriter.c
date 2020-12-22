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

diminuto_readerwriter_t * diminuto_readerwriter_init(diminuto_readerwriter_t * rwp, diminuto_readerwriter_buffer_t * buffer, size_t capacity)
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
        /* Do nothing. */
    } else {
        rwp->buffer = buffer;
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
        /* Do nothing. */
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

static int queue_reader(diminuto_readerwriter_t * rwp)
{
    int rc = DIMINUTO_READERWRITER_ERROR;
    int index = -1;

    if ((index = diminuto_ring_producer_request(&(rwp->ring), 1)) < 0) {
        rc = DIMINUTO_READERWRITER_FULL;
        errno = rc;
        diminuto_perror("queue_reader");
    } else {
        DIMINUTO_BITS_CLEAR(diminuto_readerwriter_buffer_t, index, rwp->buffer);
        DIMINUTO_LOG_DEBUG("Reader WAITING");
        if ((rc = pthread_cond_wait(&(rwp->reader), &(rwp->mutex))) != 0) {
            errno = rc;
            diminuto_perror("queue_reader: pthread_cond_wait");
            if (diminuto_ring_producer_revoke(&(rwp->ring), 1) < 0) {
                rc = DIMINUTO_READERWRITER_STATE;
                errno = rc;
                diminuto_perror("queue_reader");
            }
        } else {
            DIMINUTO_LOG_DEBUG("Reader SIGNALED");
        }
    }

    return rc;
}

static int queue_writer(diminuto_readerwriter_t * rwp)
{
    int rc = DIMINUTO_READERWRITER_ERROR;
    int index = -1;

    if ((index = diminuto_ring_producer_request(&(rwp->ring), 1)) < 0) {
        rc = DIMINUTO_READERWRITER_FULL;
        errno = rc;
        diminuto_perror("queue_writer");
    } else {
        DIMINUTO_BITS_SET(diminuto_readerwriter_buffer_t, index, rwp->buffer);
        DIMINUTO_LOG_DEBUG("Writer WAITING");
        if ((rc = pthread_cond_wait(&(rwp->writer), &(rwp->mutex))) != 0) {
            errno = rc;
            diminuto_perror("queue_writer: pthread_cond_wait");
            if (diminuto_ring_producer_revoke(&(rwp->ring), 1) < 0) {
                rc  = DIMINUTO_READERWRITER_STATE;
                errno = rc;
                diminuto_perror("queue_writer");
            }
        } else {
            DIMINUTO_LOG_DEBUG("Writer SIGNALED");
        }
    }

    return rc;
}

static int dequeue(diminuto_readerwriter_t * rwp)
{
    int rc = DIMINUTO_READERWRITER_ERROR;
    int index = -1;
    int bit = -1;
    int reading = 0;

    while (!0) {
        if ((index = diminuto_ring_consumer_peek(&(rwp->ring))) < 0) {
            /*
             * There are no readers or writers waiting. We're done.
             */
            rc = 0;
            break;
        } else if ((bit = DIMINUTO_BITS_GET(diminuto_readerwriter_buffer_t, index, rwp->buffer)) && reading) {
            /*
             * The next waiter is a writer but we are only doing readers.
             * We're done.
             */
            rc = 0;
            break;
        } else if (bit) {
            /*
             * The next waiter was a writer. Wake it up and We're done.
             */
            if (diminuto_ring_consumer_request(&(rwp->ring), 1) != index) {
                errno = rc;
                diminuto_perror("dequeue");
            } else if ((rc = pthread_cond_signal(&(rwp->writer))) != 0) {
                errno = rc;
                diminuto_perror("dequeue: pthread_cond_signal: writer");
            } else {
                /* Do nothing. */
            }
            break;
        } else {
            /*
             * The next waiter was a reader. Wake it up and continue to
             * service the queue as long as we find readers.
             */
            if (diminuto_ring_consumer_request(&(rwp->ring), 1) != index) {
                errno = rc;
                diminuto_perror("dequeue");
                break;
            } else if ((rc = pthread_cond_signal(&(rwp->reader))) != 0) {
                errno = rc;
                diminuto_perror("dequeue: pthread_cond_signal: reader");
                break;
            } else {
                reading = !0;
            }
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

            if ((rwp->active >= 0) && (diminuto_readerwriter_total(rwp) == 0)) {
                /*
                 * There are zero or more active readers and no one is waiting.
                 * Reader can proceeed.
                 */
                rwp->active += 1;
                result = 0;
                DIMINUTO_LOG_DEBUG("Reader ACTIVE %d", rwp->active);
            } else if (queue_reader(rwp) == 0) {
                /*
                 * Either there was an active writer or someone is waiting.
                 * If someone is waiting, it is presumably a writer, since
                 * a reader would not have waited.
                 */
                rwp->active += 1;
                result = 0;
                DIMINUTO_LOG_DEBUG("Reader REACTIVE %d", rwp->active);
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

    if (rwp->active <= 0) {
        errno = DIMINUTO_READERWRITER_ORDER;
        diminuto_perror("diminuto_reader_end: active");
    } else if ((rc = pthread_mutex_lock(&(rwp->mutex))) != 0) {
        errno = rc;
        diminuto_perror("diminuto_reader_end: pthread_mutex_lock");
    } else {
        pthread_cleanup_push(reader_end_cleanup, rwp);

            if (rwp->active != 1) {
                result = 0;
                DIMINUTO_LOG_DEBUG("Reader IDLING %d", rwp->active);
            } else if ((rc = dequeue(rwp)) == 0) {
                /*
                 * This was the last active reader. If anyones is waiting,
                 * the first waiter must be a writer, since a reader would
                 * not have waited.
                 */
                result = 0;
                DIMINUTO_LOG_DEBUG("Reader IDLE %d", rwp->active);
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
                DIMINUTO_LOG_DEBUG("Writer ACTIVE %d", rwp->active);
            } else if (queue_writer(rwp) == 0) {
                /*
                 * Either there was one or more active readers or an
                 * active writer.
                 */
                rwp->active -= 1;
                result = 0;
                DIMINUTO_LOG_DEBUG("Writer REACTIVE %d", rwp->active);
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

    if (rwp->active != -1) {
        errno = DIMINUTO_READERWRITER_ORDER;
        diminuto_perror("diminuto_writer_end: active");
    } else if ((rc = pthread_mutex_lock(&(rwp->mutex))) != 0) {
        errno = rc;
        diminuto_perror("diminuto_writer_end: pthread_mutex_lock");
    } else {
        pthread_cleanup_push(writer_end_cleanup, rwp);

            if (dequeue(rwp) == 0) {
                DIMINUTO_LOG_DEBUG("Writer IDLE %d", rwp->active);
                result = 0;
            } else {
                /* Failed! */
            }

        pthread_cleanup_pop(!0);
    }

    return result;
}
