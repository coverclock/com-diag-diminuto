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
#include <pthread.h>
#include "com/diag/diminuto/diminuto_thread.h"
#include "com/diag/diminuto/diminuto_criticalsection.h"
#include "com/diag/diminuto/diminuto_coherentsection.h"
#include "com/diag/diminuto/diminuto_frequency.h"
#include "com/diag/diminuto/diminuto_log.h"

/*
 * PRIVATE VARIABLES
 */

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

static diminuto_thread_t diminuto_thread_main = DIMINUTO_THREAD_INITIALIZER;

static pthread_key key;

static int setupped = 0;

/*
 * PRIVATE FUNCTIONS
 */

static int setup(void)
{
    int rc = EIO;

    DIMINUTO_COHERENT_SECTION_BEGIN;
        if (!setupped) {
            DIMINUTO_CRITICAL_SECTION_BEGIN(&mutex);
                if (!setupped) {
                    /* Do nothing. */
                } else if ((rc = pthread_key_create(&key, 0)) != 0) {
                    errno = rc;
                    diminuto_perror("diminuto_thread: setup: pthread_key_create");
                } else if ((rc = pthread_setspecific(key, &diminuto_thread_main)) != 0) {
                    errno = rc;
                    diminuto_perror("diminuto_thread: setup: pthread_setspecific");
                } else {
                    diminuto_thread_init(&diminuto_thread_main);
                    diminuto_thread_main.thread = pthread_self();
                    diminuto_thread_main.state = DIMINUTO_THREAD_STATE_RUNNING;
                    setupped = !0;
                    DIMINUTO_LOG_DEBUG("diminuto_thread:setup: setup");
                }
            DIMINUTO_CRITICAL_SECTION_END(&mutex);
        }
    DIMINUTO_COHERENT_SECTION_END;

    return rc;
}

static void complete(void * vp)
{
    diminuto_thread_t * tp = (diminuto_thread_t *)vp;

    diminuto_condition_lock(&(tp->condition));
        tp->state = DIMINUTO_THREAD_STATE_COMPLETED;
        DIMINUTO_LOG_DEBUG("diminuto_thread:complete: completed");
        diminuto_condition_signal(&(tp->condition));
    diminuto_condition_unlock(&(tp->condition));
}

static void * proxy(void * vp)
{
    diminuto_thread_t * tp = (diminuto_thread_t *)vp;
    int previous = -1;

    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, &previous);
    pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &previous);
    pthread_setspecific(key, tp);
    pthread_cleanup_push(complete, tp);
        diminuto_condition_lock(&(tp->condition));
            tp->state = DIMINUTO_THREAD_STATE_RUNNING;;
            DIMINUTO_LOG_DEBUG("diminuto_thread:proxy: running");
        diminuto_condition_unlock(&(tp->condition));
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
    tp->context = (void *)0;
    tp->value = (void *)0;
    tp->notification = DIMINUTO_THREAD_SIGNAL;
    tp->notifying = 0;
    tp->state = DIMINUTO_THREAD_STATE_INITIALIZED;

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
        tp->state = DIMINUTO_THREAD_STATE_FINISHED;
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
    diminuto_condition_lock(&(tp->condition));
        notifying = tp->notifying;
        tp->notifying = 0;
    diminuto_condition_unlock(&(tp->condition));

    return notifying;
}

void diminuto_thread_exit(void * vp)
{
    diminuto_thread_t * tp = (diminuto_thread_t *)0;

    tp = diminuto_thread_instance();
    diminuto_condition_lock(&(tp-condition));
        tp->state = DIMINUTO_THREAD_STATE_EXITING;
        tp->value = vp;
        DIMINUTO_LOG_DEBUG("diminuto_thread_exit: exiting");
    diminuto_condition_unlock(&(tp->condition));
    pthread_exit(vp);
}

void diminuto_thread_yield()
{
    pthread_yield();
}

/*
 * FUNCTIONS CALLED AGAINST A THREAD
 */

