/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2020-2022 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is the implementation of the Reader Writer feature.
 *
 * This translation unit contains Diminuto assert statements that check
 * invariants upon entry and exit from the Reader and Writer Begin and End
 * functions. As long as the implementation does the right thing, these are
 * benign. Should the invariants be violated, the default behavior is for the
 * asserts to abort the application and produce a core dump. This behavior
 * can be overridden by compile time options - see the diminuto_assert.h
 * header file for more details. But the asserts failing indicates a very
 * serious bug in my code, implying the reader-writer synchronization is
 * unreliable.
 */

#include "com/diag/diminuto/diminuto_readerwriter.h"
#include "com/diag/diminuto/diminuto_assert.h"
#include "com/diag/diminuto/diminuto_criticalsection.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_time.h"
#include "com/diag/diminuto/diminuto_frequency.h"
#include "com/diag/diminuto/diminuto_readerwriter.h"
#include <stdlib.h>
#include "diminuto_readerwriter.h"

/*******************************************************************************
 * CONSTANTS
 ******************************************************************************/

/**
 * Reader Writer calling threads may assume the following "roles". These role
 * values are used as parameter to the Reader Writer scheduling functions
 * (below), or as tokens inserted and removed from the waiting list.
 * "Pending" means the thread has been awakened from the condition variable but
 * has not yet removed its token from the circular list..
 */
typedef enum Role {
    ANY     = '*',  /**< Any role. */
    NONE    = '-',  /**< No role. */
    STARTED = 'S',  /**< Initial thread role. */
    READER  = 'R',  /**< Waiting reader role. */
    WRITER  = 'W',  /**< Waiting writer role. */
    FAILED  = 'X',  /**< Cancelled, timed out, failed, etc. */
    READING = 'r',  /**< Pending reader role. */
    WRITING = 'w',  /**< Pending writer role. */
    RUNNING = 'A',  /**< Running thread role. */
} role_t;

/*******************************************************************************
 * FILE GLOBAL
 ******************************************************************************/

/**
 * This is the file global mutex used to control access to the key value
 * used to access thread local data used by Reader Writer.
 */
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

/**
 * This is the file global flag that insures that the initialization routine
 * is performed once and only once.
 */
static int initialized = 0;

/**
 * This is the key value used to access thread local data used by Reader Writer.
 * The key will be initialized the first time it is needed by any Reader
 * Writer object. Multiple Reader Writer objects will use the same key value.
 */
static pthread_key_t key;

/*******************************************************************************
 * CODE GENERATORS
 ******************************************************************************/

/**
 * @def BEGIN_CRITICAL_SECTION
 * This is the opening bracket of a critical section using the mutex
 * in the Reader Writer object pointed to by @a _RWP_.
 */
#define BEGIN_CRITICAL_SECTION(_RWP_) \
    do { \
        diminuto_readerwriter_t * critical_section_rwp = (diminuto_readerwriter_t *)0; \
        critical_section_rwp = (_RWP_); \
        if ((errno = pthread_mutex_lock(&(critical_section_rwp->mutex))) != 0) { \
            diminuto_perror("diminuto_readerwriter: BEGIN_CRITICAL_SECTION: pthread_mutex_lock"); \
        } else { \
            pthread_cleanup_push(mutex_cleanup, critical_section_rwp)

/**
 * @def END_CRITICAL_SECTION
 * This is the closing bracket of a critical section using the mutex
 * in the Reader Writer object pointed to by @a _RWP_.
 */
#define END_CRITICAL_SECTION \
            pthread_cleanup_pop(!0); \
            critical_section_rwp = (diminuto_readerwriter_t *)0; \
        } \
    } while (0)

/*******************************************************************************
 * CALLBACKS
 ******************************************************************************/

/**
 * Clean up the mutex if an abnormal termination occurrs.
 * @param vp points to the Reader Writer object.
 */
static void mutex_cleanup(void * vp)
{
    diminuto_readerwriter_t * rwp = (diminuto_readerwriter_t *)vp;
    int rc = DIMINUTO_READERWRITER_ERROR;

    if ((rc = pthread_mutex_unlock(&(rwp->mutex))) != 0) {
        errno = rc;
        diminuto_perror("diminuto_readerwriter: mutex_cleanup: pthread_mutex_unlock");
    }
}

/**
 * Cleanup allocated memory in the main thread at process exit. This does
 * seem pointless, doesn't it? But it keeps valgrind(1) happy, since it
 * complains about the Reader Writer unit test.
 */
static void exit_cleanup()
{
    void * vp = (void *)0;

    if ((vp = pthread_getspecific(key)) != (void *)0) {
        diminuto_list_t * np = (diminuto_list_t *)0;
        np = (diminuto_list_t *)vp;
        free(np);
        DIMINUTO_LOG_DEBUG("diminuto_readerwriter: Global %p RELEASED", np);
    }
}

