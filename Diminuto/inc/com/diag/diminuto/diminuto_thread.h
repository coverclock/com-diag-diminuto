/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_THREAD_
#define _H_COM_DIAG_DIMINUTO_THREAD_

/**
 * @file
 *
 * Copyright 2020 Digital Aggregates Corporation, Colorado, USA.<BR>
 * Licensed under the terms in LICENSE.txt.<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 * This module creates a framework the implements POSIX thread
 * operations using a very specific model of behavior: all threads
 * contain a dedicated condition (and therefore a dedicated mutex);
 * all threads allow deferred cancellation (but discourage its use
 * by not providing functions that use it); all threads have a
 * synchronized notification facility that can use a kill signal to
 * unblock them from a system call; and all threads signal their
 * condition when their state changes.
 */

/***********************************************************************
 * PREREQUISITES
 **********************************************************************/

#include <signal.h>
#include "com/diag/diminuto/diminuto_types.h"
#include "com/diag/diminuto/diminuto_condition.h"

/***********************************************************************
 * CONSTANTS
 **********************************************************************/

/**
 * This is the kill signal used to try to unblock a thread from a
 * system call when it is notified.
 */
static const int DIMINUTO_THREAD_SIGNAL = SIGUSR1;

/**
 * This value when used as a clocktime specifies that the caller blocks
 * indefinitely.
 */
static const diminuto_ticks_t DIMINUTO_THREAD_INFINITY = ~(diminuto_ticks_t)0;

/**
 * This is the error number returned when the caller joins with a
 * thread and the clocktime is reached without being completed.
 */
static const int DIMINUTO_THREAD_TIMEDOUT = ETIMEDOUT;

/***********************************************************************
 * TYPES
 **********************************************************************/

/**
 * These are the states in which a Diminuto thread object may be. The
 * ALLOCATED state is only useful if the caller zeros out the object
 * (for example, at compile time).
 */
typedef enum DiminutoThreadState {
    DIMINUTO_THREAD_STATE_ALLOCATED     = '\0',     /* thread object allocated (if zeroed) */
    DIMINUTO_THREAD_STATE_INITIALIZED   = 'I',      /* thread object init performed */
    DIMINUTO_THREAD_STATE_STARTED       = 'S',      /* thread object start performed */
    DIMINUTO_THREAD_STATE_RUNNING       = 'R',      /* thread function running */
    DIMINUTO_THREAD_STATE_COMPLETING    = 'C',      /* thread function completing */
    DIMINUTO_THREAD_STATE_JOINED        = 'J',      /* thread object join performed */
    DIMINUTO_THREAD_STATE_FINALIZED     = 'F',      /* thread object fini performed */
    DIMINUTO_THREAD_STATE_FAILED        = 'X',      /* thread object start failed */
} diminuto_thread_state_t;

/**
 * This is the Diminuto thread object.
 */
typedef struct DiminutoThread {
    diminuto_condition_t condition;     /* Diminuto condition object */
    pthread_t thread;                   /* POSIX Thread thread object */
    void * (*function)(void *);         /* pointer to thread function implementation */
    void * context;                     /* pointer to thread function context */
    void * value;                       /* final thread function value */
    diminuto_thread_state_t state;      /* Diminuto thread state */
    int notification;                   /* kill signal used for notification or 0 if none */
    int8_t notifying;                   /* True if notification performed */
} diminuto_thread_t;

/**
 * @def DIMINUTO_THREAD_INITIALIZER
 * This is a static initializer for the Diminuto thread object.
 */
#define DIMINUTO_THREAD_INITIALIZER(_FP_) \
    { \
        DIMINUTO_CONDITION_INITIALIZER, \
        0, \
        (_FP_), \
        (void *)0, \
        (void *)(~0), \
        DIMINUTO_THREAD_STATE_INITIALIZED, \
        DIMINUTO_THREAD_SIGNAL, \
        0, \
    }