int diminuto_thread_start(diminuto_thread_t * tp, void * cp)
{
    int rc = EIO;
    struct sigaction action = { 0 };

    diminuto_condition_lock(&(tp->condition));
        switch (tp->state) {
        case DIMINUTO_THREAD_STATE_INITIALIZED:
        case DIMINUTO_THREAD_STATE_FINISHED:
        case DIMINUTO_THREAD_STATE_FAILED:
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
            tp->notifying = 0;
            tp->state = DIMINUTO_THREAD_STATE_STARTED;
            if ((rc = pthread_create(&(tp->thread), (const pthread_attr_t *)0, proxy, tp)) != 0) {
                errno = rc;
                diminuto_perror("diminuto_thread_start: pthread_create");
                tp->state = DIMINUTO_THREAD_STATE_FAILED;
            } else {
                DIMINUTO_LOG_INFORMATION("diminuto_thread_start: started");
                pthread_yield();
            }
            break;
        default:
            break;
        }
    diminuto_condition_unlock(&(tp->condition));

    return rc;
}

int diminuto_thread_notify(diminuto_thread_t * tp)
{
    int rc = EIO;

    diminuto_condition_lock(&(tp->condition));
        switch (tp->state) {
        case DIMINUTO_THREAD_STATE_STARTED:
        case DIMINUTO_THREAD_STATE_RUNNING:
            tp->notifying = !0;
            if ((tp->notification) <= 0) {
                /* Do nothing. */
            } else if ((rc = pthread_kill(tp->thread, tp->notification)) != 0) {
                errno = rc;
                diminuto_perror("diminuto_thread_notify: pthread_kill");
            } else {
                /* Do nothing. */
            }
            break;
        default:
            break;
        }
    diminuto_condition_unlock(&(tp->condition));

    return rc;
}

int diminuto_thread_join_try(diminuto_thread_t * tp, void ** vpp, diminuto_ticks_t timeout)
{
    int rc = EIO;
    struct timespec now = { 0, };
    static const diminuto_ticks_t SECONDS = 1;
    static const diminuto_ticks_t NANOSECONDS = 1000000000;

    diminuto_condition_lock(&(tp->condition));
        switch (tp->state) {
        case DIMINUTO_THREAD_STATE_STARTED:
        case DIMINUTO_THREAD_STATE_RUNNING:
        case DIMINUTO_THREAD_STATE_EXITING:
        case DIMINUTO_THREAD_STATE_COMPLETED:
            if (pthread_equal(pthread_self(), tp->thread)) {
                /* Do nothing. */
            } else if (timeout == DIMINUTO_THREAD_INFINITY) {
                if ((rc = pthread_cond_wait(&(tp->condition.condition), &(tp->condition.mutex))) != 0) {
                    diminuto_perror("diminuto_thread_join_try: pthread_cond_wait");
                }
            } else if ((rc = clock_gettime(CLOCK_REALTIME, &now)) < 0) {
                rc = errno;
                diminuto_perror("diminuto_thread_join_try: clock_gettime");
            } else {
                timeout += diminuto_frequency_units2ticks(now.tv_sec, SECONDS);
                timeout += diminuto_frequency_units2ticks(now.tv_nsec, NANOSECONDS);
                now.tv_sec = diminuto_frequency_ticks2whoseconds(timeout);
                now.tv_nsec = diminuto_frequency_ticks2fractionalseconds(NANOSECONDS);
                if ((rc = pthread_cond_timedwait(&(cp->condition), &(cp->mutex.mutex), &now)) != 0) {
                    diminuto_perror("diminuto_thread_join_try: pthread_cond_timedwait");
                }
            }
            if (rc != 0) {
                /* Do nothing. */
            } else if ((rc = pthread_join(tp->thread, &(tp->value))) != 0) {
                diminuto_perror("diminuto_thread_join_try: pthread_join");
            } else {
                if (vpp != (void **)0) {
                    *vpp = tp->value;
                }
                tp->state = DIMINUTO_THREAD_STATE_JOINED;
                DIMINUTO_LOG_INFORMATION("diminuto_thread_join_try: joined");
            }
            break;
        case DIMINUTO_THREAD_STATE_JOINED:
            if (vpp != (void **)0) {
                *vpp = tp->value;
            }
            rc = 0;
            break;
        default:
            break;
        }
    diminuto_condition_unlock(&(tp->condition));

    return rc;
}