/**
 * Release the object pointed to by the Reader Writer thread local storage.
 * Typically this has to be done when a thread terminates, for example by
 * exiting or by cancellation. In other languages, this would be known as
 * a destructor. The implementation of this is not entirely thread safe
 * in the context of thread cancellation, especially asynchronous
 * cancellation.
 * @param vp points to the thread local object for the terminating thread.
 */
static void key_cleanup(void * vp)
{
    if (vp != (void *)0) {
        diminuto_list_t * np = (diminuto_list_t *)vp;
        diminuto_list_t * rp = (diminuto_list_t *)0;
        diminuto_readerwriter_t * rwp = (diminuto_readerwriter_t *)0;

        if (diminuto_list_isroot(np)) {
            /* Do nothing. */
        } else if ((rwp = (diminuto_readerwriter_t *)diminuto_list_data(diminuto_list_root(np))) == (diminuto_readerwriter_t *)0) {
            /* Do nothing. */
        } else {
            BEGIN_CRITICAL_SECTION(rwp);

                (void)diminuto_list_remove(np);

            END_CRITICAL_SECTION;
        }
        free(np);
        DIMINUTO_LOG_DEBUG("diminuto_readerwriter: Local %p RELEASED", np);
    }
}

/**
 * Clean up the state if an abnormal wait occurs. Note that
 * we can't remove the node from the list because we have no
 * way of acquiring the mutex for the Reader Writer structure.
 * So we set its state to FAILED and let someone who does hold
 * the mutex clean it up.
 * @param vp points to the thread local object for the associated thread.
 */
static void wait_cleanup(void * vp)
{
#if 0
    diminuto_list_t * np = (diminuto_list_t *)vp;

    diminuto_list_dataset(np, (void *)FAILED);
#else
    key_cleanup(vp);
#endif
}

/*******************************************************************************
 * THREAD LOCAL
 ******************************************************************************/

/**
 * Create the key used to identify the thread local storage associated with
 * each thread that uses any Reader Writer object.
 * @return 0 for success, or an error code otherwise.
 */
static int initialize()
{
    int rc = 0;

    DIMINUTO_CRITICAL_SECTION_BEGIN(&mutex);

        if (initialized) {
            /* Do nothing. */
        } else if ((rc = pthread_key_create(&key, &key_cleanup)) != 0) {
            errno = rc;
            diminuto_perror("diminuto_readerwriter: initialize: pthread_key_create");
        } else if ((rc = atexit(&exit_cleanup)) != 0) {
            diminuto_perror("diminuto_readerwriter: initialize: atexit");
        } else {
            DIMINUTO_LOG_DEBUG("diminuto_readerwriter: Global %u ACQUIRED", key);
            initialized = !0;
        }

    DIMINUTO_CRITICAL_SECTION_END;

    return rc;
}

/**
 * Return a pointer to the thread local object used by Reader Writer for the
 * calling thread. This will be different for every thread.
 * @return a pointer to the thread local object.
 */
static diminuto_list_t * acquire()
{
    diminuto_list_t * np = (diminuto_list_t *)0;
    void * vp = (void *)0;
    int rc = 0;

    if (initialize() != 0) {
        /* Do nothing. */
    } else if ((vp = pthread_getspecific(key)) != (void *)0) {
        np = (diminuto_list_t *)vp;
    } else if ((np = (diminuto_list_t *)malloc(sizeof(diminuto_list_t))) == (diminuto_list_t *)0) {
        diminuto_perror("diminuto_readerwriter: acquire: malloc");
    } else if ((rc = pthread_setspecific(key, diminuto_list_datainit(np, (void *)STARTED))) != 0) {
        errno = rc;
        diminuto_perror("diminuto_readerwriter: acquire: pthread_key_setspecific");
        free(np);
        np = (diminuto_list_t *)0;
    } else {
        DIMINUTO_LOG_DEBUG("diminuto_readerwriter: Local %p ACQUIRED", np);
    }

    return np;
}

/*******************************************************************************
 * STRUCTORS
 ******************************************************************************/

diminuto_readerwriter_t * diminuto_readerwriter_init(diminuto_readerwriter_t * rwp)
{
    diminuto_readerwriter_t * result = (diminuto_readerwriter_t *)0;
    int rc = DIMINUTO_READERWRITER_ERROR;

    if ((rc = pthread_mutex_init(&(rwp->mutex), (pthread_mutexattr_t *)0)) != 0) {
        errno = rc;
        diminuto_perror("diminuto_readerwriter_init: pthread_mutex_init");
    } else if ((rc = pthread_cond_init(&(rwp->reader), (pthread_condattr_t *)0)) != 0) {
        errno = rc;
        diminuto_perror("diminution_readerwriter_init: pthread_cond_init: reader");
    } else if ((rc = pthread_cond_init(&(rwp->writer), (pthread_condattr_t *)0)) != 0) {
        errno = rc;
        diminuto_perror("diminution_readerwriter_init: pthread_cond_init: writer");
    } else {
        diminuto_list_datainit(&(rwp->list), (void *)rwp);
        rwp->fp = (FILE *)0;
        rwp->reading = 0;
        rwp->writing = 0;
        rwp->waiting = 0;
        result = rwp;
    }

    return result;

}