/***********************************************************************
 * ACTIONS
 **********************************************************************/

/**
 * Returns a pointer to the Diminuto thread object associated with
 * the calling thread.
 * @return a pointer to the object.
 */
extern diminuto_thread_t * diminuto_thread_instance(void);

/**
 * Returns the opaque value of the POSIX thread identity associated with
 * the calling thread.
 * @return a POSIX thread identity.
 */
extern pthread_t diminuto_thread_self(void);

/**
 * Yield the processor from the calling thread to another ready thread.
 * @return 0 or an error code of the yield failed.
 */
extern int diminuto_thread_yield();

/**
 * Return true if the calling thread has been notified, false otherwise.
 * @return true if the caller has been notified, false otherwise.
 */
extern int diminuto_thread_notified(void);

/**
 * Cause the calling thread to exit.
 * @param vp is the final value of the calling thread.
 */
extern void diminuto_thread_exit(void * vp);

/***********************************************************************
 * INITIALIZERS AND FINALIZERS
 **********************************************************************/

/**
 * Initialize a Diminuto thread object. Allocate any resources.
 * The thread is not started.
 * @param tp points to the object.
 * @param fp points to the function to be associated with the object.
 * @return a pointer to the object or NULL if initialization failed.
 */
extern diminuto_thread_t * diminuto_thread_init(diminuto_thread_t * tp, void * (*fp)(void *));

/**
 * Finalize a Diminuto thread object. Free any resources. The thread
 * must not be in the STARTED or RUNNING states.
 * @param tp points to the object.
 * @return NULL or a pointer to the object if finalization failed.
 */
extern diminuto_thread_t * diminuto_thread_fini(diminuto_thread_t * tp);

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
static inline diminuto_sticks_t diminuto_thread_clock(void)
{
    return diminuto_condition_clock();
}

/***********************************************************************
 * EXTENSIONS
 **********************************************************************/

/**
 * Lock the Diminuto mutex associated with the Diminuto condition
 * associated with a Diminuto thread.
 * @param tp points to the object.
 * @return 0 or an error code if the lock failed.
 */
static inline int diminuto_thread_lock(diminuto_thread_t * tp)
{
    return diminuto_condition_lock(&(tp->condition));
}

/**
 * Attempt to lock the Diminuto mutex associated with the Diminuto
 * condition associated with a Diminuto thread object. EBUSY is
 * returned (and no error message is generated) if the mutex was
 * already locked.
 * @param tp points to the object.
 * @return 0 or an error code if the lock failed.
 */
static inline int diminuto_thread_lock_try(diminuto_thread_t * tp)
{
    return diminuto_condition_lock_try(&(tp->condition));
}

/**
 * Unlock the Diminuto mutex associated with the Diminuto condition
 * associated with a Diminuto thread object.
 * @param tp points to the object.
 * @return 0 or an error code if the unlock failed.
 */
static inline int diminuto_thread_unlock(diminuto_thread_t * tp)
{
    return diminuto_condition_unlock(&(tp->condition));
}

/**
 * Block the calling thread on the Diminuto condition assoociated with
 * a Diminuto thread object until either the condition is signalled or
 * the absolute clock time is reached. ETIMEDOUT is returned if the
 * absolute clock time was reached before the condition was signaled.
 * If the clock time is INFINITY, the caller blocks indefinitely.
 * @param tp points to the object.
 * @param clocktime is the absolute clock time in Diminuto ticks.
 * @return 0 or an error code if the wait failed.
 */
static inline int diminuto_thread_wait_until(diminuto_thread_t * tp, diminuto_ticks_t clocktime)
{
    return diminuto_condition_wait_until(&(tp->condition), clocktime);
}

/**
 * Block the calling thread on a Diminuto condition associated with
 * a Diminuto thread object until the condition is signalled.
 * @param tp points to the object.
 * @return 0 or an error code if the wait failed.
 */
