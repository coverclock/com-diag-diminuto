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
 * This module creates a framework the implements POSIX mutual
 * exclusion (mutex) operations using a very specific model of
 * behavior: all mutexen are recursive (an attribute which
 * greatly simplifies writing reusable multi-threaded libraries.)
 */

/***********************************************************************
 * PREREQUISITES
 **********************************************************************/

#define __USE_GNU
#define _GNU_SOURCE
#include <pthread.h>
#include <errno.h>

/***********************************************************************
 * CONSTANTS
 **********************************************************************/

/**
 * This is the error number returned with the caller tries to lock a
 * locked mutex.
 */
static const int DIMINUTO_MUTEX_BUSY = EBUSY;

/***********************************************************************
 * TYPES
 **********************************************************************/

/**
 * This is the Diminuto mutex object.
 */
typedef struct DiminutoMutex {
    pthread_mutex_t mutex;              /* POSIX Thread mutual exclusion object */
    pthread_mutexattr_t attribute;      /* POSIX Thread mutual exclusion attribute */
} diminuto_mutex_t;

/**
 * @def DIMINUTO_MUTEX_INITIALIZER
 * This is a static initializer for a Diminuto mutex object.
 */
#define DIMINUTO_MUTEX_INITIALIZER \
    { \
        PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP, \
    }

/***********************************************************************
 * INITIALIZERS AND FINALIZERS
 **********************************************************************/

/**
 * Initialize a Diminuto mutex object. Allocate any resources.
 * @param mp points to the object.
 * @return a pointer to the object or NULL if initializetion failed.
 */
extern diminuto_mutex_t * diminuto_mutex_init(diminuto_mutex_t * mp);

/**
 * Finalize a Diminuto mutex object. Release any resources.
 * @param mp points to the object.
 * @return NULL or a pointer to the object if finalization failed.
 */
extern diminuto_mutex_t * diminuto_mutex_fini(diminuto_mutex_t * mp);

/***********************************************************************
 * OPERATIONS
 **********************************************************************/

/**
 * Lock a Diminuto mutex object.
 * @param mp points to the object.
 * @return 0 or an error code if the lock failed.
 */
extern int diminuto_mutex_lock(diminuto_mutex_t * mp);

/**
 * Attempt to lock a Diminuto mutex object. EBUSY is returned
 * (and no error message is generated) if the object was already
 * locked.
 * @param mp points to the object.
 * @return 0 or an error code if the lock failed.
 */
extern int diminuto_mutex_lock_try(diminuto_mutex_t * mp);

/**
 * Unlock the Diminuto mutex object.
 * @param mp points to the object.
 * @return 0 or an error code if the unlock failed.
 */
extern int diminuto_mutex_unlock(diminuto_mutex_t * mp);

/***********************************************************************
 * CALLBACKS
 **********************************************************************/

/**
 * This is a callback used to unlock a Diminuto mutex object in the
 * event of a cancellation.
 * @param vp points to the object.
 */
extern void diminuto_mutex_cleanup(void * vp);

/***********************************************************************
 * MACROS
 **********************************************************************/

/**
 * @def DIMINUTO_MUTEX_BEGIN
 * Begin a code section that is serialized using a Diminuto mutex
 * specified by the caller as a pointer in the argument @a _MP_ by
 * locking the mutex.
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
 * End a code section that was serialized using the Diminuto mutex
 * specified at the beginning of the block by unlocking the mutex.
 */
#define DIMINUTO_MUTEX_END \
            } while (0); \
            pthread_cleanup_pop(!0); \
        } \
    } while (0)

#endif