diminuto_readerwriter_t * diminuto_readerwriter_fini(diminuto_readerwriter_t * rwp)
{
    diminuto_readerwriter_t * result = rwp;
    int rc = DIMINUTO_READERWRITER_ERROR;

    if (rwp->waiting > 0) {
        errno = DIMINUTO_READERWRITER_BUSY;
        diminuto_perror("diminuto_readerwriter_fini");
    } else if (diminuto_list_fini(&(rwp->list)) != (diminuto_list_t *)0) {
        errno = DIMINUTO_READERWRITER_UNEXPECTED;
        diminuto_perror("diminuto_readerwriter_fini: diminuto_list_fini");
    } else if ((rc = pthread_cond_destroy(&(rwp->writer))) != 0) {
        errno = rc;
        diminuto_perror("diminuto_readerwriter_fini: pthread_cond_destroy: writer");
    } else if ((rc = pthread_cond_destroy(&(rwp->reader))) != 0) {
        errno = rc;
        diminuto_perror("diminuto_readerwriter_fini: pthread_cond_destroy: reader");
    } else if ((rc = pthread_mutex_destroy(&(rwp->mutex))) != 0) {
        errno = rc;
        diminuto_perror("diminuto_readerwriter_fini: pthread_mutex_destroy");
    } else {
        result = (diminuto_readerwriter_t *)0;
    }

    return result;
}

/*******************************************************************************
 * HELPERS
 ******************************************************************************/

/**
 * Display information about the internal state of the Reader Writer object
 * to a file stream (like stderr).
 * @param fp points to the file stream.
 * @param rwp points to the Reader Writer object.
 * @param label is a string displayed to identify who is calling the function.
 */
static void dump(FILE * fp, diminuto_readerwriter_t * rwp, const char * label)
{
    extern pthread_mutex_t diminuto_log_mutex;
    diminuto_list_t * np = (diminuto_list_t *)0;
    diminuto_list_t * rp = (diminuto_list_t *)0;
    char buffer[DIMINUTO_LOG_BUFFER_MAXIMUM] = { '\0', };
    char * bp = (char *)0;
    size_t size = 0;
    int length = 0;
    unsigned int started = 0;
    unsigned int pending = 0;
    unsigned int readers = 0;
    unsigned int writers = 0;
    unsigned int failed = 0;
    unsigned int running = 0;
    unsigned int unknown = 0;
    unsigned int queued = 0;
    role_t role = NONE;

    bp = &(buffer[0]);
    size = sizeof(buffer) - 1;

    length = snprintf(bp, size, "%s(%p):", label, rwp);
    if ((0 < length) && (length < size)) { bp += length; size -= length; }

    length = snprintf(bp, size, " %dreading", rwp->reading);
    if ((0 < length) && (length < size)) { bp += length; size -= length; }

    length = snprintf(bp, size, " %dwriting", rwp->writing);
    if ((0 < length) && (length < size)) { bp += length; size -= length; }

    length = snprintf(bp, size, " %dwaiting {", rwp->waiting);
    if ((0 < length) && (length < size)) { bp += length; size -= length; }

    rp = diminuto_list_root(&(rwp->list));
    np = diminuto_list_next(rp);
    while (np != rp) {
        role = (role_t)diminuto_list_data(np);
        switch (role) {
            case STARTED: ++started; break; /* Should be zero. */
            case READING: ++pending; break;
            case WRITING: ++pending; break;
            case READER:  ++readers; break;
            case WRITER:  ++writers; break;
            case FAILED:  ++failed;  break;
            case RUNNING: ++running; break; /* Should be zero. */
            default:      ++unknown; break; /* Should be zero. */
        }
        length = snprintf(bp, size, " %c", role);
        if ((0 < length) && (length < size)) { bp += length; size -= length; }
        ++queued;
        np = diminuto_list_next(np);
    }

    length = snprintf(bp, size, " } %dqueued", queued);
    if ((0 < length) && (length < size)) { bp += length; size -= length; }

    length = snprintf(bp, size, " %dstarted", started);
    if ((0 < length) && (length < size)) { bp += length; size -= length; }

    length = snprintf(bp, size, " %dpending", pending);
    if ((0 < length) && (length < size)) { bp += length; size -= length; }

    length = snprintf(bp, size, " %dreaders", readers);
    if ((0 < length) && (length < size)) { bp += length; size -= length; }

    length = snprintf(bp, size, " %dwriters", writers);
    if ((0 < length) && (length < size)) { bp += length; size -= length; }

    length = snprintf(bp, size, " %dfailed", failed);
    if ((0 < length) && (length < size)) { bp += length; size -= length; }

    length = snprintf(bp, size, " %drunning", running);
    if ((0 < length) && (length < size)) { bp += length; size -= length; }

    length = snprintf(bp, size, " %dunknown", unknown);
    if ((0 < length) && (length < size)) { bp += length; size -= length; }

    *(bp++) = '\n';
    *(bp) = '\0';

    /*
     * We mutually exclude on the logging system mutex so that if we are
     * not a daemon (so that the log messages from the threads are all
     * going to stderr) we don't step on any log messages. If we are a
     * deamon, then we probably shouldn't be dumping stuff to stderr
     * anyway (because it is probably redirected to /dev/null).
     */

    DIMINUTO_CRITICAL_SECTION_BEGIN(&diminuto_log_mutex);
        fputs(buffer, fp);
        fflush(fp);
    DIMINUTO_CRITICAL_SECTION_END;
}

