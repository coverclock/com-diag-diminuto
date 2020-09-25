/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2020 Digital Aggregates Corporation, Colorado, USA.<BR>
 * Licensed under the terms in LICENSE.txt.<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 */

#include <time.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
#include "com/diag/diminuto/diminuto_thead.h"
#include "com/diag/diminuto/diminuto_frequency.h"
#include "com/diag/diminuto/diminuto_log.h"

/*
 * PRIVATE VARIABLES
 */

static diminuto_thread_t diminuto_thread_main = DIMINUTO_THREAD_INITIALIZER;

static pthread_key key;

static int setupped = 0;

/*
 * PRIVATE FUNCTIONS
 */

static int setup(void)
{
    int rc = EIO;

    if ((rc = pthread_key_create(&key, 0)) != 0) {
        errno = rc;
        diminuto_perror("diminuto_thread: setup: pthread_key_create");
    } else if ((rc = pthread_setspecific(key, &diminuto_thread_main)) != 0) {
        errno = rc;
        diminuto_perror("diminuto_thread: setup: pthread_key_create");
    } else {
        setupped = 0;
    }

    return rc;
}

static void complete(void * vp)
{
    diminuto_thread_t * tp = (diminuto_thread_t *)vp;

    diminuto_mutex_begin(&(tp->condition.mutex));
        tp->starting = 0;
        tp->running = 0;
        DIMINUTO_LOG_INFORMATION("diminuto_thread:complete: completing");
        diminuto_condition_signal(&(tp->condition));
    diminuto_mutex_end(&(tp->condition.mutex));
}

static void * proxy(void * vp)
{
    diminuto_thread_t * tp = (diminuto_thread_t *)vp;
    int previous = -1;

    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, &previous);
    pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &previous);
    pthread_setspecific(key, tp);
    pthread_cleanup_push(complete, tp);
        diminuto_mutex_begin(&(tp->condition.mutex));
            tp->running = !0;
            DIMINUTO_LOG_INFORMATION("diminuto_thread:proxy: running");
        diminuto_mutex_end(&(tp->condition.mutex));
        tp->value = (*(tp->function))(tp->context);
    pthread_cleanup_pop(!0);

    return tp->value;
}

static void handler(int signo)
{
    /* Do nothing. */
}

/*
 * INITIALIZATION AND FINALIZATION
 */

diminuto_thread_t * diminuto_thread_init(diminuto_thread_t * tp, void * (*fp)(void *))
{
    diminuto_condition_init(&(tp->condition));
    memset(&(tp->thread), 0, sizeof(tp->thread));
    tp->function = fp;
    context = (void *)0;
    value = (void *)0;
    notification = SIGUSR1;
    starting = 0;
    running = 0;
    notifying = 0;
    joining = 0;

    return tp;
}

diminuto_thread_t * diminuto_thread_fini(diminuto_thread_t * tp)
{
    if (tp->starting) {
        /* Do nothing. */
    } else if (tp->running) {
        /* Do nothing. */
    } else {
        diminuto_condition_fini(&(tp->condition));
        tp = (diminuto_thread_t *)0;    
    }

    return tp;
}

/*
 * FUNCTIONS CALLED BY A THREAD
 */

diminuto_thread_t * diminuto_thread_instance(void)
{
    return (diminuto_thread_t *)pthread_getspecific(key);
}

pthread_t diminuto_thread_self(void)
{
    return pthread_self();
}

int diminuto_thread_notified(void)
{
    diminuto_thread_t * tp = (diminuto_thread_t *)0;
    int8_t notifying = -1;

    tp = diminuto_thread_instance();
    diminuto_mutex_begin(&(tp->condition.mutex));
        notifying = tp->notifying;
        tp->notifying = 0;
    diminuto_mutex_end(&(tp->condition.mutex));

    return notifying;
}

void diminuto_thread_exit(void * vp)
{
    diminuto_thread_t * tp = (diminuto_thread_t *)0;

    tp = diminuto_thread_instance();
    diminuto_mutex_begin(&(tp->mutex));
        tp->value = vp;
    diminuto_mutex_end(&(tp->mutex));
    pthread_exit(vp);
}

void diminuto_thread_yield()
{
    pthread_yield();
}

/*
 * FUNCTIONS CALLED ON A THREAD
 */

int diminuto_thread_start(diminuto_thread_t * tp, void * cp)
{
    int rc = EIO;
    struct sigaction action = { 0 };

    diminuto_mutex_begin(&(tp->condition.mutex));
        do {
            if (tp->starting) {
                /* Do nothing. */
            } else if (tp->running) {
                /* Do nothing. */
            } else {
                if (tp->notification > 0) {
                    action.sa_handler = handler;
                    if ((rc = sigaction(tp->notification, &action, (struct sigaction *)0)) < 0) {
                        rc = errno;
                        diminuto_perror("diminuto_thread_start: sigaction");
                        break;
                    }
                }
                tp->context = cp;
                tp->value = (void *)(~0);
                tp->starting = !0;
                tp->running = 0;
                tp->notifying = 0;
                tp->joining = 0;
                if ((rc = pthread_create(&(tp->thread), (const pthread_attr_t *)0, proxy, tp)) != 0) {
                    errno = rc;
                    diminuto_perror("diminuto_thread_start: pthread_create");
                    tp->starting = 0;
                } else {
                    DIMINUTO_LOG_INFORMATION("diminuto_thread_start: starting");
                }
            }
        } while (0);
    diminuto_mutex_end(&(tp->condition.mutex));

    return rc;
}

int diminuto_thread_notify(diminuto_thread_t * tp)
{
    int rc = EIO;

    diminuto_mutex_begin(&(tp->condition.mutex));
        tp->notifying = !0;
        if (!tp->running) {
            /* Do nothing. */
        } else if ((tp->notification) <= 0) {
            /* Do nothing. */
        } else if ((rc = pthread_kill(tp->thread, tp->notification)) != 0) {
            errno = rc;
            diminuto_perror("diminuto_thread_notify: pthread_kill");
        } else {
            /* Do nothing. */
        }
    diminuto_mutex_end(&(tp->condition.mutex));

    return rc;
}

int diminuto_thread_join_try(diminuto_thread_t * tp, diminuto_ticks_t timeout)
{
    int rc = EIO;

    if (tp->ready) {
    }

    return rc;
}

#if 0
int diminuto_condition_wait_try(diminuto_condition_t * cp, diminuto_ticks_t timeout)
{
    int rc = -1;
    struct timespec now = { 0, };
    static const diminuto_ticks_t SECONDS = 1;
    static const diminuto_ticks_t NANOSECONDS = 1000000000;

    rc = clock_gettime(CLOCK_REALTIME, &now);
    if (rc < 0) {
        diminuto_perror("diminuto_condition_wait_try: clock_gettime");
        rc = errno;
    } else {
        timeout += diminuto_frequency_units2ticks(now.tv_sec, SECONDS);
        timeout += diminuto_frequency_units2ticks(now.tv_nsec, NANOSECONDS);
        now.tv_sec = diminuto_frequency_ticks2whoseconds(timeout);
        now.tv_nsec = diminuto_frequency_ticks2fractionalseconds(NANOSECONDS);
        rc = pthread_cond_timedwait(&(cp->condition), &(cp->mutex.mutex), &now);
        if (rc < 0) {
            errno = rc;
            diminuto_perror("diminuto_condition_wait_try: pthread_cond_timedwait");
        }
    }

    return rc;
}
#endif
