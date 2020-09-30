/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2020 Digital Aggregates Corporation, Colorado, USA.<BR>
 * Licensed under the terms in LICENSE.txt.<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 */

/***********************************************************************
 * PREREQUISITES
 **********************************************************************/

#include <time.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
#include "com/diag/diminuto/diminuto_thread.h"
#include "com/diag/diminuto/diminuto_criticalsection.h"
#include "com/diag/diminuto/diminuto_coherentsection.h"
#include "com/diag/diminuto/diminuto_frequency.h"
#include "com/diag/diminuto/diminuto_log.h"

/***********************************************************************
 * GLOBALS
 **********************************************************************/

/**
 * This is the pthread specific key used to store a pointer to the
 * thread's Diminuto thread object.
 */
static pthread_key_t key;

/***********************************************************************
 * WRAPPERS
 **********************************************************************/

/**
 * This is the actual thread function called by POSIX threads. It calls
 * the user function, and handles some bookkeeping beforehand and
 * afterwards.
 * @param ap points to the thread object.
 * @return the final value returned by the user function.
 */
static void * proxy(void * ap)
{
    diminuto_thread_t * tp = (diminuto_thread_t *)ap;
    void * value = (void *)0;
    int previous = -1;
    int rc = -1;

    if ((rc = pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, &previous)) != 0) {
        errno = rc;
        diminuto_perror("diminuto_thread: setup: pthread_setcanceltype");
    } else if ((rc = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, &previous)) != 0) {
        errno = rc;
        diminuto_perror("diminuto_thread: setup: pthread_setcancelstate");
    } else if ((rc = pthread_setspecific(key, tp)) != 0) {
        errno = rc;
        diminuto_perror("diminuto_thread: setup: pthread_setspecific");
    } else {

        if (diminuto_thread_lock(tp) != 0) {

            tp->state = DIMINUTO_THREAD_STATE_FAILED;

        } else {

            pthread_cleanup_push(diminuto_thread_cleanup, (void *)tp);
                tp->state = DIMINUTO_THREAD_STATE_RUNNING;
                DIMINUTO_LOG_DEBUG("diminuto_thread:proxy: RUNNING %p", tp);
                (void)diminuto_thread_signal(tp);
            pthread_cleanup_pop(!0);

            value = (*(tp->function))(tp->context);

            if (diminuto_thread_lock(tp) != 0) {

                tp->state = DIMINUTO_THREAD_STATE_FAILED;

            } else {

                pthread_cleanup_push(diminuto_thread_cleanup, (void *)tp);
                    tp->value = value;
                    tp->state = DIMINUTO_THREAD_STATE_COMPLETING;
                    DIMINUTO_LOG_DEBUG("diminuto_thread:proxy: COMPLETING %p", tp);
                    (void)diminuto_thread_signal(tp);
                pthread_cleanup_pop(!0);
            }

        }

    }

    return tp->value;
}

/***********************************************************************
 * HANDLERS
 **********************************************************************/

/**
 * This is the kill signal handler that receives the notification
 * signal (if enabled).
 * @param signo is the kill signal number.
 */
static void handler(int signo)
{
    /* Do nothing. */
}

/***********************************************************************
 * HELPERS
 **********************************************************************/

/**
 * This function initializes once and only once the main thread object.
 * Should probably have used pthread_once() feature instead.
 */
static void setup()
{
    extern int main(int argc, char * argv[]);
    int rc = EIO;
    static int setupped = 0;
    static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    static diminuto_thread_t thread = DIMINUTO_THREAD_INITIALIZER((void * (*)(void *))main);

    DIMINUTO_COHERENT_SECTION_BEGIN;
        if (!setupped) {
            DIMINUTO_CRITICAL_SECTION_BEGIN(&mutex);
                if (setupped) {
                    /* Do nothing. */
                } else if ((rc = pthread_key_create(&key, 0)) != 0) {
                    errno = rc;
                    diminuto_perror("diminuto_thread:setup: pthread_key_create");
                } else if ((rc = pthread_setspecific(key, &thread)) != 0) {
                    errno = rc;
                    diminuto_perror("diminuto_thread:setup: pthread_setspecific");
                } else {
                    thread.thread = pthread_self();
                    thread.notification = 0;
                    thread.state = DIMINUTO_THREAD_STATE_RUNNING;
                    setupped = !0;
                    DIMINUTO_LOG_DEBUG("diminuto_thread:setup: SETUP %p", &thread);
                }
            DIMINUTO_CRITICAL_SECTION_END;
        }
    DIMINUTO_COHERENT_SECTION_END;
}