/*******************************************************************************
 * SCHEDULING
 ******************************************************************************/

/**
 * Return the node at the head of the list without removing it,
 * after first discarding any leading failed nodes. This can also be
 * used just to discard leading failed nodes. Nodes on the list can be
 * in the failing state because they were cancelled, but could not be
 * removed from the list because the caller did not hold the mutex.
 * @param rwp points to the Reader Writer object.
 * @return the node at the head of the list or NULL if empty.
 */
static diminuto_list_t * head(diminuto_readerwriter_t * rwp)
{
    diminuto_list_t * np = (diminuto_list_t *)0;

    while (!0) {
       if ((np = diminuto_list_head(&(rwp->list))) == (diminuto_list_t *)0) {
            break;
        } else if ((role_t)diminuto_list_data(np) != FAILED) {
            break;
        } else {
            (void)diminuto_list_remove(np);
            rwp->waiting -= 1;
            DIMINUTO_LOG_DEBUG("diminuto_readerwriter: Failed %p REMOVED %dreading %dwriting %dwaiting", np, rwp->reading, rwp->writing, rwp->waiting);
        }
    }

    return np;
}


/**
 * Return true if the indicated node in the list is now at the head, and the
 * state of that node is now pending, indicating that the waiting reader or
 * writer has been activated, false otherwise. Calling head has the side effect
 * of removing any ignored tokens, indicating wait failures, off the head of
 * the list.
 * @param rwp points to the Reader Writer object.
 * @param np points to the thread local list node for the calling thread.
 * @param pending is the pending role to be used: READING or WRITING.
 * @return true if the token is both head and pending, false otherwise.
 */
static inline int ready(diminuto_readerwriter_t * rwp, diminuto_list_t * np, role_t pending)
{
    return (head(rwp) == np) && ((role_t)diminuto_list_data(np) == pending);
}

/**
 * Perform either an infinite wait or a timed wait as specified. If a timed
 * wait, compute the absolute clocktime for the timed wait based on the
 * relative timeout duration. This allows the caller to specify a relative
 * timeout duration that applies no matter how many times the timed wait
 * has to be repeated after receiving a broadcast.
 * @param rwp points to the Reader Writer object.
 * @param np points to the thread local list node for the calling thread.
 * @param pending is the pending role to be used: READING or WRITING.
 * @param conditionp points to the condition variable in the object to use.
 * @param timeout is a timeout duration in ticks.
 * @return 0 for success, or an error number otherwise.
 */
static int satisfy(diminuto_readerwriter_t * rwp, diminuto_list_t * np, role_t pending, pthread_cond_t * conditionp, diminuto_ticks_t timeout)
{
    int rc = DIMINUTO_READERWRITER_ERROR;
    diminuto_sticks_t clocktime = 0;

    if (timeout == DIMINUTO_READERWRITER_INFINITY) {

        /*
         * Caller specified an infinite timeout, so we do an untimed wait.
         */

        do {
            if ((rc = pthread_cond_wait(conditionp, &(rwp->mutex))) != 0) {
                errno = rc;
                diminuto_perror("diminuto_readerwriter: satisfy: pthread_cond_wait");
                break;
            }
        } while (!ready(rwp, np, pending));

    } else if ((clocktime = diminuto_time_clock()) < 0) {

        /* Error message already emitted. */
        rc = errno;

    } else {
        static const diminuto_ticks_t NANOSECONDS = 1000000000;
        struct timespec absolutetime = { 0, };

        /*
         * Caller specified a timeout, so we do a timed wait. The
         * timed wait requires an absolute clocktime because it
         * needs to be a fixed point in time every time we iterate
         * through the loop waiting for the condition to be met. So
         * we convert our relative duration into an absolute clocktime
         * (providing our reading of the system clock succeeded).
         */

        clocktime += timeout;
        absolutetime.tv_sec = diminuto_frequency_ticks2wholeseconds(clocktime);
        absolutetime.tv_nsec = diminuto_frequency_ticks2fractionalseconds(clocktime, NANOSECONDS);

        do {
            if ((rc = pthread_cond_timedwait(conditionp, &(rwp->mutex), &absolutetime)) == 0) {
                /* Do nothing. */
            } else if (rc == ETIMEDOUT) {
                errno = rc;
                /* Suppress error message but return error number. */
                break;
            } else {
                errno = rc;
                diminuto_perror("diminuto_readerwriter: satisfy: pthread_cond_timedwait");
                break;
            }
        } while (!ready(rwp, np, pending));

    }

    return rc;
}

