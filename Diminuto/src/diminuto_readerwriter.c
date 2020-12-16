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
#include "com/diag/diminuto/diminuto_frequency.h"

/*******************************************************************************
 * CTORS/DTORS
 ******************************************************************************/

diminuto_readerwriter_t * diminuto_readerwriter_init(diminuto_readerwriter_t * rwp)
{
    diminuto_readerwriter_t * result = (diminuto_readerwriter_t *)0;
    int rc = DIMINUTO_READERWRITER_ERROR;

    rwp->reading = 0;
    rwp->writing = 0;    
    rwp->readers = 0;
    rwp->writers = 0;

    if ((rc = pthread_mutex_init(&(rwp->mutex), (pthread_mutexattr_t *)0)) != 0) {
        errno = rc;
        diminuto_perror("diminuto_readerwriter_init: pthread_mutex_init");
    } else if ((rc = pthread_cond_init(&(rwp->reader), (pthread_condattr_t *)0)) != 0) {
        errno = rc;
        diminuto_perror("diminution_readerwriter_init: pthread_cond_init: reader");
    } else if ((rc = pthread_cond_init(&(rwp->writer), (pthread_condattr_t *)0)) != 0) {
        errno = rc;
        diminuto_perror("diminution_readerwriter_init: pthread_cond_init: writer");
    } else {
        result = rwp;
    }

    return result;
}

