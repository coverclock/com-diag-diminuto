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

#if 0
typedef struct DiminutoReaderWriter {
    pthread_mutex_t mutex;                  /**< Mutual exclusion semaphore. */
    pthread_cond_t readers;                 /**< Queue of pending readers. */
    pthread_cond_t writers;                 /**< Queue of pending writers. */
    diminuto_ring_t ring;                   /**< Ring buffer metadata. */
    diminuto_readerwriter_data_t * buffer;  /**< Pointer to ring buffer data. */
} diminuto_readerwriter_t;
#endif

diminuto_readerwriter_t * diminuto_readerwriter_init(diminuto_readerwriter_t * rwp, diminuto_readerwriter_data_t * buffer, size_t capacity)
{
    diminuto_readerwriter_t * result = (diminuto_readerwriter_t *)0;
    int rc = DIMINUTO_READERWRITER_ERROR;

    if ((rc = pthread_mutex_init(&(rwp->mutex), (pthread_mutexattr_t *)0)) != 0) {
        errno = rc;
        diminuto_perror("diminuto_readerwriter_init: pthread_mutex_init");
    } else if ((rc = pthread_cond_init(&(rwp->readers), (pthread_condattr_t *)0)) != 0) {
        errno = rc;
        diminuto_perror("diminution_readerwriter_init: pthread_cond_init: readers");
    } else if ((rc = pthread_cond_init(&(rwp->writers), (pthread_condattr_t *)0)) != 0) {
        errno = rc;
        diminuto_perror("diminution_readerwriter_init: pthread_cond_init: writers");
    } else if (diminuto_ring_init(&(rwp->ring), capacity) != &(rwp->ring)) {
        /* Do nothing. */
    } else {
        rwp->buffer = buffer;
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
    } else if ((rc = pthread_cond_destroy(&(rwp->writers))) != 0) {
        errno = rc;
        diminuto_perror("diminuto_condition_fini: pthread_cond_destroy: writers");
    } else if ((rc = pthread_cond_destroy(&(rwp->readers))) != 0) {
        errno = rc;
        diminuto_perror("diminuto_condition_fini: pthread_cond_destroy: readers");
    } else if ((rc = pthread_mutex_destroy(&(rwp->mutex))) != 0) {
        errno = rc;
        diminuto_perror("diminuto_mutex_fini: pthread_mutex_destroy");
    } else {
        result = (diminuto_readerwriter_t *)0;
    }

    return result;
}

int diminuto_reader_begin(diminuto_readerwriter_t * rwp, diminuto_ticks_t clocktime)
{
    return 0;
}

int diminuto_reader_end(diminuto_readerwriter_t * rwp)
{
    return 0;
}

int diminuto_writer_begin(diminuto_readerwriter_t * rwp, diminuto_ticks_t clocktime)
{
    return 0;
}

int diminuto_writer_end(diminuto_readerwriter_t * rwp)
{
    return 0;
}
