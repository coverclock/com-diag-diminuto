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
 */

/***********************************************************************
 *
 **********************************************************************/

#include <signal.h>
#include "com/diag/diminuto/diminuto_types.h"
#include "com/diag/diminuto/diminuto_condition.h"

/***********************************************************************
 *
 **********************************************************************/

static const int DIMINUTO_THREAD_SIGNAL = SIGUSR1;

static const diminuto_ticks_t DIMINUTO_THREAD_INFINITY = ~(diminuto_ticks_t)0;

static const int DIMINUTO_THREAD_TIMEDOUT = ETIMEDOUT;

/***********************************************************************
 *
 **********************************************************************/

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
 *
 **********************************************************************/

extern diminuto_thread_t * diminuto_thread_instance(void);

extern pthread_t diminuto_thread_self(void);

extern int diminuto_thread_yield();

extern int diminuto_thread_notified(void);

extern void diminuto_thread_exit(void * vp);

extern diminuto_thread_t * diminuto_thread_init(diminuto_thread_t * tp, void * (*fp)(void *));

extern diminuto_thread_t * diminuto_thread_fini(diminuto_thread_t * tp);

static inline int diminuto_thread_lock(diminuto_thread_t * tp)
{
    return diminuto_condition_lock(&(tp->condition));
}

static inline int diminuto_thread_lock_try(diminuto_thread_t * tp)
{
    return diminuto_condition_lock_try(&(tp->condition));
}

static inline int diminuto_thread_unlock(diminuto_thread_t * tp)
{
    return diminuto_condition_unlock(&(tp->condition));
}

extern void diminuto_thread_cleanup(void * vp);

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

static inline int diminuto_thread_wait_until(diminuto_thread_t * tp, diminuto_ticks_t clocktime)
{
    return diminuto_condition_wait_until(&(tp->condition), clocktime);
}

static inline int diminuto_thread_wait(diminuto_thread_t * tp)
{
    return diminuto_thread_wait_until(tp, DIMINUTO_CONDITION_INFINITY);
}

static inline int diminuto_thread_signal(diminuto_thread_t * tp)
{
    return diminuto_condition_signal(&(tp->condition));
}

extern int diminuto_thread_start(diminuto_thread_t * tp, void * cp);

extern int diminuto_thread_notify(diminuto_thread_t * tp);

extern int diminuto_thread_join_until(diminuto_thread_t * tp, void ** vpp, diminuto_ticks_t clocktime);

static inline int diminuto_thread_join(diminuto_thread_t * tp, void ** vpp)
{
    return diminuto_thread_join_until(tp, vpp, DIMINUTO_THREAD_INFINITY);
}

/***********************************************************************
 *
 **********************************************************************/

#define DIMINUTO_THREAD_BEGIN(_TP_) DIMINUTO_CONDITION_BEGIN(&((_TP_)->condition))

#define DIMINUTO_THREAD_TRY(_TP_) DIMINUTO_CONDITION_TRY(&((_TP_)->condition))

#define DIMINUTO_THREAD_END DIMINUTO_CONDITION_END

/***********************************************************************
 *
 **********************************************************************/

#endif