diminuto_readerwriter_t * diminuto_readerwriter_fini(diminuto_readerwriter_t * rwp)
{
    diminuto_readerwriter_t * result = rwp;
    int rc = DIMINUTO_READERWRITER_ERROR;

    if (rwp->reading > 0) {
        errno = rc;
        diminuto_perror("diminuto_readerwriter_fini: reading");
    } else if (rwp->writing > 0) {
        errno = rc;
        diminuto_perror("diminuto_readerwriter_fini: writing");
    } else if (rwp->readers > 0) {
        errno = rc;
        diminuto_perror("diminuto_readerwriter_fini: readers");
    } else if (rwp->writers > 0) {
        errno = rc;
        diminuto_perror("diminuto_readerwriter_fini: writers");
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

static int readerwriter_wait(pthread_cond_t * cp, pthread_mutex_t * mp, diminuto_ticks_t clocktime)
{
    int rc = DIMINUTO_READERWRITER_ERROR;
    struct timespec later = { 0, };
    static const diminuto_ticks_t NANOSECONDS = 1000000000;

    if (clocktime == DIMINUTO_READERWRITER_INFINITY) {
        if ((rc = pthread_cond_wait(cp, mp)) == 0) {
            /* Do nothing. */
        } else {
            errno = rc;
            diminuto_perror("readerwriter_wait: pthread_cond_wait");
        }
    } else {
        later.tv_sec = diminuto_frequency_ticks2wholeseconds(clocktime);
        later.tv_nsec = diminuto_frequency_ticks2fractionalseconds(clocktime, NANOSECONDS);
        if ((rc = pthread_cond_timedwait(cp, mp, &later)) == 0) {
            /* Do nothing. */
        } else if (rc == DIMINUTO_READERWRITER_TIMEDOUT) {
            /* Do nothing. */
        } else {
            errno = rc;
            diminuto_perror("readerwriter_wait: pthread_cond_timedwait");
        }
    }

    return rc;
}

static int readerwriter_signal(pthread_cond_t * cp)
{
    int rc = DIMINUTO_READERWRITER_ERROR;

    if ((rc = pthread_cond_broadcast(cp)) != 0) {
        errno = rc;
        diminuto_perror("readerwriter_signal: pthread_cond_broadcast");
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

    rwp->readers -= 1;

    if ((rc = pthread_mutex_unlock(&(rwp->mutex))) != 0) {
        errno = rc;
        diminuto_perror("reader_begin_cleanup: pthread_mutex_unlock");
    }
}

static void reader_end_cleanup(void * vp)
{
    diminuto_readerwriter_t * rwp = (diminuto_readerwriter_t *)vp;
    int rc = DIMINUTO_READERWRITER_ERROR;

    rwp->reading -= 1;

    if ((rc = pthread_mutex_unlock(&(rwp->mutex))) != 0) {
        errno = rc;
        diminuto_perror("reader_end_cleanup: pthread_mutex_unlock");
    }
}

static void writer_begin_cleanup(void * vp)
{
    diminuto_readerwriter_t * rwp = (diminuto_readerwriter_t *)vp;
    int rc = DIMINUTO_READERWRITER_ERROR;

    rwp->writers -= 1;

    if ((rc = pthread_mutex_unlock(&(rwp->mutex))) != 0) {
        errno = rc;
        diminuto_perror("writer_begin_cleanup: pthread_mutex_unlock");
    }
}

static void writer_end_cleanup(void * vp)
{
    diminuto_readerwriter_t * rwp = (diminuto_readerwriter_t *)vp;
    int rc = DIMINUTO_READERWRITER_ERROR;

    rwp->writing -= 1;

    if ((rc = pthread_mutex_unlock(&(rwp->mutex))) != 0) {
        errno = rc;
        diminuto_perror("writer_end_cleanup: pthread_mutex_unlock");
    }
}

/*******************************************************************************
 * ACTIONS
 ******************************************************************************/

int diminuto_reader_begin(diminuto_readerwriter_t * rwp, diminuto_ticks_t clocktime)
{
    int result = DIMINUTO_READERWRITER_ERROR;
    struct timespec later = { 0, };

    if ((result = pthread_mutex_lock(&(rwp->mutex))) != 0) {
        errno = result;
        diminuto_perror("diminuto_reader_begin: pthread_mutex_lock");
    } else {
        pthread_cleanup_push(reader_begin_cleanup, rwp);

            rwp->readers += 1;
 
            while ((rwp->writing > 0) || (rwp->writers > 0)) {
                if ((result = readerwriter_wait(&(rwp->reader), &(rwp->mutex), clocktime)) != 0) {
                    break;
                }
            }

            if (result == 0) {
                rwp->reading += 1;
            }

        pthread_cleanup_pop(!0);
    }

    return result;
}

int diminuto_reader_end(diminuto_readerwriter_t * rwp)
{
    int result = DIMINUTO_READERWRITER_ERROR;

    if (rwp->reading <= 0) {
        result = DIMINUTO_READERWRITER_INVALID;
        errno = result;
        diminuto_perror("diminuto_reader_end: reading");
    } else if ((result = pthread_mutex_lock(&(rwp->mutex))) != 0) {
        errno = result;
        diminuto_perror("diminuto_readerwriter_begin: pthread_mutex_lock");
    } else {
        pthread_cleanup_push(reader_end_cleanup, rwp);

            if ((rwp->reading <= 1) && (rwp->writers > 0)) {
                result = readerwriter_signal(&(rwp->writer));
            }

        pthread_cleanup_pop(!0);
    }

    return result;
}

int diminuto_writer_begin(diminuto_readerwriter_t * rwp, diminuto_ticks_t clocktime)
{
    int result = DIMINUTO_READERWRITER_ERROR;
    struct timespec later = { 0, };

    if ((result = pthread_mutex_lock(&(rwp->mutex))) != 0) {
        errno = result;
        diminuto_perror("diminuto_readerwriter_begin: pthread_mutex_lock");
    } else {
        pthread_cleanup_push(writer_begin_cleanup, rwp);

            rwp->writers += 1;
 
            while ((rwp->reading > 0) || (rwp->writing > 0)) {
                if ((result = readerwriter_wait(&(rwp->writer), &(rwp->mutex), clocktime)) != 0) {
                    break;
                }
            }

            if (result == 0) {
                rwp->writing += 1;
            }

        pthread_cleanup_pop(!0);
    }

    return result;
}

int diminuto_writer_end(diminuto_readerwriter_t * rwp)
{
    int result = DIMINUTO_READERWRITER_ERROR;

    if (rwp->writing != 1) {
        result = DIMINUTO_READERWRITER_INVALID;
        errno = result;
        diminuto_perror("diminuto_writer_end: writing");
    } else if ((result = pthread_mutex_lock(&(rwp->mutex))) != 0) {
        errno = result;
        diminuto_perror("diminuto_writer_end: pthread_mutex_lock");
    } else {
        pthread_cleanup_push(writer_end_cleanup, rwp);

            if (rwp->writers > 0) {
                result = readerwriter_signal(&(rwp->writer));
            } else if (rwp->readers > 0) {
                result = readerwriter_signal(&(rwp->reader));
            } else {
                /* Do nothing. */
            }

        pthread_cleanup_pop(!0);
    }

    return result;
}
