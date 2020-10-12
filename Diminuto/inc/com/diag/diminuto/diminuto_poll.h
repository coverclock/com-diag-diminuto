/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_POLL_
#define _H_COM_DIAG_DIMINUTO_POLL_

/**
 * @file
 *
 * Copyright 2015 Digital Aggregates Corporation, Colorado, USA.
 * Licensed under the terms in LICENSE.txt.
 *
 * The Poll feature is a socket multiplexer that uses the ppoll(2) system
 * call instead of the pselect(2) system call used by the mux feature. This
 * feature exists mostly because I wanted to benchmark ppoll(2) against
 * pselect(2) using the same abstraction and a similar API. But it isn't
 * as efficient as it could be since it sits on top of the mux feature and
 * only differs in the wait function.
 *
 * NOTE: Because the diminuto_mux feature uses the pselect(2) system call, it
 * is limited to using the first 1024 file descriptors enumerated 0 to 1023.
 * This is a limit imposed by the size of the fd_set bit mask. The ppoll(2)
 * system call circumvents this limit, but alas even though the diminuto_poll
 * feature uses this system call instead, it still uses the fd_set bit mask
 * to manage file descriptors, hence it has the same limitation.
 */

#include "com/diag/diminuto/diminuto_mux.h"
#include <poll.h>

/**
 * @def COM_DIAG_DIMINUTO_POLL_FREQUENCY
 * This manifest constant is the frequency in Hertz at which this feature
 * operates. The inverse of this value is the smallest unit of time in fractions
 * of a second that this feature can express or use. This constant is provided
 * for use in those cases where it is useful to have the value at compile time.
 * However, you should always prefer to use the inline function when possible.
 */
#define COM_DIAG_DIMINUTO_POLL_FREQUENCY COM_DIAG_DIMINUTO_MUX_FREQUENCY

/**
 * Return the resolution of the Diminuto poller timeout units in ticks per
 * second (Hertz). Timeout durations smaller than the equivalent period in ticks
 * may not yield the expected results. In fact, there is no guarantee that the
 * underlying software platform or hardware target can support timeout durations
 * with even this resolution.
 * @return the resolution in ticks per second.
 */
static inline diminuto_sticks_t diminuto_poll_frequency(void) {
    return diminuto_mux_frequency();
}

/**
 * This is the poller state.
 */
typedef struct DiminutoPoll {
    diminuto_mux_t mux;
    struct pollfd * pollfd;
    nfds_t nfds;
    int min;
    int max;
    int refresh;
} diminuto_poll_t;

/**
 * Initialize a poller. Any prior state is lost, although this does not
 * effect any file descriptors. The poller inherits the current signal
 * mask of the caller such that signals that are blocked when the poller
 * is initialized will continue to be blocked when the poller is waiting
 * (in other words, blocked signals are automatically registered).
 * Signals can be removed from this cached mask by unregistering them.
 * @param pollp points to a poller structure.
 * @return a pointer to the object or NULL if an error occurred.
 */
extern diminuto_poll_t * diminuto_poll_init(diminuto_poll_t * pollp);

/**
 * Release any dynamically acquired storage associated with the poller.
 * @param pollp points to a poller structure.
 * @return NULL or a pointer to the object if an error occurred.
 */
extern diminuto_poll_t * diminuto_poll_fini(diminuto_poll_t * pollp);

/**
 * Register a file descriptor with the poller for reading.
 * @param pollp points to an initialized poller structure.
 * @param fd is an unregistered file descriptor.
 * @return >=0 for success, <0 for error.
 */
static inline int diminuto_poll_register_read(diminuto_poll_t * pollp, int fd) {
    int rc;
    rc = diminuto_mux_register_read(&pollp->mux, fd);
    if (rc >= 0) { pollp->refresh = !0; }
    return rc;
}

/**
 * Register a file descriptor with the poller for writing.
 * @param pollp points to an initialized poller structure.
 * @param fd is an unregistered file descriptor.
 * @return >=0 for success, <0 for error.
 */
static inline int diminuto_poll_register_write(diminuto_poll_t * pollp, int fd) {
    int rc;
    rc = diminuto_mux_register_write(&pollp->mux, fd);
    if (rc >= 0) { pollp->refresh = !0; }
    return rc;
}

/**
 * Register a listening file descriptor with the poller for accepting
 * connections.
 * @param pollp points to an initialized poller structure.
 * @param fd is an unregistered file descriptor.
 * @return >=0 for success, <0 for error.
 */