/**
 * Place the calling thread in a wait on the condition variable in the
 * Reader Writer object identified by the specified role. The local thread
 * object is inserted into the tail of the wait list before the thread
 * waits. When the thread awakens, it will remove its token (which will have
 * been changed from a "waiting" token to a "pending" token) from the head
 * of the wait list. The index of the token of the thread in the wait
 * list is passed back when the thread awakens purely for purposes of
 * debugging.
 * @param rwp points to the Reader Writer object.
 * @param label is a string used for logging.
 * @param np points to the thread local list node for the calling thread.
 * @param waiting is the waiting role to be used: READER or WRITER.
 * @param pending is the pending role to be used: READING or WRITING.
 * @param conditionp points to the condition variable in the object to use.
 * @param timeout is a timeout duration in ticks.
 * @return 0 for success, or an error number otherwise.
 */
static int queue(diminuto_readerwriter_t * rwp, const char * label, diminuto_list_t * np, role_t waiting, role_t pending, pthread_cond_t * conditionp, diminuto_ticks_t timeout)
{
    int rc = DIMINUTO_READERWRITER_ERROR;

    /*
     * Insert the node onto the tail of the list.
     */

    diminuto_list_dataset(np, (void *)waiting);
    rwp->waiting += 1;
    diminuto_list_insert(diminuto_list_prev(&(rwp->list)), np);

    DIMINUTO_LOG_DEBUG("diminuto_readerwriter: %s WAITING %dreading %dwriting %dwaiting", label, rwp->reading, rwp->writing, rwp->waiting);

    /*
     * Wait until this thread is signaled, while at the head of the list, and
     * specifically selected. Note that POSIX doesn't guarantee FIFO
     * behavior on the part of condition variables. Use a cleanup handler to
     * reconcile the node in the list if the caller is cancelled. We only
     * return from the wait for two reasons: the wait failed or the waiter
     * is ready (activated). It is possible (I think) that both can be true!
     */

    pthread_cleanup_push(wait_cleanup, np);

        rc = satisfy(rwp, np, pending, conditionp, timeout);

    pthread_cleanup_pop(0);

    /*
     * Remove the node from (presumably) the head of the list.
     */

    diminuto_list_remove(np);
    rwp->waiting -= 1;
    diminuto_list_dataset(np, (void *)RUNNING);

    DIMINUTO_LOG_DEBUG("diminuto_readerwriter: %s RUNNING %dreading %dwriting %dwaiting", label, rwp->reading, rwp->writing, rwp->waiting);

    return rc;
}

/**
 * Signal a waiting thread who is at the head of the list.
 * Prior to signaling the waiting thread, the state is changed from a
 * waiting state to a pending state.
 * @param rwp points to the Reader Writer object.
 * @param label is a string used for logging.
 * @param np points to the thread local list node for the calling thread.
 * @param pending is the pending role to be used: READING or WRITING.
 * @param conditionp points to the condition variable in the object to use.
 * @return 0 for success, or an error number otherwise.
 */
static int broadcast(diminuto_readerwriter_t * rwp, const char * label, diminuto_list_t * np, role_t pending, pthread_cond_t * conditionp)
{
    int rc = DIMINUTO_READERWRITER_ERROR;
    role_t role = NONE;

    /*
     * We have to set the new role before we broadcast, because
     * the waiting thread may wake up and run before we return
     * from the broadcast.
     */

    role = (role_t)diminuto_list_data(np);
    diminuto_list_dataset(np, (void *)pending);

    if ((rc = pthread_cond_broadcast(conditionp)) != 0) {
        errno = rc;
        diminuto_perror("broadcast: pthread_cond_broadcast");
        diminuto_list_dataset(np, (void *)role);
    } else {
        DIMINUTO_LOG_DEBUG("diminuto_readerwriter: %s SIGNALED %dreading %dwriting %dwaiting", label, rwp->reading, rwp->writing, rwp->waiting);
    }

    return rc;
}

/*******************************************************************************
 * ROLES
 ******************************************************************************/

/**
 * Suspend the caller until it is at the head of the list with the desired role.
 * @param rwp points to the Reader Writer object.
 * @param np points to the thread local list node for the calling thread.
 * @param role is the role of the calling thread: READER or WRITER.
 * @param timeout is a timeout duration in ticks.
 * @return 0 for success, or an errno number otherwise.
 */