static inline int diminuto_thread_wait(diminuto_thread_t * tp)
{
    return diminuto_thread_wait_until(tp, DIMINUTO_CONDITION_INFINITY);
}

/**
 * Broadcast a signal waking up all threads (if any) waiting on the
 * Diminuto condition associated with a Diminuto thread object.
 * @param tp points to the object.
 * @return 0 or an error code if the signal failed.
 */
static inline int diminuto_thread_signal(diminuto_thread_t * tp)
{
    return diminuto_condition_signal(&(tp->condition));
}

/***********************************************************************
 * OPERATIONS
 **********************************************************************/

/**
 * Start a new POSIX thread by executing the function associated with
 * a Diminuto thread object. Pass the specified context pointer
 * (whose usage is user defined) to the function.
 * @param tp points to the object.
 * @param cp points to the context.
 * @return 0 or an error code of the start failed.
 */
extern int diminuto_thread_start(diminuto_thread_t * tp, void * cp);

/**
 * Notify a Diminuto thread object. The Diminuto signal associated with
 * the object will be signalled. If enabled, a kill signal will be sent
 * to the associated thread, possibly unblocking it from a system call
 * by interrupting it.
 * @param tp points to the object.
 * @return 0 or an error code if the notify failed.
 */
extern int diminuto_thread_notify(diminuto_thread_t * tp);

/**
 * Block until the POSIX thread associated with a Diminuto thread
 * object returns or exits, or until the absolute clock time is
 * reached. ETIMEDOUT is returned if the absolute clock time was
 * reached before the join was completed. If the absolute clock time
 * is INFINITY, the caller blocks indeefinitely. If the join was
 * comnpleted, the final value returned by the thread is placed in
 * a value-result parameter.
 * @param tp points to the object.
 * @param vpp points to a variable into whcih the final value is placed.
 * @param clocktime is the absolute clock time.
 * @return 0 or an error code if the join failed.
 */
extern int diminuto_thread_join_until(diminuto_thread_t * tp, void ** vpp, diminuto_ticks_t clocktime);

/**
 * Block until the POSIX thread associated with a Diminuto thread
 * object returns or exits. If the join was comnpleted, the final value
 * returned by the thread is placed in a value-result parameter.
 * @param tp points to the object.
 * @param vpp points to a variable into whcih the final value is placed.
 * @return 0 or an error code if the join failed.
 */
static inline int diminuto_thread_join(diminuto_thread_t * tp, void ** vpp)
{
    return diminuto_thread_join_until(tp, vpp, DIMINUTO_THREAD_INFINITY);
}

/***********************************************************************
 * CALLBACKS
 **********************************************************************/

/**
 * This is a callback used to unlock the Diminuto mutex associated with
 * the Diminuto condition associated with a Diminuto thread object in the
 * event of a cancellation.
 * @param vp points to the object.
 */
extern void diminuto_thread_cleanup(void * vp);

/***********************************************************************
 * MACROS
 **********************************************************************/

/**
 * @def DIMINUTO_THREAD_BEGIN
 * Begin a code section that is serialized using a Diminuto thread
 * specified by the caller as a pointer in the argument @a _TP_ by
 * locking the thread condition mutex.
 */
#define DIMINUTO_THREAD_BEGIN(_TP_) DIMINUTO_CONDITION_BEGIN(&((_TP_)->condition))

/**
 * @def DIMINUTO_THREAD_TRY
 * Conditionally begin a code section that is serialized using a Diminuto
 * thread specified by the caller as a pointer in the argument @a _TP_
 * by locking the thread condition mutex.
 */
#define DIMINUTO_THREAD_TRY(_TP_) DIMINUTO_CONDITION_TRY(&((_TP_)->condition))

/**
 * @def DIMINUTO_THREAD_END
 * End a code section that was serialized using the Diminuto thread
 * specified at the beginning of the block by unlocking the thread
 * condition mutex.
 */
#define DIMINUTO_THREAD_END DIMINUTO_CONDITION_END

#endif