/***********************************************************************
 * INITIALIZERS AND FINALIZERS
 **********************************************************************/

diminuto_thread_t * diminuto_thread_init(diminuto_thread_t * tp, void * (*fp)(void *))
{
    diminuto_thread_t * result = (diminuto_thread_t *)0;

    if (diminuto_condition_init(&(tp->condition)) == &(tp->condition)) {
        memset(&(tp->thread), 0, sizeof(tp->thread));
        tp->function = fp;
        tp->context = (void *)0;
        tp->value = (void *)~0;
        tp->notification = DIMINUTO_THREAD_SIGNAL;
        tp->notifying = 0;
        tp->state = DIMINUTO_THREAD_STATE_INITIALIZED;
        result = tp;
    }

    return result;
}

diminuto_thread_t * diminuto_thread_fini(diminuto_thread_t * tp)
{
    diminuto_thread_t * result = tp;
    int rc = 0; /* Not EIO. */

    switch (tp->state) {
    case DIMINUTO_THREAD_STATE_INITIALIZED:
    case DIMINUTO_THREAD_STATE_JOINED:
    case DIMINUTO_THREAD_STATE_FAILED:
        if (diminuto_condition_fini(&(tp->condition)) != (diminuto_condition_t *)0) {
            rc = EIO;
        }
        /* FALL THRU */
    case DIMINUTO_THREAD_STATE_ALLOCATED:
        if (rc == 0) {
            tp->state = DIMINUTO_THREAD_STATE_FINALIZED;
        }
        /* FALL THRU */
    case DIMINUTO_THREAD_STATE_FINALIZED:
        if (rc == 0) {
            result = (diminuto_thread_t *)0;    
        }
        break;
    default:
        /* Do nothing. */
        break;
    }

    return result;
}

/***********************************************************************
 * ACTIONS
 **********************************************************************/

diminuto_thread_t * diminuto_thread_instance()
{
    diminuto_thread_t * tp = (diminuto_thread_t *)0;

    setup();

    tp = (diminuto_thread_t *)pthread_getspecific(key);
    if (tp == (diminuto_thread_t *)0) {
        errno = EIO;
        diminuto_perror("diminuto_thread_instance: pthread_getspecific");
    }

    return tp;
}

pthread_t diminuto_thread_self()
{
    return pthread_self();
}

int diminuto_thread_yield()
{
    int rc = EIO;

    if ((rc = pthread_yield())) {
        errno = rc;
        diminuto_perror("diminuto_thread_yield: pthread_yield");
    }

    return rc;
}

int diminuto_thread_notified()
{
    diminuto_thread_t * tp = (diminuto_thread_t *)0;
    int notifying = -1;

    if ((tp = diminuto_thread_instance()) != (diminuto_thread_t *)0) {
        if (diminuto_thread_lock(tp) == 0) {
            pthread_cleanup_push(diminuto_thread_cleanup, (void *)tp);
                notifying = tp->notifying;
                tp->notifying = 0;
                (void)diminuto_thread_signal(tp);
            pthread_cleanup_pop(!0);
        }
    }

    return notifying;
}

void diminuto_thread_exit(void * vp)
{
    diminuto_thread_t * tp = (diminuto_thread_t *)0;

    if ((tp = diminuto_thread_instance()) != (diminuto_thread_t *)0) {
        if (diminuto_thread_lock(tp) == 0) {
            pthread_cleanup_push(diminuto_thread_cleanup, (void *)tp);
                tp->value = vp;
                tp->state = DIMINUTO_THREAD_STATE_COMPLETING;
                DIMINUTO_LOG_DEBUG("diminuto_thread_exit: COMPLETING %p", tp);
                (void)diminuto_thread_signal(tp);
            pthread_cleanup_pop(!0);
        }
    }

    DIMINUTO_LOG_DEBUG("diminuto_thread_exit: EXITING %p", tp);

    pthread_exit(vp);
}

/***********************************************************************
 * OPERATIONS
 **********************************************************************/

