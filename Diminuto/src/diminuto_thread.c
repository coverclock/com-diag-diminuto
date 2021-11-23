/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2020-2021 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is the implementation of the Thread feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is the implementation of the Thread feature.
 */

/***********************************************************************
 * PREREQUISITES
 **********************************************************************/

#include "com/diag/diminuto/diminuto_thread.h"
#include "com/diag/diminuto/diminuto_criticalsection.h"
#include "com/diag/diminuto/diminuto_coherentsection.h"
#include "com/diag/diminuto/diminuto_frequency.h"
#include "com/diag/diminuto/diminuto_log.h"
#include <time.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <sched.h>

/***********************************************************************
 * GLOBALS
 **********************************************************************/

/**
 * This is the pthread specific key used to store a pointer to the
 * thread's Diminuto thread object.
 */
static pthread_key_t key;

/***********************************************************************
 * CALLBACKS
 **********************************************************************/

void thread_cleanup(void * vp)
{
    diminuto_thread_t * tp = (diminuto_thread_t *)vp;

    diminuto_thread_unlock(tp);
}

void proxy_cleanup(void * vp)
{
    diminuto_thread_t * tp = (diminuto_thread_t *)vp;

    if (diminuto_thread_lock(tp) != 0) {
        tp->state = DIMINUTO_THREAD_STATE_FAILED;
    } else {
        pthread_cleanup_push(thread_cleanup, (void *)tp);
            tp->state = DIMINUTO_THREAD_STATE_EXITING;
            DIMINUTO_LOG_DEBUG("diminuto_thread:proxy: EXITING %p", tp);
            (void)diminuto_thread_signal(tp);
        pthread_cleanup_pop(!0);
    }
}

/***********************************************************************
 * WRAPPERS
 **********************************************************************/

/**
 * This is the actual thread function called by POSIX threads. It calls
 * the user function, and handles some bookkeeping beforehand and
 * afterwards.
 * @param vp points to the thread object.
 * @return the final value returned by the user function.
 */
static void * proxy(void * vp)
{
    diminuto_thread_t * tp = (diminuto_thread_t *)vp;
    void * value = (void *)0;
    int previous = -1;
    int rc = DIMINUTO_THREAD_ERROR;

    /*
     * We allow deferred cancellation, but we don't encourage it by
     * providing an API for it.
     */

    if ((rc = pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, &previous)) != 0) {
        errno = rc;
        diminuto_perror("diminuto_thread:proxy: pthread_setcanceltype");
    } else if ((rc = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, &previous)) != 0) {
        errno = rc;
        diminuto_perror("diminuto_thread:proxy: pthread_setcancelstate");
    } else if ((rc = pthread_setspecific(key, tp)) != 0) {
        errno = rc;
        diminuto_perror("diminuto_thread:proxy: pthread_setspecific");
    } else if (diminuto_thread_lock(tp) != 0) {
        tp->state = DIMINUTO_THREAD_STATE_FAILED;
    } else {
        pthread_cleanup_push(thread_cleanup, (void *)tp);
            tp->state = DIMINUTO_THREAD_STATE_RUNNING;
            DIMINUTO_LOG_DEBUG("diminuto_thread:proxy: RUNNING %p", tp);
            (void)diminuto_thread_signal(tp);
        pthread_cleanup_pop(!0);
        /*
         * This is where we call the actual user function and later
         * capture its return value.
         */
        pthread_cleanup_push(proxy_cleanup, tp);
            tp->value = (*(tp->function))(tp->context);
        pthread_cleanup_pop(!0);
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
 * SINGLETONS
 **********************************************************************/

/**
 * This function initializes once and only once the main thread object.
 * Should probably have used pthread_once() feature instead.
 */
static void setup()
{
    extern int main(int argc, char * argv[]);
    int rc = DIMINUTO_THREAD_ERROR;
    static int setupped = 0;
    static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    static diminuto_thread_t thread = DIMINUTO_THREAD_INITIALIZER((diminuto_thread_function_t *)0);

    /*
     * Do the double check locking pattern, and hope it works. The
     * coherent section might or might not expand into read and write
     * memory barriers.
     */

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
                    thread.function = (diminuto_thread_function_t *)main;
                    thread.notify = 0; /* Main doesn't get a kill signal. */
                    thread.notifications = 0;
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
        tp->notify = DIMINUTO_THREAD_NOTIFY;
        tp->notifications = 0;
        tp->state = DIMINUTO_THREAD_STATE_INITIALIZED;
        result = tp;
    }

    return result;
}