static int suspend(diminuto_readerwriter_t * rwp, diminuto_list_t * np, role_t role, diminuto_ticks_t timeout)
{
    int rc = DIMINUTO_READERWRITER_ERROR;

    if (role == READER) {

        rc = queue(rwp, "Reader", np, READER, READING, &(rwp->reader), timeout);

    } else if (role == WRITER) {

        rc = queue(rwp, "Writer", np, WRITER, WRITING, &(rwp->writer), timeout);

    } else {

        /*
         * The caller passed a role that was neither READER nor WRITER.
         */

        errno = rc;
        diminuto_perror("diminuto_readerwriter: suspend: role");

    }

    return rc;
}

/**
 * Resume the thread at the head of the list whose role matches that
 * of the required role, or any role if the role is ANY.
 * @param rwp points to the Reader Writer object.
 * @param required is the required role: READER, WRITER, or ANY.
 * @return the role of the thread signaled: READER or WRITER.
 */
static role_t resume(diminuto_readerwriter_t * rwp, role_t required)
{
    role_t result = NONE;
    role_t role = NONE;
    diminuto_list_t * np = (diminuto_list_t *)0;

    if ((np = head(rwp)) != (diminuto_list_t *)0) {

        role = (role_t)diminuto_list_data(np);

        if ((role == READER) && ((required == READER) || (required == ANY))) {

            /*
             * The next waiter is a reader. Signal it. It will resume
             * the next waiter if it is also a reader. We do a broadcast
             * because POSIX doesn't guarantee FIFO behavior on the part
             * of condition variables. The signaled reader only resumes
             * if it is at the head of the list and it was placed into the
             * pending state.
             */

            if (broadcast(rwp, "Reader", np, READING, &(rwp->reader)) == 0) {
                result = READER;
            }

        } else if ((role == WRITER) && ((required == WRITER) || (required == ANY))) {

            /*
             * The next waiter is a writer. Signal it. When it is done
             * writing it will resume the next head of the list,
             * whatever it is. We do a broadcast because POSIX doesn't
             * guarantee FIFO behavior on the part of condition variables.
             * The signaled writer only resumes if it is at the head
             * of the list and it was placed into the pending state.
             */

            if (broadcast(rwp, "Writer", np, WRITING, &(rwp->writer)) == 0) {
                result = WRITER;
            }

        } else {

            /*
             * The rest of this is just error checking because I'm paranoid.
             */

            switch (required) {
            case READER:
            case WRITER:
            case ANY:
                break;
            default:
                /*
                 * The caller passed a role that wasn't READER, WRITER, or ANY,
                 * We emit an error message, but still pass back NONE.
                 */
                errno = DIMINUTO_READERWRITER_ERROR;
                diminuto_perror("diminuto_readerwriter: resume: required");
                break;
            }

            switch (role) {
            case READER:
            case WRITER:
            case READING:
            case WRITING:
                break;
            default:
                /*
                 * The state is invalid. This should be impossible.
                 * We emit an error message, but still pass back NONE.
                 */
                errno = DIMINUTO_READERWRITER_UNEXPECTED;
                diminuto_perror("diminuto_readerwriter: resume: state");
                break;
            }

            /*
             * If no error detected: either the thread at the head of the list
             * was not the role the caller wanted, or it is a thread that is
             * pending and has not yet been scheduled to run. This is a possible
             * state, and is especially likely for the multiple concurrent
             * readers scenario in which a subsequent reader has ended after a
             * prior reader has resumed the reader at the head of the list but
             * that reader has not yet run.
             */
        }

    }

    return result;
}

/*******************************************************************************
 * POLICY
 ******************************************************************************/