int diminuto_thread_start(diminuto_thread_t * tp, void * cp)
{
    int rc = EIO;
    struct sigaction action = { 0 };

    setup();

    if (diminuto_thread_lock(tp) == 0) {
        pthread_cleanup_push(diminuto_thread_cleanup, (void *)tp);
            switch (tp->state) {
            case DIMINUTO_THREAD_STATE_INITIALIZED:
            case DIMINUTO_THREAD_STATE_JOINED:
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
                if ((rc = pthread_create(&(tp->thread), (const pthread_attr_t *)0, proxy, tp)) != 0) {
                    errno = rc;
                    diminuto_perror("diminuto_thread_start: pthread_create");
                    tp->state = DIMINUTO_THREAD_STATE_FAILED;
                    DIMINUTO_LOG_DEBUG("diminuto_thread_start: FAILED %p", tp);
                    (void)diminuto_thread_signal(tp);
                } else {
                    tp->state = DIMINUTO_THREAD_STATE_STARTED;
                    DIMINUTO_LOG_DEBUG("diminuto_thread_start: STARTED %p", tp);
                    (void)diminuto_thread_signal(tp);
                }
                break;
            default:
                break;
            }
        pthread_cleanup_pop(!0);
    }

    return rc;
}

int diminuto_thread_notify(diminuto_thread_t * tp)
{
    int rc = EIO;

    if (diminuto_thread_lock(tp) == 0) {
        pthread_cleanup_push(diminuto_thread_cleanup, (void *)tp);
            switch (tp->state) {
            case DIMINUTO_THREAD_STATE_STARTED:
            case DIMINUTO_THREAD_STATE_RUNNING:
                tp->notifying = !0;
                (void)diminuto_thread_signal(tp);
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
        pthread_cleanup_pop(!0);
    }

    return rc;
}

int diminuto_thread_join_until(diminuto_thread_t * tp, void ** vpp, diminuto_ticks_t clocktime)
{
    int rc = EIO;
    int done = 0;
    void * value = (void *)0;
    struct timespec now = { 0, };
    static const diminuto_ticks_t SECONDS = 1;
    static const diminuto_ticks_t NANOSECONDS = 1000000000;

    if (diminuto_thread_lock(tp) == 0) {
        pthread_cleanup_push(diminuto_thread_cleanup, (void *)tp);
        
            switch (tp->state) {
    
            case DIMINUTO_THREAD_STATE_STARTED:
            case DIMINUTO_THREAD_STATE_RUNNING:
                if (pthread_equal(pthread_self(), tp->thread)) {
    
                    /* Do nothing. */
    
                } else if (clocktime == DIMINUTO_THREAD_INFINITY) {
    
                    do {
                        if ((rc = diminuto_thread_wait(tp)) != 0) {
                            done = !0;
                        } else if (tp->state == DIMINUTO_THREAD_STATE_COMPLETING) {
                            done = !0;
                        } else {
                            /* Do nothing. */
                        }
                    } while (!done);
    
                } else {
    
                    do {
                        if ((rc = diminuto_thread_wait_until(tp, clocktime)) == 0) {
                            /* Do nothing. */
                        } else if (rc == ETIMEDOUT) {
                            done = !0;
                        } else {
                            diminuto_perror("diminuto_thread_join_until: pthread_cond_timedwait");
                            done = !0;
                        }
                        if (done) {
                            /* Do nothing. */
                        } else if (tp->state == DIMINUTO_THREAD_STATE_COMPLETING) {
                            done = !0;
                        } else {
                            /* Do nothing. */
                        }
                    } while (!done);
    
                }
    
            default:
                break;
    
            }
    
        pthread_cleanup_pop(!0);
    }
    
    switch (tp->state) {
    
    case DIMINUTO_THREAD_STATE_COMPLETING:
        if (pthread_equal(pthread_self(), tp->thread)) {
            rc = EIO;
        } else if ((rc = pthread_join(tp->thread, &value)) != 0) {
            diminuto_perror("diminuto_thread_join_until: pthread_join");
        } else {
            if (diminuto_thread_lock(tp) == 0) {
                pthread_cleanup_push(diminuto_thread_cleanup, (void *)tp);
                    tp->value = value;
                    tp->state = DIMINUTO_THREAD_STATE_JOINED;
                    DIMINUTO_LOG_DEBUG("diminuto_thread_join_until: JOINED %p", tp);
                    (void)diminuto_thread_signal(tp);
                pthread_cleanup_pop(!0);
            }
        }
        break;
    
   default:
        break;
    
    }
    
    switch (tp->state) {
    
    case DIMINUTO_THREAD_STATE_JOINED:
        if (pthread_equal(pthread_self(), tp->thread)) {
            rc = EIO;
        } else if (vpp != (void **)0) {
            *vpp = tp->value;
            rc = 0;
        } else {
            rc = 0;
        }
        break;
    
    default:
        break;
    
    }

    return rc;
}

/***********************************************************************
 * CALLBACKS
 **********************************************************************/

void diminuto_thread_cleanup(void * vp)
{
    diminuto_thread_t * tp = (diminuto_thread_t *)vp;

    diminuto_thread_unlock(tp);
}
