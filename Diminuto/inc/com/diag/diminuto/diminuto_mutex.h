/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_MUTEX_
#define _H_COM_DIAG_DIMINUTO_MUTEX_

/**
 * @file
 * @copyright Copyright 2020 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief Implements a framework to use POSIX mutexen.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 *
 * The Mutex feature creates a framework that implements POSIX mutual
 * exclusion (mutex) operations using a very specific model of
 * behavior: all mutexen are recursive (an attribute which
 * greatly simplifies writing reusable multi-threaded subroutines).
 */

/***********************************************************************
 * PREREQUISITES
 **********************************************************************/

#include <pthread.h>
#include <errno.h>

/***********************************************************************
 * CONSTANTS
 **********************************************************************/

/**
 * Defines error codes.
 */
enum DiminutoMutexErrno {
    DIMINUTO_MUTEX_ERROR    = EIO,      /**< Non-specific error. */
    DIMINUTO_MUTEX_BUSY     = EBUSY,    /**< Mutex already locked. */
};

/***********************************************************************
 * TYPES
 **********************************************************************/

/**
 * This is the Diminuto mutex object.
 */
typedef struct DiminutoMutex {
    pthread_mutex_t     mutex;      /**< POSIX mutual exclusion object. */
    pthread_mutexattr_t attribute;  /**< POSIX mutual exclusion attribute. */
} diminuto_mutex_t;

#if !defined(PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP)
#   warning PTHREAD_RECURSIVE_MUTEX_INITIALZER_NP not defined on this platform!
#   define PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP PTHREAD_MUTEX_INITIALIZER
#endif

/**
 * @def DIMINUTO_MUTEX_INITIALIZER
 * This is a static initializer for a Diminuto mutex object.
 * N.B. if the non-portable GNU-specific initializer for recursive
 * mutexn is available, it is used; otherwise the mutex is not
 * recursive. If this is not desirable, the initialization function
 * should be used instead.
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
 * Attempt to lock a Diminuto mutex object. BUSY is returned
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
 * GENERATORS
 **********************************************************************/

/**
 * @def DIMINUTO_MUTEX_BEGIN
 * Begin a code section that is serialized using a Diminuto mutex
 * specified by the caller as a pointer in the argument @a _MP_ by
 * locking the mutex.
 */
#define DIMINUTO_MUTEX_BEGIN(_MP_) \
    do { \
        diminuto_mutex_t * diminuto_mutex_p = (diminuto_mutex_t *)0; \
        diminuto_mutex_p = (_MP_); \
        if (diminuto_mutex_lock(diminuto_mutex_p) == 0) { \
            pthread_cleanup_push(diminuto_mutex_cleanup, diminuto_mutex_p); \
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
        diminuto_mutex_t * diminuto_mutex_p = (diminuto_mutex_t *)0; \
        diminuto_mutex_p = (_MP_); \
        if (diminuto_mutex_lock_try(diminuto_mutex_p) == 0) { \
            pthread_cleanup_push(diminuto_mutex_cleanup, diminuto_mutex_p); \
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
        diminuto_mutex_p = (diminuto_mutex_t *)0; \
    } while (0)

#endif
