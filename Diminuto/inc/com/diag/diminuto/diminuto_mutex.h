/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_MUTEX_
#define _H_COM_DIAG_DIMINUTO_MUTEX_

/**
 * @file
 *
 * Copyright 2020 Digital Aggregates Corporation, Colorado, USA.<BR>
 * Licensed under the terms in LICENSE.txt.<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 */

/***********************************************************************
 *
 **********************************************************************/

#define __USE_GNU
#define _GNU_SOURCE
#include <pthread.h>
#include <errno.h>

/***********************************************************************
 *
 **********************************************************************/

static const int DIMINUTO_MUTEX_BUSY = EBUSY;

/***********************************************************************
 *
 **********************************************************************/

typedef struct DiminutoMutex {
    pthread_mutex_t mutex;              /* POSIX Thread mutual exclusion object */
    pthread_mutexattr_t attribute;      /* POSIX Thread mutual exclusion attribute */
} diminuto_mutex_t;

#define DIMINUTO_MUTEX_INITIALIZER \
    { \
        PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP, \
    }

/***********************************************************************
 *
 **********************************************************************/

extern diminuto_mutex_t * diminuto_mutex_init(diminuto_mutex_t * mp);

extern diminuto_mutex_t * diminuto_mutex_fini(diminuto_mutex_t * mp);

extern int diminuto_mutex_lock(diminuto_mutex_t * mp);

extern int diminuto_mutex_lock_try(diminuto_mutex_t * mp);

extern int diminuto_mutex_unlock(diminuto_mutex_t * mp);

extern void diminuto_mutex_cleanup(void * vp);

/***********************************************************************
 *
 **********************************************************************/

/**
 * @def DIMINUTO_MUTEX_BEGIN
 * Begin a code section that is serialized using a Diminuto mutex specified by
 * the caller as a pointer in the argument @a _P_ by locking the mutex.
 */
#define DIMINUTO_MUTEX_BEGIN(_MP_) \
    do { \
        if (diminuto_mutex_lock(_MP_) == 0) { \
            pthread_cleanup_push(diminuto_mutex_cleanup, _MP_); \
            do { \
                (void)0

/**
 * @def DIMINUTO_MUTEX_TRY
 * Conditionally begin a code section that is serialized using a Diminuto
 * mutex specified by the caller as a pointer in the argument @a _MP_
 * by locking the mutex.
 */
#define DIMINUTO_MUTEX_TRY(_MP_) \
    do { \
        if (diminuto_mutex_lock_try(_MP_) == 0) { \
            pthread_cleanup_push(diminuto_mutex_cleanup, _MP_); \
            do { \
                (void)0

/**
 * @def DIMINUTO_MUTEX_END
 * End a code section that was serialized using the Diminuto mutex specified at
 * the beginning of the block by unlocking the mutex.
 */
#define DIMINUTO_MUTEX_END \
            } while (0); \
            pthread_cleanup_pop(!0); \
        } \
    } while (0)

/***********************************************************************
 *
 **********************************************************************/

#endif
