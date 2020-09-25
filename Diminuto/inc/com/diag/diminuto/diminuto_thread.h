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
 */

#include <signal.h>
#include "com/diag/diminuto/diminuto_condition.h"

static const int DIMINUTO_THREAD_NOTIFICATION = SIGUSR1;

static const diminuto_ticks_t DIMINUTO_THREAD_INFINITE = ~(diminuto_ticks_t)0;

typedef struct DiminutoThread {
    diminuto_condition_t condition;
    pthread_t thread;
    void * (*function)(void *);
    void * context;
    void * value;
    int notification;
    int8_t starting;
    int8_t running;
    int8_t notifying;
    int8_t joining;
} diminuto_thread_t;

#define DIMINUTO_THREAD_INITIALIZER \
    { \
        DIMINUTO_CONDITION_INITIALIZER, \
        { 0, }, \
        (void * (*)(void*))0, \
        (void *)0, \
        (void *)(~0), \
        0, \
        0, \
        0, \
        0, \
        0, \
    }

extern diminuto_thread_t * diminuto_thread_instance(void);

extern pthread_t diminuto_thread_self(void);

extern int diminuto_thread_notified(void);

extern void diminuto_thread_exit(void * vp);

extern diminuto_thread_t * diminuto_thread_init(diminuto_thread_t * tp, void * (*fp)(void *));

extern void diminuto_thread_yield();

extern int diminuto_thread_start(diminuto_thread_t * tp, void * cp);

extern int diminuto_thread_notify(diminuto_thread_t * tp);

extern int diminuto_thread_join_try(diminuto_thread_t * tp, diminuto_ticks_t timeout);

static inline int diminuto_thread_join(diminuto_thread_t * tp)
{
    return diminuto_thread_join_try(tp, DIMINUTO_THREAD_INFINITE);
}

extern diminuto_thread_t * diminuto_thread_fini(diminuto_thread_t * tp);

#endif