static inline int diminuto_poll_register_accept(diminuto_poll_t * pollp, int fd) {
    int rc;
    rc = diminuto_mux_register_accept(&pollp->mux, fd);
    if (rc >= 0) { pollp->refresh = !0; }
    return rc;
}

/**
 * Register a file descriptor with the poller for urgent exceptions, which
 * typically means for urgent out-of-band one byte receives on a stream socket.
 * @param pollp points to an initialized poller structure.
 * @param fd is an unregistered file descriptor.
 * @return >=0 for success, <0 for error.
 */
static inline int diminuto_poll_register_urgent(diminuto_poll_t * pollp, int fd) {
    int rc;
    rc = diminuto_mux_register_urgent(&pollp->mux, fd);
    if (rc >= 0) { pollp->refresh = !0; }
    return rc;
}

/**
 * Register a file descriptor with the poller for interrupt exceptions, which
 * typically means for state changes on a general purpose input/output (GPIO)
 * pin configured to interrupt on edge or level transitions.
 * @param pollp points to an initialized poller structure.
 * @param fd is an unregistered file descriptor.
 * @return >=0 for success, <0 for error.
 */
static inline int diminuto_poll_register_interrupt(diminuto_poll_t * pollp, int fd) {
    int rc;
    rc = diminuto_mux_register_interrupt(&pollp->mux, fd);
    if (rc >= 0) { pollp->refresh = !0; }
    return rc;
}

/**
 * Add a signal to the mask of those to be atomically blocked while the
 * poller is waiting.
 * @param pollp points to an initialized poller structure.
 * @param signum is an unregistered blocked to be blocked.
 * @return >=0 for success, <0 for error.
 */
static inline int diminuto_poll_register_signal(diminuto_poll_t * pollp, int signum) {
    return diminuto_mux_register_signal(&pollp->mux, signum);
}

/**
 * Unregister a file descriptor from the poller for reading.
 * @param pollp points to an initialized poller structure.
 * @param fd is an registered file descriptor.
 * @return >=0 for success, <0 for error.
 */
static inline int diminuto_poll_unregister_read(diminuto_poll_t * pollp, int fd) {
    int rc;
    rc = diminuto_mux_unregister_read(&pollp->mux, fd);
    if (rc >= 0) { pollp->refresh = !0; }
    return rc;
}

/**
 * Unregister a file descriptor from the poller for writing.
 * @param pollp points to an initialized poller structure.
 * @param fd is a registered file descriptor.
 * @return >=0 for success, <0 for error.
 */
static inline int diminuto_poll_unregister_write(diminuto_poll_t * pollp, int fd) {
    int rc;
    rc = diminuto_mux_unregister_write(&pollp->mux, fd);
    if (rc >= 0) { pollp->refresh = !0; }
    return rc;
}

/**
 * Unregister a listening file descriptor from the poller for accepting
 * connections.
 * @param pollp points to an initialized poller structure.
 * @param fd is a registered file descriptor.
 * @return >=0 for success, <0 for error.
 */
static inline int diminuto_poll_unregister_accept(diminuto_poll_t * pollp, int fd) {
    int rc;
    rc = diminuto_mux_unregister_accept(&pollp->mux, fd);
    if (rc >= 0) { pollp->refresh = !0; }
    return rc;
}

/**
 * Unregister a file descriptor from the poller for urgent exceptions.
 * @param pollp points to an initialized poller structure.
 * @param fd is a registered file descriptor.
 * @return >=0 for success, <0 for error.
 */
static inline int diminuto_poll_unregister_urgent(diminuto_poll_t * pollp, int fd) {
    int rc;
    rc = diminuto_mux_unregister_urgent(&pollp->mux, fd);
    if (rc >= 0) { pollp->refresh = !0; }
    return rc;
}

/**
 * Unregister a file descriptor from the poller for interrupt exceptions.
 * @param pollp points to an initialized poller structure.
 * @param fd is a registered file descriptor.
 * @return >=0 for success, <0 for error.
 */
static inline int diminuto_poll_unregister_interrupt(diminuto_poll_t * pollp, int fd) {
    int rc;
    rc = diminuto_mux_unregister_interrupt(&pollp->mux, fd);
    if (rc >= 0) { pollp->refresh = !0; }
    return rc;
}