int diminuto_reader_begin_timed(diminuto_readerwriter_t * rwp, diminuto_ticks_t timeout)
{
    int result = -1;
    diminuto_list_t * np = (diminuto_list_t *)0;
    role_t role = NONE;

    if ((np = acquire()) == (diminuto_list_t *)0) {
        return result;
    }

    BEGIN_CRITICAL_SECTION(rwp);

        DIMINUTO_LOG_DEBUG("diminuto_readerwriter: Reader BEGIN enter %dreading %dwriting %dwaiting", rwp->reading, rwp->writing, rwp->waiting);

        diminuto_list_initif(&(rwp->list));
        diminuto_list_datasetif(&(rwp->list), (void *)rwp);

        if ((rwp->writing <= 0) && (head(rwp) == (diminuto_list_t *)0)) {

            /*
             * There are zero or more active writers and no one is waiting.
             * Reader can proceeed. Increment.
             */

            rwp->reading += 1;
            result = 0;

        } else if (timeout == DIMINUTO_READERWRITER_POLL) {

            /*
             * Reader specified zero timeout a.k.a. POLL.
             * We didn't pass the first test, so we're done.
             * Note that we do not emit an error message
             * (because this was deliberate on the part of
             * the caller) but we do set errno (so the reader
             * knows the nature of the failure).
             */

            errno = DIMINUTO_READERWRITER_TIMEDOUT;

        } else if (suspend(rwp, np, READER, timeout) == 0) {

            /*
             * Either there was an active writer or someone is waiting.
             * If someone is waiting, it is presumably a writer, since
             * a reader would not have waited. The reader count has
             * already been incremented by the thread that resumed
             * us.
             */

            result = 0;

        } else {

            /* An error message has already been emitted. */

        }

        /*
         * If we are running, a reader at the head of the list can
         * run as well. When such a reader runs, it will also check
         * for another reader at the head of the list.
         */

        if (result < 0) {

            /* Do nothing */

        } else if ((role = resume(rwp, READER)) == NONE) {

            /*
             * There isn't a reader at the head of the list to resume.
             */

        } else if (role == READER) {

            /*
             * There's another reader. It will in turn resume the following
             * reader if there is one. This is where a formerly suspended
             * and now resumed reader may get its increment.
             */

            rwp->reading += 1;

        } else {

            errno = DIMINUTO_READERWRITER_UNEXPECTED;
            diminuto_perror("diminuto_reader_begin: role");
            result = -1;

        }

        /*
         * Is is important for fairness and correctness (and fairly subtle
         * IMO) that the reader count already be incremented for any waiting
         * Reader that we resumed before we exit the critical section. Any
         * Reader that we resume will itself resume a Reader if it follows
         * that Reader in the wait list.
         */

        DIMINUTO_LOG_DEBUG("diminuto_readerwriter: Reader BEGIN exit %dreading %dwriting %dwaiting %d", rwp->reading, rwp->writing, rwp->waiting, result);

        if (rwp->fp != (FILE *)0) {
            dump(rwp->fp, rwp, "diminuto_reader_begin");
        }

        diminuto_assert(((result == 0) && (rwp->reading > 0) && (rwp->writing == 0) && (rwp->waiting >= 0)) || (result < 0));

    END_CRITICAL_SECTION;

    return result;
}

int diminuto_reader_end(diminuto_readerwriter_t * rwp)
{
    int result = -1;
    role_t role = NONE;

    BEGIN_CRITICAL_SECTION(rwp);

        DIMINUTO_LOG_DEBUG("diminuto_readerwriter: Reader END enter %dreading %dwriting %dwaiting", rwp->reading, rwp->writing, rwp->waiting);

        diminuto_assert((rwp->reading > 0) && (rwp->writing == 0));

        /*
         * Decrement.
         */

        rwp->reading -= 1;

        /*
         * Try to schedule the head of the list to run.
         */

        if (rwp->reading > 0) {

            /*
             * There are still readers running.
             */

            result = 0;

        } else if ((role = resume(rwp, ANY)) == NONE) {

            /*
             * This was the last reader, and there are no other readers
             * or writers waiting.
             */

            result = 0;

        } else if (role == READER) {

            /*
             * This shouldn't happen: a reader should never have waited.
             * We'll handle it anyway, but whine about it. This is where
             * a formerly suspended and now resumed reader may get its
             * increment.
             */

            errno = DIMINUTO_READERWRITER_UNEXPECTED;
            diminuto_perror("diminuto_reader_end: reader");
            rwp->reading += 1;
            result = 0;

        } else if (role == WRITER) {

            /*
             * We're resuming a writer who has been waiting. This is
             * where a formerly suspended and now resumed writer may get
             * its increment.
             */

            rwp->writing += 1;
            result = 0;

        } else {

            errno = DIMINUTO_READERWRITER_UNEXPECTED;
            diminuto_perror("diminuto_reader_end: role");

        }

        /*
         * Is is important for fairness and correctness (and fairly subtle IMO)
         * that the reader or writer count already be incremented for any
         * waiting Reader or Writer that we resumed before we exit the critical
         * section.
         */

        DIMINUTO_LOG_DEBUG("diminuto_readerwriter: Reader END exit %dreading %dwriting %dwaiting", rwp->reading, rwp->writing, rwp->waiting);

        if (rwp->fp != (FILE *)0) {
            dump(rwp->fp, rwp, "diminuto_reader_end");
        }

        diminuto_assert(((rwp->reading > 0) && (rwp->writing == 0) && (rwp->waiting >= 0)) || ((rwp->reading == 0) && (rwp->writing == 1) && (rwp->waiting >= 0)) || ((rwp->reading == 0) && (rwp->writing == 0) && (rwp->waiting == 0)));

    END_CRITICAL_SECTION;

    return result;
}

