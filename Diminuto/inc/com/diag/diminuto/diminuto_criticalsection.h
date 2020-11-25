/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_CRITICALSECTION_
#define _H_COM_DIAG_DIMINUTO_CRITICALSECTION_

/**
 * @file
 * @copyright Copyright 2013-2020 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief Implements critical section begin and end operators.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 *
 * The Critical Section feature implements a critical section of
 * code using a POSIX pthread mutex. The mutex is automatically
 * unlocked if the thread cleanup action is invoked.
 */

#include <pthread.h>

/**
 * This is the thread cleanup action.
 * @param voidp is the pointer POSIX passes to the cleanup function.
 */
extern void diminuto_criticalsection_cleanup(void * voidp);

/**
 * @def DIMINUTO_CRITICAL_SECTION_BEGIN
 * Begin a code section that is serialized using a pthread mutex specified by
 * the caller as a pointer in the argument @a _MUTEXP_ by locking the mutex.
 */
#define DIMINUTO_CRITICAL_SECTION_BEGIN(_MUTEXP_) \
    do { \
        pthread_mutex_t * diminuto_criticalsection_mutexp = (pthread_mutex_t *)0; \
        int diminuto_criticalsection_rc = -1; \
        diminuto_criticalsection_mutexp = (_MUTEXP_); \
        diminuto_criticalsection_rc = pthread_mutex_lock(diminuto_criticalsection_mutexp); \
        if (diminuto_criticalsection_rc == 0) { \
            pthread_cleanup_push(diminuto_criticalsection_cleanup, diminuto_criticalsection_mutexp); \
            do { \
                do { } while (0)

/**
 * @def DIMINUTO_CRITICAL_SECTION_TRY
 * Conditionally begin a code section that is serialized using a pthread
 * mutex specified by the caller as a pointer in the argument @a _MUTEXP_
 * by locking the mutex.
 */
#define DIMINUTO_CRITICAL_SECTION_TRY(_MUTEXP_) \
    do { \
        pthread_mutex_t * diminuto_criticalsection_mutexp = (pthread_mutex_t *)0; \
        int diminuto_criticalsection_rc = -1; \
        diminuto_criticalsection_mutexp = (_MUTEXP_); \
        diminuto_criticalsection_rc = pthread_mutex_trylock(diminuto_criticalsection_mutexp); \
        if (diminuto_criticalsection_rc == 0) { \
            pthread_cleanup_push(diminuto_criticalsection_cleanup, diminuto_criticalsection_mutexp); \
            do { \
                do { } while (0)

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
