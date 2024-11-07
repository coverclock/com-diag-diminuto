/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_CRITICALSECTION_
#define _H_COM_DIAG_DIMINUTO_CRITICALSECTION_

/**
 * @file
 * @copyright Copyright 2013-2024 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief Implements critical section begin and end operators.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 *
 * The Critical Section feature implements a critical section of
 * code using a POSIX pthread mutex in a set of bracketing macros.
 * The mutex is automatically unlocked if the thread cleanup action
 * is invoked.
 */

#include "com/diag/diminuto/diminuto_assert.h"
#include <pthread.h>

/**
 * Lock a POSIX mutex.
 * @param mp points to the mutex.
 * @return 0 for success, <0 with errno set if an error occurred.
 */
extern int diminuto_criticalsection_lock(pthread_mutex_t * mp);

/**
 * Try to lock a POSIX mutex. Return <0 with errno set to EBUSY if
 * the mutex was already locked.
 * @param mp points to the mutex.
 * @return 0 for success, <0 with errno set if an error occurred.
 */
extern int diminuto_criticalsection_trylock(pthread_mutex_t * mp);

/**
 * This is the thread cleanup callback.
 * @param vp is the pointer POSIX passes to the cleanup function.
 */
extern void diminuto_criticalsection_cleanup(void * vp);

/**
 * @def DIMINUTO_CRITICAL_SECTION_BEGIN
 * Begin a code section that is serialized using a pthread mutex specified by
 * the caller as a pointer in the argument @a _MP_ by locking the mutex.
 */
#define DIMINUTO_CRITICAL_SECTION_BEGIN(_MP_) \
    do { \
        pthread_mutex_t * _diminuto_criticalsection_mp; \
        _diminuto_criticalsection_mp = (_MP_); \
        diminuto_contract(diminuto_criticalsection_lock(_diminuto_criticalsection_mp) == 0); \
        if (!0) { \
            pthread_cleanup_push(diminuto_criticalsection_cleanup, _diminuto_criticalsection_mp); \
            do { \
                ((void)0)

/**
 * @def DIMINUTO_CRITICAL_SECTION_TRY
 * Conditionally begin a code section that is serialized using a pthread
 * mutex specified by the caller as a pointer in the argument @a _MP_
 * by locking the mutex. Applications can use their own variable that
 * changes state within the critical section to determine if the lock
 * succeeded or failed.
 */
#define DIMINUTO_CRITICAL_SECTION_TRY(_MP_) \
    do { \
        pthread_mutex_t * _diminuto_criticalsection_mp; \
        _diminuto_criticalsection_mp = (_MP_); \
        if (diminuto_criticalsection_trylock(_diminuto_criticalsection_mp) == 0) { \
            pthread_cleanup_push(diminuto_criticalsection_cleanup, _diminuto_criticalsection_mp); \
            do { \
                ((void)0)

/**
 * @def DIMINUTO_CRITICAL_SECTION_END
 * End a code section that was serialized using the pthread mutex specified at
 * the beginning of the block by unlocking the mutex.
 */
#define DIMINUTO_CRITICAL_SECTION_END \
            } while (0); \
            pthread_cleanup_pop(!0); \
        } \
    } while (0)

#endif
