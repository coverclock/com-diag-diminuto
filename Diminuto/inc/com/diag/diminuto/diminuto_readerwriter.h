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
 * Work In Progress!
 */

/*******************************************************************************
 * PREREQUISITES
 ******************************************************************************/

#include "com/diag/diminuto/diminuto_types.h"
#include <errno.h>
#include <pthread.h>

/*******************************************************************************
 * CONSTANTS
 ******************************************************************************/

enum DiminutoReaderWriterErrno {
    DIMINUTO_READERWRITER_ERROR = EIO,
    DIMINUTO_READERWRITER_INVALID = EINVAL,
    DIMINUTO_READERWRITER_TIMEDOUT = ETIMEDOUT,
};

/*******************************************************************************
 * GLOBALS
 ******************************************************************************/

static const diminuto_ticks_t DIMINUTO_READERWRITER_INFINITY = (~(diminuto_ticks_t)0);

/*******************************************************************************
 * TYPES
 ******************************************************************************/

typedef struct DiminutoReaderWriter {
    pthread_mutex_t mutex;
    pthread_cond_t reader;
    pthread_cond_t writer;
    int reading;
    int writing;
    int readers;
    int writers;
} diminuto_readerwriter_t;

#define DIMINUTO_READERWRITER_INITIALIZER \
    { \
        PTHREAD_MUTEX_INITIALIZER, \
        PTHREAD_COND_INITIALIZER, \
        PTHREAD_COND_INITIALIZER, \
        0, \
        0, \
        0, \
        0, \
    }

/*******************************************************************************
 * CTORS/DTORS
 ******************************************************************************/

extern diminuto_readerwriter_t * diminuto_readerwriter_init(diminuto_readerwriter_t * rwp);

extern diminuto_readerwriter_t * diminuto_readerwriter_fini(diminuto_readerwriter_t * rwp);

/*******************************************************************************
 * ACTIONS
 ******************************************************************************/

extern int diminuto_reader_begin(diminuto_readerwriter_t * rwp, diminuto_ticks_t clocktime);

extern int diminuto_reader_end(diminuto_readerwriter_t * rwp);

extern int diminuto_writer_begin(diminuto_readerwriter_t * rwp, diminuto_ticks_t clocktime);

extern int diminuto_writer_end(diminuto_readerwriter_t * rwp);

/*******************************************************************************
 * GENERATORS
 ******************************************************************************/

#define DIMINUTO_READER_BEGIN(_RWP_) \
    do { \
        diminuto_readerwriter_t * diminuto_readerwriter_rwp = (diminuto_readerwriter_t *)0; \
        diminuto_readerwriter_rwp = (_RWP_); \
        if (diminuto_reader_begin(diminuto_readerwriter_rwp, DIMINUTO_READERWRITER_INFINITY) == 0) { \
            do { \
                (void)0

#define DIMINUTO_READER_END \
            } while (0); \
            diminuto_reader_end(diminuto_readerwriter_rwp); \
        } \
    } while (0)

#define DIMINUTO_WRITER_BEGIN(_RWP_) \
    do { \
        diminuto_readerwriter_t * diminuto_readerwriter_rwp = (diminuto_readerwriter_t *)0; \
        diminuto_readerwriter_rwp = (_RWP_); \
        if (diminuto_writer_begin(diminuto_readerwriter_rwp, DIMINUTO_READERWRITER_INFINITY) == 0) { \
            do { \
                (void)0

#define DIMINUTO_WRITER_END \
            } while (0); \
            diminuto_writer_end(diminuto_readerwriter_rwp); \
        } \
    } while (0)

#endif