int diminuto_writer_begin_timed(diminuto_readerwriter_t * rwp, diminuto_ticks_t timeout)
{
    int result = -1;
    diminuto_list_t * np = (diminuto_list_t *)0;

    if ((np = acquire()) == (diminuto_list_t *)0) {
        return result;
    }

    BEGIN_CRITICAL_SECTION(rwp);

        DIMINUTO_LOG_DEBUG("diminuto_readerwriter: Writer BEGIN enter %dreading %dwriting %dwaiting", rwp->reading, rwp->writing, rwp->waiting);

        diminuto_list_initif(&(rwp->list));
        diminuto_list_datasetif(&(rwp->list), (void *)rwp);

        if ((rwp->reading <= 0) && (rwp->writing <= 0) && (head(rwp) == (diminuto_list_t *)0)) {

            /*
             * There are no active readers or writers and no one waiting.
             * Writer can proceed. Increment.
             */

            rwp->writing += 1;
            result = 0;

        } else if (timeout == DIMINUTO_READERWRITER_POLL) {

            /*
             * Writer specified zero timeout a.k.a. POLL.
             * We didn't pass the first test, so we're done.
             * Note that we do not emit an error message
             * (because this was deliberate on the part of
             * the caller) but we do set errno (so the writer
             * knows the nature of the failure).
             */

            errno = DIMINUTO_READERWRITER_TIMEDOUT;

        } else if (suspend(rwp, np, WRITER, timeout) == 0) {

            /*
             * Either there was at least active readers, an active
             * writer, or someone is waiting. (A waiter could be a thread
             * that has been resumed but has not run yet to remove itself
             * from the list.) The writer count has already been incremented
             * by the thread that resumed us.
             */

            result = 0;

        } else {

            /* An error message has already been emitted. */

        }

        DIMINUTO_LOG_DEBUG("diminuto_readerwriter: Writer BEGIN exit %dreading %dwriting %dwaiting %d", rwp->reading, rwp->writing, rwp->waiting, result);

        if (rwp->fp != (FILE *)0) {
            dump(rwp->fp, rwp, "diminuto_writer_begin");
        }

        diminuto_assert(((result == 0) && (rwp->reading == 0) && (rwp->writing == 1) && (rwp->waiting >= 0)) || (result < 0));

    END_CRITICAL_SECTION;

    return result;
}

int diminuto_writer_end(diminuto_readerwriter_t * rwp)
{
    int result = -1;
    role_t role = NONE;

    BEGIN_CRITICAL_SECTION(rwp);

        DIMINUTO_LOG_DEBUG("diminuto_readerwriter: Writer END enter %dreading %dwriting %dwaiting", rwp->reading, rwp->writing, rwp->waiting);

        diminuto_assert((rwp->reading == 0) && (rwp->writing == 1));

        /*
         * Decrement.
         */

        rwp->writing -= 1;

        /*
         * There should be no readers or writers running.
         * Try to schedule the head of the list to run.
         */

        if ((role = resume(rwp, ANY)) == NONE) {

            /*
             * There are no other readers or writers waiting.
             */

            result = 0;

        } else if (role == READER) {

            /*
             * We're activating a reader who has been waiting. This is
             * where a formerly suspended and now resumed reader may
             * gets its increment.
             */

            rwp->reading += 1;
            result = 0;

        } else if (role == WRITER) {

            /*
             * We're activating a writer who has been waiting. This is
             * where a formerly suspended and now resumed writer may
             * get its increment.
             */

            rwp->writing += 1;
            result = 0;

        } else {

            errno = DIMINUTO_READERWRITER_UNEXPECTED;
            diminuto_perror("diminuto_writer_end: role");

        }

        /*
         * Is is important for fairness and correctness (and fairly subtle IMO)
         * that the reader or writer count already be incremented for any
         * waiting Reader or Writer that we resumed before we exit the critical
         * section.
         */

        DIMINUTO_LOG_DEBUG("diminuto_readerwriter: Writer END exit %dreading %dwriting %dwaiting", rwp->reading, rwp->writing, rwp->waiting);

        if (rwp->fp != (FILE *)0) {
            dump(rwp->fp, rwp, "diminuto_writer_end");
        }

        diminuto_assert(((rwp->reading > 0) && (rwp->writing == 0) && (rwp->waiting >= 0)) || ((rwp->reading == 0) && (rwp->writing == 1) && (rwp->waiting >= 0)) || ((rwp->reading == 0) && (rwp->writing == 0) && (rwp->waiting == 0)));

    END_CRITICAL_SECTION;

    return result;
}

/*******************************************************************************
 * CALLBACKS
 ******************************************************************************/

void diminuto_reader_cleanup(void * vp)
{
    diminuto_readerwriter_t * rwp = (diminuto_readerwriter_t *)vp;

    (void)diminuto_reader_end(rwp);
}

void diminuto_writer_cleanup(void * vp)
{
    diminuto_readerwriter_t * rwp = (diminuto_readerwriter_t *)vp;

    (void)diminuto_writer_end(rwp);
}

/*******************************************************************************
 * DEBUGGING
 ******************************************************************************/

FILE * diminuto_readerwriter_debug(diminuto_readerwriter_t * rwp, FILE * fp)
{
    FILE * oldfp = (FILE *)0;

    BEGIN_CRITICAL_SECTION(rwp);

        oldfp = rwp->fp;
        rwp->fp = fp;

    END_CRITICAL_SECTION;

    return oldfp;
}
