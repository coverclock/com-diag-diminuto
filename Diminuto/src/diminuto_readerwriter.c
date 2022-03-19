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
 *
 * This is Generation 3 of the Reader Writer feature. The implementation of
 * the various generations vary significantly, although the API and its
 * semantics (at least from the caller's point of view) remains the same.
 * The differences between the generations represent successive optimizations
 * ("Make it work; then make it fast.").
 *
 * Generation 2 replaced the token ring buffer with the doubly-linked wait
 * list.
 *
 * Generation 3 removed activated threads immediately from the wait list
 * and updated the Reader Writer lock state rather than wait for the threads
 * to activate and do it themselves. It also replaced the condition variable
 * broadcast signal done by every activating thread by a single broadcast
 * signal for a bulk activation (this made a big difference for readers).
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
 * TYPES
 ******************************************************************************/

typedef diminuto_readerwriter_role_t role_t;

/*******************************************************************************
 * STATICS
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
 * GENERATORS
 ******************************************************************************/

/**
 * @def BEGIN_CRITICAL_SECTION
 * This is the opening bracket of a critical section using the mutex
 * in the Reader Writer object pointed to by @a _RWP_.
 */
#define BEGIN_CRITICAL_SECTION(_RWP_) \
    do { \
        diminuto_readerwriter_t * diminuto_readerwriter_rwp; \
        int diminuto_readerwriter_rc; \
        diminuto_readerwriter_rwp = (_RWP_); \
        if ((diminuto_readerwriter_rc = pthread_mutex_lock(&(diminuto_readerwriter_rwp->mutex))) != 0) { \
            errno = diminuto_readerwriter_rc; \
            diminuto_perror("diminuto_readerwriter: BEGIN_CRITICAL_SECTION: pthread_mutex_lock"); \
        } else { \
            pthread_cleanup_push(mutex_cleanup, diminuto_readerwriter_rwp); \
            do { \
                ((void)0)

/**
 * @def END_CRITICAL_SECTION
 * This is the closing bracket of a critical section using the mutex
 * in the Reader Writer object.
 */
#define END_CRITICAL_SECTION \
            } while (0); \
            pthread_cleanup_pop(!0); \
        } \
    } while (0)

/*******************************************************************************
 * QUEUE
 ******************************************************************************/

/**
 * Return the node in the wait list that is the closest to the head of the list
 * without being pending. If the wait list is empty, this will be the head of
 * the list, not a node on the list. A new node, representing a soon-to-be
 * waiting thread, that requests priority scheduling, will be scheduled before
 * the returned node.
 * @param rwp points to the Reader Writer object.
 * @return the node after which a priority node can be inserted.
 */
static diminuto_list_t * front(diminuto_readerwriter_t * rwp)
{
    diminuto_list_t * np = (diminuto_list_t *)0;
    role_t role = NONE;

    for (np = diminuto_list_next(&(rwp->list)); !diminuto_list_isroot(np); np = diminuto_list_next(np)) {
        role = (role_t)diminuto_list_data(np);
        if (role == READER) {
            break;
        } else if (role == WRITER) {
            break;
        } else if (role == FAILED) {
            break;
        } else {
            /* Do nothing. */
        }
    }

    return np;
}

/**
 * Insert a node onto the wait list.
 * @param rwp points to the Reader Writer object.
 * @param np points to the node to be inserted.
 * @param priority is true if high priority, false otherwise.
 */
static inline void enqueue(diminuto_readerwriter_t * rwp, diminuto_list_t * np, int priority)
{
    rwp->waiting += 1; /* WAITING COUNTER INCREMENT */
    if (rwp->waiting > rwp->maximum) { rwp->maximum = rwp->waiting; }
    (void)diminuto_list_insert(diminuto_list_prev(priority ? front(rwp) : &(rwp->list)), np);
}

/**
 * Remove a node from the wait list.
 * @param rwp points to the Reader Writer object.
 * @param np points to the node to be removed.
 */
static inline void dequeue(diminuto_readerwriter_t * rwp, diminuto_list_t * np)
{
    (void)diminuto_list_remove(np);
    rwp->waiting -= 1; /* WAITING COUNTER DECREMENT */
}

/**
 * Return true if the state of the indicated node in the list is now pending,
 * indicating that the waiting reader or writer has been activated, false
 * otherwise. Calling head has the side effect of removing any failed threads
 * off the head of the list.
 * @param np points to the list node for the calling thread.
 * @param pending is the pending role to be used: READABLE or WRITABLE.
 * @return true if the token is both head and pending, false otherwise.
 */
static inline int ready(diminuto_list_t * np, role_t pending)
{
    return ((role_t)diminuto_list_data(np) == pending);
}

/**
 * Return the node at the head of the wait list without removing it,
 * after first discarding any leading failed nodes. This can also be
 * used just to discard leading failed nodes. Nodes on the wait list can be
 * in the failing state because they were cancelled, but could not be
 * removed from the wait list because the caller did not hold the mutex.
 * @param rwp points to the Reader Writer object.
 * @return a pointer to the node at the head of the wait list or NULL if empty.
 */
static diminuto_list_t * head(diminuto_readerwriter_t * rwp)
{
    diminuto_list_t * np = (diminuto_list_t *)0;

    while (((np = diminuto_list_head(&(rwp->list))) != (diminuto_list_t *)0) && ((role_t)diminuto_list_data(np) == FAILED)) {
        dequeue(rwp, np);
        DIMINUTO_LOG_DEBUG("diminuto_readerwriter: Failed %p REMOVED %dreading %dwriting %dwaiting", np, rwp->reading, rwp->writing, rwp->waiting);
    }

    return np;
}

/*******************************************************************************
 * INTERNAL CALLBACKS
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
 * seem pointless, doesn't it? But it keeps valgrind(1) happy, since otherwise
 * it complains about the Reader Writer unit test leaving dynamically allocated
 * memory behind that is part of the implicit "main" thread that participates
 * in the early unit tests, before explicit threads are started.
 */
static void exit_cleanup()
{
    void * vp = (void *)0;

    if ((vp = pthread_getspecific(key)) != (void *)0) {
        free(vp);
        DIMINUTO_LOG_DEBUG("diminuto_readerwriter: Process %p RELEASED", vp);
    }
}

/**
 * Clean up the state if an abnormal wait occurs.
 * @param vp points to the thread local object for the associated thread.
 */
static void wait_cleanup(void * vp)
{
    diminuto_list_t * np = (diminuto_list_t *)vp;

    /*
     * I grapple with how thread safe this code is, and how safe it needs
     * to be. The fact that all modifications of the Reader Writer list
     * should be done in a critical section, such sections implement
     * appropriate read and write memory barriers, the reading of the root
     * and data fields in the structure should be atomic, and the root and
     * data fields are not modified after initialization, assuages my concern
     * somewhat.
     */

    if (!diminuto_list_isroot(np)) {
        diminuto_readerwriter_t * rwp = (diminuto_readerwriter_t *)diminuto_list_data(diminuto_list_root(np));

        BEGIN_CRITICAL_SECTION(rwp);

            dequeue(rwp, np);
            diminuto_list_dataset(np, (void *)FAILED);

        END_CRITICAL_SECTION;
    }
}

/**
 * Release the object pointed to by the Reader Writer thread local storage.
 * Typically this has to be done when a thread terminates, for example by
 * exiting or by cancellation. In other languages, this would be known as
 * a destructor.
 * @param vp points to the thread local object for the terminating thread.
 */
static void key_cleanup(void * vp)
{
    wait_cleanup(vp);
    free(vp);
    DIMINUTO_LOG_DEBUG("diminuto_readerwriter: Thread %p RELEASED", vp);
}

/*******************************************************************************
 * STORAGE
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
            DIMINUTO_LOG_DEBUG("diminuto_readerwriter: Process %u INITIALIZED", key);
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
        DIMINUTO_LOG_DEBUG("diminuto_readerwriter: Thread %p ACQUIRED", np);
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
        rwp->debugging = 0;
        rwp->reading = 0;
        rwp->writing = 0;
        rwp->waiting = 0;
        rwp->maximum = 0;
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
 * AUDITOR
 ******************************************************************************/

/**
 * Display information about the internal state of the Reader Writer object
 * to the log. As a side effect, audit the object and enforce its invariants.
 * @param rwp points to the Reader Writer object.
 * @param label is a string displayed to identify who is calling the function.
 */
static void audit(diminuto_readerwriter_t * rwp, const char * label)
{
    int save = errno;
    diminuto_list_t * np = (diminuto_list_t *)0;
    diminuto_list_t * rp = (diminuto_list_t *)0;
    char buffer[DIMINUTO_LOG_BUFFER_MAXIMUM] = { '\0', };
    char * bp = (char *)0;
    size_t size = 0;
    int length = 0;
    unsigned int readable = 0;
    unsigned int writable = 0;
    unsigned int readers = 0;
    unsigned int writers = 0;
    unsigned int started = 0;
    unsigned int failed = 0;
    unsigned int running = 0;
    unsigned int unknown = 0;
    unsigned int queued = 0;
    role_t role = NONE;

    /*
     * Audit the list.
     */

    diminuto_assert(diminuto_list_audit(&(rwp->list)) == (diminuto_list_t *)0);

    /*
     * Audit the object.
     */

    bp = &(buffer[0]);
    size = sizeof(buffer) - 1;

    length = snprintf(bp, size, "%s audit@%p[%zu]", label, rwp, sizeof(*rwp));
    if ((0 < length) && (length < size)) { bp += length; size -= length; }

    /*
     * These counters are part of the Reader Writer object state.
     */

    length = snprintf(bp, size, " %dreading", rwp->reading);
    if ((0 < length) && (length < size)) { bp += length; size -= length; }

    length = snprintf(bp, size, " %dwriting", rwp->writing);
    if ((0 < length) && (length < size)) { bp += length; size -= length; }

    length = snprintf(bp, size, " %dwaiting", rwp->waiting);
    if ((0 < length) && (length < size)) { bp += length; size -= length; }

    length = snprintf(bp, size, " %dmaximum {", rwp->maximum);
    if ((0 < length) && (length < size)) { bp += length; size -= length; }

    /*
     * Step through the wait list.
     */

    rp = diminuto_list_root(&(rwp->list));
    np = diminuto_list_next(rp);
    while (np != rp) {
        role = (role_t)diminuto_list_data(np);
        switch (role) {
            case READABLE: ++readable; break;
            case WRITABLE: ++writable; break;
            case READER:   ++readers;  break;
            case WRITER:   ++writers;  break;
            case STARTED:  ++started;  break;
            case FAILED:   ++failed;   break;
            case RUNNING:  ++running;  break;
            default:       ++unknown; role = UNKNOWN; break;
        }
        length = snprintf(bp, size, "%c", role);
        if ((0 < length) && (length < size)) { bp += length; size -= length; }
        queued += 1;
        np = diminuto_list_next(np);
    }

    /*
     * These counters may be zero or greater.
     */

    length = snprintf(bp, size, "} %dqueued", queued);
    if ((0 < length) && (length < size)) { bp += length; size -= length; }

    length = snprintf(bp, size, " %dreaders", readers);
    if ((0 < length) && (length < size)) { bp += length; size -= length; }

    length = snprintf(bp, size, " %dwriters", writers);
    if ((0 < length) && (length < size)) { bp += length; size -= length; }

    length = snprintf(bp, size, " %dfailed", failed);
    if ((0 < length) && (length < size)) { bp += length; size -= length; }

    /*
     * These counters should always be zero.
     */

    length = snprintf(bp, size, " %dstarted", started);
    if ((0 < length) && (length < size)) { bp += length; size -= length; }

    length = snprintf(bp, size, " %dreadable", readable);
    if ((0 < length) && (length < size)) { bp += length; size -= length; }

    length = snprintf(bp, size, " %dwritable", writable);
    if ((0 < length) && (length < size)) { bp += length; size -= length; }

    length = snprintf(bp, size, " %drunning", running);
    if ((0 < length) && (length < size)) { bp += length; size -= length; }

    length = snprintf(bp, size, " %dunknown", unknown);
    if ((0 < length) && (length < size)) { bp += length; size -= length; }

    *(bp) = '\0';

    DIMINUTO_LOG_DEBUG("%s\n", buffer); /* Warning otherwise. */

    /*
     * The reading and writing invariants are enforced every time RW
     * is used, whether audit() is called or not, so we don't check
     * them again here.
     */

    diminuto_assert(rwp->reading >= 0);
    diminuto_assert(rwp->writing >= 0);
    diminuto_assert(rwp->waiting >= 0);
    diminuto_assert(rwp->maximum >= 0);

    diminuto_assert(rwp->waiting <= rwp->maximum);

    diminuto_assert(queued  >= 0);
    diminuto_assert(readers >= 0);
    diminuto_assert(writers >= 0);
    diminuto_assert(failed  >= 0);

    diminuto_assert(queued == rwp->waiting);

    /*
     * None of these states should have been on the wait list.
     */

    diminuto_assert(readable == 0);
    diminuto_assert(writable == 0);
    diminuto_assert(started  == 0);
    diminuto_assert(running  == 0);
    diminuto_assert(unknown  == 0);

    errno = save;
}

/*******************************************************************************
 * SCHEDULING
 ******************************************************************************/

/**
 * Perform either an infinite wait or a timed wait as specified. If a timed
 * wait, compute the absolute clocktime for the timed wait based on the
 * relative timeout duration. This allows the caller to specify a relative
 * timeout duration that applies no matter how many times the timed wait
 * has to be repeated after receiving a broadcast.
 * @param rwp points to the Reader Writer object.
 * @param np points to the thread local list node for the calling thread.
 * @param pending is the pending role to be used: READABLE or WRITABLE.
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

        pthread_cleanup_push(wait_cleanup, np);

            do {
                if ((rc = pthread_cond_wait(conditionp, &(rwp->mutex))) != 0) {
                    errno = rc;
                    diminuto_perror("diminuto_readerwriter: satisfy: pthread_cond_wait");
                    break;
                }
            } while (!ready(np, pending));

        pthread_cleanup_pop(0);

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

        pthread_cleanup_push(wait_cleanup, np);

            do {
                if ((rc = pthread_cond_timedwait(conditionp, &(rwp->mutex), &absolutetime)) == 0) {
                    /* Do nothing. */
                } else if (rc != ETIMEDOUT) {
                    errno = rc;
                    diminuto_perror("diminuto_readerwriter: satisfy: pthread_cond_timedwait");
                    break;
                } else {
                    errno = rc;
                    /* Suppress error message but return error number. */
                    break;
                }
            } while (!ready(np, pending));

        pthread_cleanup_pop(0);

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
 * @param pending is the pending role to be used: READABLE or WRITABLE.
 * @param conditionp points to the condition variable in the object to use.
 * @param timeout is a timeout duration in ticks.
 * @param priority if true gives the caller priority in the list.
 * @return 0 for success, or an error number otherwise.
 */