diminuto_thread_t * diminuto_thread_fini(diminuto_thread_t * tp)
{
    diminuto_thread_t * result = tp;
    int rc = 0; /* Not ERROR! */

    switch (tp->state) {
    case DIMINUTO_THREAD_STATE_INITIALIZED:
    case DIMINUTO_THREAD_STATE_JOINED:
    case DIMINUTO_THREAD_STATE_FAILED:
        if (diminuto_condition_fini(&(tp->condition)) != (diminuto_condition_t *)0) {
            rc = DIMINUTO_MUTEX_ERROR;
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
 * GETTORS
 **********************************************************************/

diminuto_thread_state_t diminuto_thread_state(const diminuto_thread_t * tp)
{
    diminuto_thread_state_t state = DIMINUTO_THREAD_STATE_UNKNOWN;

    /*
     * Accessing the thread state should be atomic. We can't do this in
     * a critical section because doing so would prevent us from
     * seeing states like ALLOCATED or FINALIZED.
     */

    DIMINUTO_COHERENT_SECTION_BEGIN;
        state = tp->state;
    DIMINUTO_COHERENT_SECTION_END;

    return state;
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
        errno = DIMINUTO_MUTEX_ERROR;
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
    int rc = DIMINUTO_THREAD_ERROR;

#if 0
#   undef _POSIX_PRIORITY_SCHEDULING
#endif

#if defined(_POSIX_PRIORITY_SCHEDULING)
    if ((rc = sched_yield()) < 0) {
        rc = errno;
        diminuto_perror("diminuto_thread_yield: sched_yield");
    }
#else
#   warning "pthread_yield(3) is deprecated!"
    if ((rc = pthread_yield()) != 0) {
        errno = rc;
        diminuto_perror("diminuto_thread_yield: pthread_yield");
    }
#endif

    return rc;
}

unsigned int diminuto_thread_notifications()
{
    diminuto_thread_t * tp = (diminuto_thread_t *)0;
    unsigned int notifications = -1;

    if ((tp = diminuto_thread_instance()) != (diminuto_thread_t *)0) {
        if (diminuto_thread_lock(tp) == 0) {
            pthread_cleanup_push(thread_cleanup, (void *)tp);
                notifications = tp->notifications;
                tp->notifications = 0;
                if (notifications > 0) {
                    DIMINUTO_LOG_DEBUG("diminuto_thread_notifications: NOTIFICATIONS %p [%u]", tp, notifications);
                }
                (void)diminuto_thread_signal(tp);
            pthread_cleanup_pop(!0);
        }
    }

    return notifications;
}

void diminuto_thread_exit(void * xp)
{
    diminuto_thread_t * tp = (diminuto_thread_t *)0;

    if ((tp = diminuto_thread_instance()) != (diminuto_thread_t *)0) {
        tp->value = xp;
        proxy_cleanup(tp);
    }

    pthread_exit(xp);
}

/***********************************************************************
 * OPERATIONS
 **********************************************************************/

int diminuto_thread_start(diminuto_thread_t * tp, void * cp)
{
    int rc = DIMINUTO_THREAD_ERROR;
    struct sigaction action = { 0, };

    setup();

    if (diminuto_thread_lock(tp) == 0) {
        pthread_cleanup_push(thread_cleanup, (void *)tp);
            switch (tp->state) {
            case DIMINUTO_THREAD_STATE_INITIALIZED:
            case DIMINUTO_THREAD_STATE_JOINED:
            case DIMINUTO_THREAD_STATE_FAILED:
                /*
                 * Install the kill signal handler. The kill signal
                 * is used purely to interrupt an interruptable system call,
                 * allowing the thread to unblock so that it might check for
                 * notifications.
                 */
                if (tp->notify != 0) {
                    action.sa_handler = handler;
                    if ((rc = sigaction(tp->notify, &action, (struct sigaction *)0)) < 0) {
                        rc = errno;
                        diminuto_perror("diminuto_thread_start: sigaction");
                        break;
                    }
                }
                tp->context = cp;
                tp->value = (void *)(~0);
                tp->notifications = 0;
                if ((rc = pthread_create(&(tp->thread), (const pthread_attr_t *)0, proxy, tp)) != 0) {
                    errno = rc;
                    diminuto_perror("diminuto_thread_start: pthread_create");
                    tp->state = DIMINUTO_THREAD_STATE_FAILED;
                    DIMINUTO_LOG_DEBUG("diminuto_thread_start: FAILED %p", tp);
                    (void)diminuto_thread_signal(tp);
                } else {
                    tp->state = DIMINUTO_THREAD_STATE_STARTED;
                    DIMINUTO_LOG_DEBUG("diminuto_thread_start: STARTED %p {%llx}", tp, (unsigned long long int)tp->thread);
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
    int rc = DIMINUTO_THREAD_ERROR;

    if (diminuto_thread_lock(tp) == 0) {
        pthread_cleanup_push(thread_cleanup, (void *)tp);
            switch (tp->state) {
            case DIMINUTO_THREAD_STATE_STARTED:
            case DIMINUTO_THREAD_STATE_RUNNING:
                /*
                 * The notification count is allowed to
                 * reach its limit and no more. But we
                 * to all the other notification stuff.
                 */
                if (tp->notifications == (~((unsigned int)0))) {
                    DIMINUTO_LOG_DEBUG("diminuto_thread_notify: NOTIFIED %p [*]", tp);
                } else {
                    ++tp->notifications;
                    DIMINUTO_LOG_DEBUG("diminuto_thread_notify: NOTIFIED %p [%u]", tp, tp->notifications);
                }
                /*
                 * Signal any waiting threads that a notification has
                 * arrived. Then send a kill signal to the thread to
                 * possible interrupt a system call, allow it to
                 * unblock, and check for notifications.
                 */
                if ((rc = diminuto_thread_signal(tp)) != 0) {
                    /* Do nothing. */
                } else if (tp->notify == 0) {
                    /* Do nothing. */
                } else if ((rc = pthread_kill(tp->thread, tp->notify)) != 0) {
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
    int rc = DIMINUTO_THREAD_ERROR;
    int done = 0;
    void * value = (void *)0;
    struct timespec now = { 0, };
    static const diminuto_ticks_t SECONDS = 1;
    static const diminuto_ticks_t NANOSECONDS = 1000000000;

    /*
     * Wait for the thread to reach its completing state.
     */

    if (diminuto_thread_lock(tp) == 0) {
        pthread_cleanup_push(thread_cleanup, (void *)tp);
        
            switch (tp->state) {
    
            case DIMINUTO_THREAD_STATE_STARTED:
            case DIMINUTO_THREAD_STATE_RUNNING:
                if (pthread_equal(pthread_self(), tp->thread)) {
    
                    /* Do nothing. */
    
                } else if (clocktime == DIMINUTO_THREAD_INFINITY) {

                    do {
                        if ((rc = diminuto_thread_wait(tp)) != 0) {
                            done = !0;
                        } else if (tp->state == DIMINUTO_THREAD_STATE_EXITING) {
                            done = !0;
                        } else {
                            /* Do nothing. */
                        }
                    } while (!done);
    
                } else {
    
                    do {
                        if ((rc = diminuto_thread_wait_until(tp, clocktime)) == 0) {
                            /* Do nothing. */
                        } else if (rc == DIMINUTO_THREAD_TIMEDOUT) {
                            done = !0;
                        } else {
                            diminuto_perror("diminuto_thread_join_until: pthread_cond_timedwait");
                            done = !0;
                        }
                        if (done) {
                            /* Do nothing. */
                        } else if (tp->state == DIMINUTO_THREAD_STATE_EXITING) {
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

    /*
     * If the thread has reached completion, we can join with it.
     */
    
    switch (tp->state) {
    
    case DIMINUTO_THREAD_STATE_EXITING:
        if (pthread_equal(pthread_self(), tp->thread)) {
            rc = DIMINUTO_THREAD_ERROR;
        } else if ((rc = pthread_join(tp->thread, &value)) != 0) {
            diminuto_perror("diminuto_thread_join_until: pthread_join");
        } else {
            if (diminuto_thread_lock(tp) == 0) {
                pthread_cleanup_push(thread_cleanup, (void *)tp);
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

    /*
     * If the thread has joined, we can capture its final value.
     */
    
    switch (tp->state) {
    
    case DIMINUTO_THREAD_STATE_JOINED:
        if (pthread_equal(pthread_self(), tp->thread)) {
            rc = DIMINUTO_THREAD_ERROR;
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
