/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_CONDITION_
#define _H_COM_DIAG_DIMINUTO_CONDITION_

/**
 * @file
 *
 * Copyright 2020 Digital Aggregates Corporation, Colorado, USA.<BR>
 * Licensed under the terms in LICENSE.txt.<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 * This module creates a framework the implements POSIX condition
 * operations using a very specific model of behavior: all conditions
 * contain a dedicated mutex that is used to access them (this may
 * not be appropriate for all algorithms); all signals are broadcast.
 */

/***********************************************************************
 * PREREQUISITES
 **********************************************************************/

#include "com/diag/diminuto/diminuto_types.h"
#include "com/diag/diminuto/diminuto_time.h"
#include "com/diag/diminuto/diminuto_mutex.h"

/***********************************************************************
 * CONSTANTS
 **********************************************************************/

/**
 * This value when used as a clocktime specifies that the caller blocks
 * indefinitely.
 */
static const diminuto_ticks_t DIMINUTO_CONDITION_INFINITY = ~(diminuto_ticks_t)0;

/**
 * This is the error number returned when the caller waits on a
 * condition and the clocktime is reached without being signalled.
 */
static const int DIMINUTO_CONDITION_TIMEDOUT = ETIMEDOUT;

/***********************************************************************
 * TYPES
 **********************************************************************/

/**
 * This is the Diminuto condition object.
 */
typedef struct DiminutoCondition {
    diminuto_mutex_t mutex;             /* Diminuto mutual exclusion object */
    pthread_cond_t condition;           /* POSIX Thread condition object */
} diminuto_condition_t;

/**
 * @def DIMINUTO_CONDITION_INITIALIZER
 * This is a static initializer for the Diminuto condition object.
 */
#define DIMINUTO_CONDITION_INITIALIZER \
    { \
        DIMINUTO_MUTEX_INITIALIZER, \
        PTHREAD_COND_INITIALIZER, \
    }

/***********************************************************************
 * INITIALIZERS AND FINALIZERS
 **********************************************************************/

/**
 * Initialize a Diminuto condition object. Allocate any resources.
 * @param cp points to the object.
 * @return a pointer to the object or NULL if initialization failed.
 */
extern diminuto_condition_t * diminuto_condition_init(diminuto_condition_t * cp);

/**
 * Finalize a Diminuto condition object. Release any resources.
 * @param cp points to the object.
 * @return NULL or a pointer to the object finalization if failed.
 */
extern diminuto_condition_t * diminuto_condition_fini(diminuto_condition_t * cp);

/***********************************************************************
 * HELPERS
 **********************************************************************/

/**
 * Return the system clock time in Coordinated Universal Time (UTC) in
 * ticks since the Epoch (shown here in ISO8601 format)
 * 1970-01-01T00:00:00+0000.
 * @return the number of ticks elapsed since the Epoch or -1 with
 * errno set if an error occurred.
 */
static inline diminuto_sticks_t diminuto_condition_clock(void)
{
    return diminuto_time_clock();
}

/***********************************************************************
 * EXTENSIONS
 **********************************************************************/

/**
 * Lock the Diminuto mutex associated with a Diminuto condition object.
 * @param cp points to the object.
 * @return 0 or an error code if the lock failed.
 */
static inline int diminuto_condition_lock(diminuto_condition_t * cp)
{
    return diminuto_mutex_lock(&(cp->mutex));
}

/**
 * Attempt to lock the Diminuto mutex associated with a Diminuto condition
 * object. EBUSY is returned (and no error message is generated) if the
 * mutex was already locked.
 * @param cp points to the object.
 * @return 0 or an error code if the lock failed.
 */
static inline int diminuto_condition_lock_try(diminuto_condition_t * cp)
{
    return diminuto_mutex_lock_try(&(cp->mutex));
}

/**
 * Unlock the Diminuto mutex associated with a Diminuto condition object.
 * @param cp points to the object.
 * @return 0 or an error code if the unlock failed.
 */
static inline int diminuto_condition_unlock(diminuto_condition_t * cp)
{
    return diminuto_mutex_unlock(&(cp->mutex));
}

/***********************************************************************
 * OPERATIONS
 **********************************************************************/

/**
 * Block the calling thread on a Diminuto condition object until
 * either the condition is signalled or the absolute clock time is
 * reached. ETIMEDOUT is returned if the absolute clock time was
 * reached before the condition was signaled. If the absolute clock
 * time is INFINITY, the caller waits indefinitely.
 * @param cp points to the object.
 * @param clocktime is the absolute clock time in Diminuto ticks.
 * @return 0 or an error code if the wait failed.
 */
extern int diminuto_condition_wait_until(diminuto_condition_t * cp, diminuto_ticks_t clocktime);

/**
 * Block the calling thread on a Diminuto condition object until
 * the condition is signalled.
 * @param cp points to the object.
 * @return 0 or an error code if the wait failed.
 */
static inline int diminuto_condition_wait(diminuto_condition_t * cp)
{
    return diminuto_condition_wait_until(cp, DIMINUTO_CONDITION_INFINITY);
}

/**
 * Broadcast a signal waking up all threads (if any) waiting on a
 * Diminuto condition object.
 * @param cp points to the object.
 * @return 0 or an error code if the signal failed.
 */
extern int diminuto_condition_signal(diminuto_condition_t * cp);

/***********************************************************************
 * CALLBACKS
 **********************************************************************/

/**
 * This is a callback used to unlock the Diminuto mutex associated with
 * a Diminuto condition object in the event of a cancellation.
 * @param vp points to the object.
 */
extern void diminuto_condition_cleanup(void * vp);

/***********************************************************************
 * GENERATORS
 **********************************************************************/

/**
 * @def DIMINUTO_CONDITION_BEGIN
 * Begin a code section that is serialized using a Diminuto condition
 * specified by the caller as a pointer in the argument @a _CP_ by
 * locking the condition mutex.
 */
#define DIMINUTO_CONDITION_BEGIN(_CP_) DIMINUTO_MUTEX_BEGIN(&((_CP_)->mutex))

/**
 * @def DIMINUTO_CONDITION_TRY
 * Conditionally begin a code section that is serialized using a Diminuto
 * condition specified by the caller as a pointer in the argument @a _CP_
 * by locking the condition mutex.
 */
#define DIMINUTO_CONDITION_TRY(_CP_) DIMINUTO_MUTEX_TRY(&((_CP_)->mutex))

/**
 * @def DIMINUTO_CONDITION_END
 * End a code section that was serialized using the Diminuto condition
 * specified at the beginning of the block by unlocking the condition
 * mutex.
 */
#define DIMINUTO_CONDITION_END DIMINUTO_MUTEX_END

#endif