static int schedule(diminuto_readerwriter_t * rwp, const char * label, diminuto_list_t * np, role_t waiting, role_t pending, pthread_cond_t * conditionp, diminuto_ticks_t timeout, int priority)
{
    int rc = DIMINUTO_READERWRITER_ERROR;
    role_t role = NONE;

    /*
     * Insert the node onto the list. It is either inserted at the end
     * (normal priority) or as close to the front as possible (high priority).
     */

    diminuto_list_dataset(np, (void *)waiting);
    enqueue(rwp, np, priority);

    DIMINUTO_LOG_DEBUG("diminuto_readerwriter: %s WAITING %s %s %dreading %dwriting %dwaiting", label, (timeout == DIMINUTO_READERWRITER_INFINITY) ? "Inf" : (timeout == 0) ? "Pol" : "Tim", priority ? "Hi" : "Lo", rwp->reading, rwp->writing, rwp->waiting);

    /*
     * Wait until this thread is signaled and activated. Note that POSIX
     * doesn't guarantee FIFO behavior on the part of condition variables.
     *
     * We only return from the wait for two reasons: the wait failed or the
     * waiting thread is ready (a.k.a. activated). It is possible (I think)
     * that both can be true!
     *
     * If both are true, then the the reading or writing counter has already
     * been incremented, and the thread has already been removed from the wait
     * list. Effectively, it has succesfully waited, and we can ignore the
     * error.
     *
     * If the thread has NOT been activated, we must remove it from the wait
     * list. And in any case, the thread is now running.
     */

    rc = satisfy(rwp, np, pending, conditionp, timeout);
    if (rc != 0) {
        role = (role_t)diminuto_list_data(np);
        if (role == READABLE) {
            DIMINUTO_LOG_WARNING("diminuto_readerwriter: Reader ready AND timedout!");
            rc = 0;
        } else if (role == WRITABLE) {
            DIMINUTO_LOG_WARNING("diminuto_readerwriter: Writer ready AND timedout!");
            rc = 0;
        } else {
            dequeue(rwp, np);
        }
    }

    diminuto_list_dataset(np, (void *)RUNNING);

    DIMINUTO_LOG_DEBUG("diminuto_readerwriter: %s %s %dreading %dwriting %dwaiting", label, (rc == 0) ? "ACTIVATED" : "TIMEDOUT", rwp->reading, rwp->writing, rwp->waiting);

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
 * @param priority if true gives the caller priority in the list.
 * @return 0 for success, or an errno number otherwise.
 */
static int suspend(diminuto_readerwriter_t * rwp, diminuto_list_t * np, role_t role, diminuto_ticks_t timeout, int priority)
{
    int rc = DIMINUTO_READERWRITER_ERROR;

    if (role == READER) {

        rc = schedule(rwp, "Reader", np, READER, READABLE, &(rwp->reader), timeout, priority);

    } else if (role == WRITER) {

        rc = schedule(rwp, "Writer", np, WRITER, WRITABLE, &(rwp->writer), timeout, priority);

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
    pthread_cond_t * cp = (pthread_cond_t *)0;
    int rc = -1;

    /*
     * In the case of readers, we activate all of them at the head of
     * the list. In the case of a writer, we activate only the first one
     * at the head of the list. head() automatically cleans up failed
     * threads off the list. We signal the appropriate condition variable,
     * for readers or for writers once we're done iterating.
     */

    while ((np = head(rwp)) != (diminuto_list_t *)0) {

        role = (role_t)diminuto_list_data(np);
        if ((role == READER) && ((required == READER) || (required == ANY))) {

            /*
             * The next waiter is a reader. Remove it from the queue, place
             * it in the pending state, and remember to signal it. Continue
             * to look for more readers behind this one. Once we activate a
             * reader, we can only activate readers subsequently, so we
             * must update the required role.
             */

            dequeue(rwp, np);
            diminuto_list_dataset(np, (void *)READABLE);
            rwp->reading += 1; /* READING COUNTER INCREMENT */
            cp = &(rwp->reader);
            result = role;
            required = role;
            DIMINUTO_LOG_DEBUG("diminuto_readerwriter: Reader ACTIVATED %dreading %dwriting %dwaiting", rwp->reading, rwp->writing, rwp->waiting);
            continue;

        } else if ((role == WRITER) && ((required == WRITER) || (required == ANY))) {

            /*
             * The next waiter is a writer. Remove it from the queue, place
             * it into the pending state, and remember to signal it.
             */

            dequeue(rwp, np);
            diminuto_list_dataset(np, (void *)WRITABLE);
            rwp->writing += 1; /* WRITING COUNTER INCREMENT */
            cp = &(rwp->writer);
            result = role;
            DIMINUTO_LOG_DEBUG("diminuto_readerwriter: Writer ACTIVATED %dreading %dwriting %dwaiting", rwp->reading, rwp->writing, rwp->waiting);
            break;

        } else {

            /*
             * Either we have activated a bunch of readers, or one writer,
             * or there was no viable candidate on the list.
             */
            break;

        }

    }

    /*
     * The conditional variable pointer may point to the reader condition, the
     * writer condition, or may be NULL.
     */

    if (cp == (pthread_cond_t *)0) {
        /* Do nothing. */
    } else if ((rc = pthread_cond_broadcast(cp)) == 0) {
        /* Do nothing. */
    } else {
        errno = rc;
        diminuto_perror("broadcast: pthread_cond_broadcast");
        result = NONE;
    }

    return result;
}

/*******************************************************************************
 * POLICY
 ******************************************************************************/

int diminuto_reader_begin_f(diminuto_readerwriter_t * rwp, diminuto_ticks_t timeout, int priority)
{
    int result = -1;
    diminuto_list_t * np = (diminuto_list_t *)0;
    role_t role = NONE;
    int rc = 0;

    if ((np = acquire()) == (diminuto_list_t *)0) {
        return result;
    }

    BEGIN_CRITICAL_SECTION(rwp);

        DIMINUTO_LOG_DEBUG("diminuto_readerwriter: Reader BEGIN enter %dreading %dwriting %dwaiting", rwp->reading, rwp->writing, rwp->waiting);

        diminuto_list_datasetif(diminuto_list_initif(&(rwp->list)), (void *)rwp);

        if ((rwp->writing <= 0) && (head(rwp) == (diminuto_list_t *)0)) {

            /*
             * There are zero or less active writers and no one is waiting.
             * Reader can proceeed. Increment.
             */

            rwp->reading += 1; /* READING COUNTER INCREMENT */
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

        } else if ((rc = suspend(rwp, np, READER, timeout, priority)) == 0) {

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
            errno = rc;

        }

        /*
         * We are running. Either we did not wait and incremented
         * the reading count ourselves, or we waited, and another
         * thread resumed us and incremented the reading count.
         *
         * Is is important for fairness and correctness (and fairly subtle
         * IMO) that the reading or writing count already be incremented
         * for any waiting Reader or Writer that we resumed before we exit
         * the critical section.
         */

        DIMINUTO_LOG_DEBUG("diminuto_readerwriter: Reader BEGIN exit %dreading %dwriting %dwaiting %d", rwp->reading, rwp->writing, rwp->waiting, result);

        if (rwp->debugging) {
            audit(rwp, "diminuto_readerwriter: Reader BEGIN");
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

        rwp->reading -= 1; /* READING COUNTER DECREMENT */

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
             * We're resuming one or more readers who who have been
             * waiting. It used to be this couldn't happen, but since
             * I added timeouts, it can if an impatient writer at the
             * head of the queue times out ahead of a waiting reader
             * behind it.
             */

            result = 0;

        } else if (role == WRITER) {

            /*
             * We're resuming a writer who has been waiting.
             */

            result = 0;

        } else {

            errno = DIMINUTO_READERWRITER_UNEXPECTED;
            diminuto_perror("diminuto_reader_end: role");

        }

        /*
         * Is is important for fairness and correctness (and fairly subtle
         * IMO) that the reading or writing count already be incremented
         * for any waiting Reader or Writer that we resumed before we exit
         * the critical section.
         */

        DIMINUTO_LOG_DEBUG("diminuto_readerwriter: Reader END exit %dreading %dwriting %dwaiting", rwp->reading, rwp->writing, rwp->waiting);

        if (rwp->debugging) {
            audit(rwp, "diminuto_readerwriter: Reader END");
        }

        diminuto_assert(((rwp->reading > 0) && (rwp->writing == 0) && (rwp->waiting >= 0)) || ((rwp->reading == 0) && (rwp->writing == 1) && (rwp->waiting >= 0)) || ((rwp->reading == 0) && (rwp->writing == 0) && (rwp->waiting == 0)));

    END_CRITICAL_SECTION;

    return result;
}

int diminuto_writer_begin_f(diminuto_readerwriter_t * rwp, diminuto_ticks_t timeout, int priority)
{
    int result = -1;
    diminuto_list_t * np = (diminuto_list_t *)0;
    int rc = 0;

    if ((np = acquire()) == (diminuto_list_t *)0) {
        return result;
    }

    BEGIN_CRITICAL_SECTION(rwp);

        DIMINUTO_LOG_DEBUG("diminuto_readerwriter: Writer BEGIN enter %dreading %dwriting %dwaiting", rwp->reading, rwp->writing, rwp->waiting);

        diminuto_list_datasetif(diminuto_list_initif(&(rwp->list)), (void *)rwp);

        if ((rwp->reading <= 0) && (rwp->writing <= 0) && (head(rwp) == (diminuto_list_t *)0)) {

            /*
             * There are no active readers nor writer and no one waiting.
             * Writer can proceed. Increment.
             */

            rwp->writing += 1; /* WRITING COUNTER INCREMENT */
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

        } else if ((rc = suspend(rwp, np, WRITER, timeout, priority)) == 0) {

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
            errno = rc;

        }

        /*
         * We are running. Either we did not wait and incremented
         * the reading count ourselves, or we waited, and another
         * thread resumed us and incremented the reading count.
         *
         * Is is important for fairness and correctness (and fairly subtle
         * IMO) that the reading or writing count already be incremented
         * for any waiting Reader or Writer that we resumed before we exit
         * the critical section.
         */

        DIMINUTO_LOG_DEBUG("diminuto_readerwriter: Writer BEGIN exit %dreading %dwriting %dwaiting %d", rwp->reading, rwp->writing, rwp->waiting, result);

        if (rwp->debugging) {
            audit(rwp, "diminuto_readerwriter: Writer BEGIN");
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

        rwp->writing -= 1; /* WRITING COUNTER DECREMENT */

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
             * We're activating one or more readers who have
             * been waiting.
             */

            result = 0;

        } else if (role == WRITER) {

            /*
             * We're activating a writer who has been waiting.
             */

            result = 0;

        } else {

            errno = DIMINUTO_READERWRITER_UNEXPECTED;
            diminuto_perror("diminuto_writer_end: role");

        }

        /*
         * Is is important for fairness and correctness (and fairly subtle
         * IMO) that the reading or writing count already be incremented
         * for any waiting Reader or Writer that we resumed before we exit
         * the critical section.
         */

        DIMINUTO_LOG_DEBUG("diminuto_readerwriter: Writer END exit %dreading %dwriting %dwaiting", rwp->reading, rwp->writing, rwp->waiting);

        if (rwp->debugging) {
            audit(rwp, "diminuto_readerwriter: Writer END");
        }

        diminuto_assert(((rwp->reading > 0) && (rwp->writing == 0) && (rwp->waiting >= 0)) || ((rwp->reading == 0) && (rwp->writing == 1) && (rwp->waiting >= 0)) || ((rwp->reading == 0) && (rwp->writing == 0) && (rwp->waiting == 0)));

    END_CRITICAL_SECTION;

    return result;
}

/*******************************************************************************
 * EXTERNAL CALLBACKS
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

int diminuto_readerwriter_debug(diminuto_readerwriter_t * rwp, int debugging)
{
    int prior = 0;

    BEGIN_CRITICAL_SECTION(rwp);

        prior = rwp->debugging;
        rwp->debugging = !!debugging;

    END_CRITICAL_SECTION;

    return prior;
}

int diminuto_readerwriter_maximum(diminuto_readerwriter_t * rwp)
{
    int maximum = 0;

    BEGIN_CRITICAL_SECTION(rwp);

        maximum = rwp->maximum;
        rwp->maximum = rwp->waiting;

    END_CRITICAL_SECTION;

    return maximum;
}

/*******************************************************************************
 * PRIVATE
 ******************************************************************************/

#if !0

void diminuto_readerwriter_enqueue(diminuto_readerwriter_t * rwp, diminuto_list_t * np, int priority)
{
    enqueue(rwp, np, priority);
}

void diminuto_readerwriter_dequeue(diminuto_readerwriter_t * rwp, diminuto_list_t * np)
{
    dequeue(rwp, np);
}

diminuto_list_t * diminuto_readerwriter_head(diminuto_readerwriter_t * rwp)
{
    return head(rwp);
}

diminuto_list_t * diminuto_readerwriter_front(diminuto_readerwriter_t * rwp)
{
   return front(rwp);
} 

int diminuto_readerwriter_ready(diminuto_list_t * np, diminuto_readerwriter_role_t pending)
{
    return ready(np, pending);
}

#endif