/**
 * Remove a signal from the mask of those to be atomically blocked while the
 * poller is waiting.
 * @param pollp points to an initialized poller structure.
 * @param signum is a registered signal to be unblocked.
 * @return >=0 for success, <0 for error.
 */
static inline int diminuto_poll_unregister_signal(diminuto_poll_t * pollp, int signum) {
    return diminuto_mux_unregister_signal(&pollp->mux, signum);
}

/**
 * Wait until one or more registered file descriptors are ready for reading,
 * writing, or accepting, a timeout occurs, or a signal interrupt occurs. A
 * timeout of zero returns immediately, which is useful for polling. A timeout
 * that is negative causes the poller to block indefinitely until either
 * a file descriptor is ready or a signal is caught. This is an alternative
 * API call that allows the application to provide its own signal mask (which
 * it may have for other reasons).
 * @param pollp points to an initialized poller structure.
 * @param timeout is a timeout period in ticks, 0 for polling, <0 for blocking.
 * @param maskp points to the signal mask, or NULL is none.
 * @return the number of ready file descriptors, 0 for a timeout, <0 for error.
 */
extern int diminuto_poll_wait_generic(diminuto_poll_t * pollp, diminuto_sticks_t timeout, const sigset_t * maskp);

/**
 * Wait until one or more registered file descriptors are ready for reading,
 * writing, or accepting, a timeout occurs, or a signal interrupt occurs. A
 * timeout of zero returns immediately, which is useful for polling. A timeout
 * that is negative causes the poller to block indefinitely until either
 * a file descriptor is ready or one of the registered signals is caught. This
 * API call uses the cached signal mask of registered signals in the poll
 * structure.
 * @param pollp points to an initialized poller structure.
 * @param timeout is a timeout period in ticks, 0 for polling, <0 for blocking.
 * @return the number of ready file descriptors, 0 for a timeout, <0 for error.
 */
static inline int diminuto_poll_wait(diminuto_poll_t * pollp, diminuto_sticks_t timeout)
{
    return diminuto_poll_wait_generic(pollp, timeout, &(pollp->mux.mask));
}

/**
 * Return the next registered file descriptor that is ready for reading.
 * @param pollp points to an initialized poller structure.
 * @return a file descriptor ready for reading, or <0 if none.
 */
static inline int diminuto_poll_ready_read(diminuto_poll_t * pollp) {
    return diminuto_mux_ready_read(&pollp->mux);
}

/**
 * Return the next registered file descriptor that is ready for writing.
 * @param pollp points to an initialized poller structure.
 * @return a file descriptor ready for writing, or <0 if none.
 */
static inline int diminuto_poll_ready_write(diminuto_poll_t * pollp) {
    return diminuto_mux_ready_write(&pollp->mux);
}

/**
 * Return the next registered file descriptor that is ready for accepting.
 * @param pollp points to an initialized poller structure.
 * @return a file descriptor ready for accepting, or <0 if none.
 */
static inline int diminuto_poll_ready_accept(diminuto_poll_t * pollp) {
    return diminuto_mux_ready_accept(&pollp->mux);
}

/**
 * Return the next registered file descriptor that is ready for urgent
 * exceptions. Typically this means there is urgent data available to be
 * received
 * out-of-band on the descriptor.
 * @param pollp points to an initialized poller structure.
 * @return a file descriptor ready for accepting, or <0 if none.
 */
static inline int diminuto_poll_ready_urgent(diminuto_poll_t * pollp) {
    return diminuto_mux_ready_urgent(&pollp->mux);
}

/**
 * Return the next registered file descriptor that is ready for interrupt
 * exceptions. Typically this means a GPIO pin has changed state.
 * @param pollp points to an initialized poller structure.
 * @return a file descriptor ready for accepting, or <0 if none.
 */
static inline int diminuto_poll_ready_interrupt(diminuto_poll_t * pollp) {
    return diminuto_mux_ready_interrupt(&pollp->mux);
}

/**
 * Unregister a registered file descriptor and close it. The file descriptor
 * is not closed if it is not registered.
 * @param pollp points to an initialized poller structure.
 * @param fd is a registered file descriptor.
 * @return 0 for success, <0 for error.
 */
static inline int diminuto_poll_close(diminuto_poll_t * pollp, int fd) {
    return diminuto_mux_close(&pollp->mux, fd);
}

/**
 * Dump a poller structure.
 * @param pollp points to the poller structure.
 */
extern void diminuto_poll_dump(diminuto_poll_t * pollp);

#endif
